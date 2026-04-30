/*
 * SD card library for ATmega328P — FAT16, SPI mode
 *
 * Supports SD v1, SD v2, and SDHC cards formatted as FAT16.
 * FAT32 is detected and rejected (SD_ERR_FAT).
 *
 * SRAM usage: ~550 bytes (512-byte sector buffer + ~38 bytes of state).
 *
 * Dependencies: <avr/io.h>, <util/delay.h>, <string.h>
 * F_CPU must be defined by the build system.
 *
 * Wiring (ATmega328P hardware SPI):
 *   MOSI  PB3
 *   MISO  PB4
 *   SCK   PB5
 *   CS    any pin — passed to SD_init()
 */

#include "sd.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

/* =========================================================================
 * SPI / CS
 * ========================================================================= */

static volatile uint8_t *_cs_port;
static uint8_t           _cs_mask;

#define cs_low()   (*_cs_port &= ~_cs_mask)
#define cs_high()  (*_cs_port |=  _cs_mask)

static uint8_t spi_byte(uint8_t b)
{
    SPDR = b;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

/* ≤400 kHz for card power-on / init sequence */
static void spi_slow(void)
{
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0); /* F_CPU/128 */
    SPSR = 0;
}

/* F_CPU/4 for normal operation (~4 MHz at 16 MHz) */
static void spi_fast(void)
{
    SPCR = (1 << SPE) | (1 << MSTR);
    SPSR = 0;
}

/* =========================================================================
 * SD command layer
 * ========================================================================= */

#define CMD0   0    /* GO_IDLE_STATE        */
#define CMD8   8    /* SEND_IF_COND         */
#define CMD16  16   /* SET_BLOCKLEN         */
#define CMD17  17   /* READ_SINGLE_BLOCK    */
#define CMD24  24   /* WRITE_SINGLE_BLOCK   */
#define CMD55  55   /* APP_CMD              */
#define CMD58  58   /* READ_OCR             */
#define ACMD41 41   /* SD_SEND_OP_COND      */

#define CARD_SD1  1
#define CARD_SD2  2
#define CARD_SDHC 3

static uint8_t _card_type;

/* Spin until MISO goes high (card not busy); returns final byte */
static uint8_t sd_wait(void)
{
    uint8_t r;
    uint16_t n = 0xFFFF;
    do { r = spi_byte(0xFF); } while (r != 0xFF && --n);
    return r;
}

/* Send a 6-byte SPI command while CS is already low; return R1 response.
   CRC matters only for CMD0 (0x95) and CMD8 (0x87); pass 0xFF otherwise. */
static uint8_t sd_cmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    sd_wait();
    spi_byte(0x40 | cmd);
    spi_byte((uint8_t)(arg >> 24));
    spi_byte((uint8_t)(arg >> 16));
    spi_byte((uint8_t)(arg >>  8));
    spi_byte((uint8_t)(arg      ));
    spi_byte(crc | 0x01);           /* stop bit always set */
    uint8_t r, n = 8;
    do { r = spi_byte(0xFF); } while ((r & 0x80) && --n);
    return r;
}

/* Send APP_CMD prefix (CMD55) then an ACMD; returns ACMD R1 response */
static uint8_t sd_acmd(uint8_t cmd, uint32_t arg)
{
    cs_low();
    sd_cmd(CMD55, 0, 0x65);
    cs_high();
    spi_byte(0xFF);
    cs_low();
    uint8_t r = sd_cmd(cmd, arg, 0x77);
    cs_high();
    spi_byte(0xFF);
    return r;
}

/* =========================================================================
 * 512-byte sector buffer (shared by all layers)
 * ========================================================================= */

static uint8_t _buf[512];

/* =========================================================================
 * FAT16 filesystem state
 * ========================================================================= */

static uint32_t _part_lba;          /* LBA of partition start (0 if no MBR)  */
static uint32_t _fat_lba;           /* LBA of FAT1                           */
static uint16_t _fat_sectors;       /* sectors per FAT copy                  */
static uint32_t _root_lba;          /* LBA of root directory                 */
static uint32_t _data_lba;          /* LBA of data area (cluster 2)          */
static uint8_t  _spc;               /* sectors per cluster                   */

/* Currently open file state */
static uint32_t _file_dir_lba;      /* LBA of directory sector holding entry */
static uint8_t  _file_dir_idx;      /* entry index (0-15) within that sector */
static uint32_t _file_size;         /* file size in bytes                    */
static uint16_t _file_cluster;      /* first cluster of file                 */
static uint16_t _file_last_cluster; /* last cluster (write pointer)          */

/* =========================================================================
 * Block I/O  (results stored in _buf)
 * ========================================================================= */

static uint8_t sd_read(uint32_t lba)
{
    uint32_t addr = (_card_type == CARD_SDHC) ? lba : (lba << 9);
    cs_low();
    if (sd_cmd(CMD17, addr, 0xFF) != 0x00) {
        cs_high(); spi_byte(0xFF);
        return SD_ERR_IO;
    }
    /* Wait for data token 0xFE */
    uint8_t t; uint16_t n = 0xFFFF;
    do { t = spi_byte(0xFF); } while (t != 0xFE && --n);
    if (!n) { cs_high(); spi_byte(0xFF); return SD_ERR_IO; }
    for (uint16_t i = 0; i < 512; i++) _buf[i] = spi_byte(0xFF);
    spi_byte(0xFF); spi_byte(0xFF);  /* discard CRC */
    cs_high();
    spi_byte(0xFF);
    return SD_OK;
}

static uint8_t sd_write(uint32_t lba)
{
    uint32_t addr = (_card_type == CARD_SDHC) ? lba : (lba << 9);
    cs_low();
    if (sd_cmd(CMD24, addr, 0xFF) != 0x00) {
        cs_high(); spi_byte(0xFF);
        return SD_ERR_IO;
    }
    spi_byte(0xFF);
    spi_byte(0xFE);                              /* data token */
    for (uint16_t i = 0; i < 512; i++) spi_byte(_buf[i]);
    spi_byte(0xFF); spi_byte(0xFF);              /* dummy CRC  */
    uint8_t r = spi_byte(0xFF);                  /* data response token */
    if ((r & 0x1F) != 0x05) {
        cs_high(); spi_byte(0xFF);
        return SD_ERR_IO;
    }
    uint16_t n = 0xFFFF;
    while (!spi_byte(0xFF) && --n);             /* wait for write to complete */
    cs_high();
    spi_byte(0xFF);
    return n ? SD_OK : SD_ERR_IO;
}

/* =========================================================================
 * FAT16 helpers
 * NOTE: all helpers load/store via _buf, so caller must not rely on
 *       _buf contents across these calls.
 * ========================================================================= */

/* Each 512-byte FAT sector holds 256 FAT16 entries (2 bytes each) */
static uint16_t fat_read(uint16_t cluster)
{
    if (sd_read(_fat_lba + cluster / 256)) return 0xFFFF;
    uint16_t off = (cluster % 256) * 2;
    return (uint16_t)_buf[off] | ((uint16_t)_buf[off + 1] << 8);
}

static uint8_t fat_write(uint16_t cluster, uint16_t val)
{
    uint32_t lba = _fat_lba + cluster / 256;
    if (sd_read(lba)) return SD_ERR_IO;
    uint16_t off = (cluster % 256) * 2;
    _buf[off]     = (uint8_t)(val);
    _buf[off + 1] = (uint8_t)(val >> 8);
    if (sd_write(lba)) return SD_ERR_IO;
    /* Mirror to FAT2 */
    return sd_write(lba + _fat_sectors);
}

/* Scan FAT for a free cluster, mark it EOF (0xFFFF), return cluster number.
   Returns 0 on disk-full or I/O error. */
static uint16_t fat_alloc(void)
{
    for (uint16_t s = 0; s < _fat_sectors; s++) {
        if (sd_read(_fat_lba + s)) return 0;
        for (uint16_t i = 0; i < 256; i++) {
            uint16_t cluster = s * 256 + (uint16_t)i;
            if (cluster < 2) continue;
            uint16_t off = i * 2;
            uint16_t val = (uint16_t)_buf[off] | ((uint16_t)_buf[off + 1] << 8);
            if (val == 0x0000) {
                _buf[off]     = 0xFF;
                _buf[off + 1] = 0xFF;
                if (sd_write(_fat_lba + s)) return 0;
                return cluster;
            }
        }
    }
    return 0; /* disk full */
}

/* Follow chain from cluster, freeing each entry (set to 0x0000). */
static void fat_free_chain(uint16_t cluster)
{
    while (cluster >= 2 && cluster < 0xFFF8) {
        uint16_t next = fat_read(cluster);
        fat_write(cluster, 0x0000);
        cluster = next;
    }
}

static uint32_t cluster_to_lba(uint16_t c)
{
    return _data_lba + (uint32_t)(c - 2) * _spc;
}

/* =========================================================================
 * SD_init
 * ========================================================================= */

uint8_t SD_init(volatile uint8_t *cs_ddr,
                volatile uint8_t *cs_port,
                uint8_t           cs_mask)
{
    _cs_port = cs_port;
    _cs_mask = cs_mask;

    /* CS pin: output, idle high */
    *cs_ddr  |= cs_mask;
    *cs_port |= cs_mask;

    /* SPI pins: MOSI (PB3), SCK (PB5), SS (PB2) output; MISO (PB4) input.
       SS must be output and high to stay in master mode. */
    DDRB  |= (1 << PB3) | (1 << PB5) | (1 << PB2);
    DDRB  &= ~(1 << PB4);
    PORTB |= (1 << PB2);

    spi_slow();

    /* ≥74 clock cycles with CS high before CMD0 */
    cs_high();
    for (uint8_t i = 0; i < 10; i++) spi_byte(0xFF);

    /* CMD0: software reset → R1 = 0x01 (idle) */
    cs_low();
    uint8_t r = sd_cmd(CMD0, 0, 0x95);
    cs_high();
    spi_byte(0xFF);
    if (r != 0x01) return SD_ERR_INIT;

    /* CMD8: check voltage range; distinguishes SD v2 from SD v1 */
    cs_low();
    r = sd_cmd(CMD8, 0x000001AA, 0x87);
    if (r == 0x01) {
        /* SD v2 capable: read trailing R7 bytes */
        uint8_t t[4];
        for (uint8_t i = 0; i < 4; i++) t[i] = spi_byte(0xFF);
        cs_high(); spi_byte(0xFF);
        if (t[2] != 0x01 || t[3] != 0xAA) return SD_ERR_INIT;
        /* ACMD41 with HCS bit until card leaves idle */
        uint8_t n = 200;
        do { _delay_ms(10); r = sd_acmd(ACMD41, 0x40000000); } while (r && --n);
        if (r) return SD_ERR_INIT;
        /* CMD58: read OCR to check CCS (SDHC flag) */
        cs_low();
        r = sd_cmd(CMD58, 0, 0xFF);
        uint8_t ocr0 = spi_byte(0xFF);
        spi_byte(0xFF); spi_byte(0xFF); spi_byte(0xFF);
        cs_high(); spi_byte(0xFF);
        if (r != 0x00) return SD_ERR_INIT;
        _card_type = (ocr0 & 0x40) ? CARD_SDHC : CARD_SD2;
    } else {
        cs_high(); spi_byte(0xFF);
        /* SD v1: ACMD41 without HCS */
        uint8_t n = 200;
        do { _delay_ms(10); r = sd_acmd(ACMD41, 0); } while (r && --n);
        if (r) return SD_ERR_INIT;
        _card_type = CARD_SD1;
    }

    /* CMD16: force 512-byte blocks (required for SD v1/v2, ignored for SDHC) */
    cs_low();
    r = sd_cmd(CMD16, 512, 0xFF);
    cs_high(); spi_byte(0xFF);
    if (r != 0x00 && _card_type != CARD_SDHC) return SD_ERR_INIT;

    spi_fast();
    return SD_OK;
}

/* =========================================================================
 * fat_init — parse MBR / BPB, populate FAT16 state
 * ========================================================================= */

static uint8_t fat_init(void)
{
    if (sd_read(0)) return SD_ERR_IO;
    if (_buf[510] != 0x55 || _buf[511] != 0xAA) return SD_ERR_FAT;

    _part_lba = 0;

    /* Detect MBR: boot sectors start with a jump opcode (0xEB or 0xE9).
       If the first byte is something else, assume an MBR partition table. */
    if (_buf[0] != 0xEB && _buf[0] != 0xE9) {
        /* MBR: partition 0 LBA start is at offset 0x1C6 */
        _part_lba = (uint32_t)_buf[0x1C6]
                  | ((uint32_t)_buf[0x1C7] <<  8)
                  | ((uint32_t)_buf[0x1C8] << 16)
                  | ((uint32_t)_buf[0x1C9] << 24);
        if (_part_lba == 0) return SD_ERR_FAT;
        if (sd_read(_part_lba)) return SD_ERR_IO;
    }

    /* --- Parse BIOS Parameter Block (BPB) --- */
    uint16_t bytes_per_sector  = (uint16_t)_buf[0x0B] | ((uint16_t)_buf[0x0C] << 8);
    uint8_t  spc               = _buf[0x0D];
    uint16_t reserved          = (uint16_t)_buf[0x0E] | ((uint16_t)_buf[0x0F] << 8);
    uint8_t  num_fats          = _buf[0x10];
    uint16_t root_entries      = (uint16_t)_buf[0x11] | ((uint16_t)_buf[0x12] << 8);
    uint16_t fat16_spf         = (uint16_t)_buf[0x16] | ((uint16_t)_buf[0x17] << 8);

    if (bytes_per_sector != 512) return SD_ERR_FAT;
    if (root_entries == 0)       return SD_ERR_FAT;  /* FAT32 not supported */

    _spc         = spc;
    _fat_sectors = fat16_spf;
    _fat_lba     = _part_lba + reserved;
    _root_lba    = _fat_lba  + (uint32_t)num_fats * fat16_spf;
    _data_lba    = _root_lba + (root_entries * 32 + 511) / 512;

    return SD_OK;
}

static uint8_t file_write(const uint8_t *data, uint8_t len);

/* =========================================================================
 * FAT 8.3 name helper — shared by SD_json_open and SD_json_create
 * ========================================================================= */

/* Build a FAT 8.3 name (11 bytes, space-padded, uppercase) from a base name.
   Extension is always "TXT" since FAT 8.3 supports only 3-character extensions
   and "JSON" is four characters. */
static void make_83(const char *name, uint8_t out[11])
{
    memset(out, ' ', 11);
    uint8_t i = 0;
    while (i < 8 && name[i]) {
        uint8_t c = (uint8_t)name[i++];
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        out[i - 1] = c;
    }
    out[8] = 'T'; out[9] = 'X'; out[10] = 'T';
}

/* =========================================================================
 * SD_json_open
 * ========================================================================= */

uint8_t SD_json_open(const char *name)
{
    uint8_t err = fat_init();
    if (err) return err;

    uint8_t target[11];
    make_83(name, target);

    uint32_t root_sectors = _data_lba - _root_lba;

    for (uint32_t s = 0; s < root_sectors; s++) {
        uint32_t lba = _root_lba + s;
        if (sd_read(lba)) return SD_ERR_IO;

        for (uint8_t e = 0; e < 16; e++) {
            uint8_t *ent   = _buf + (uint16_t)e * 32;
            uint8_t  first = ent[0];

            if (first == 0x00) return SD_ERR_NOTFOUND;
            if (first == 0xE5) continue;
            if (memcmp(ent, target, 11) != 0) continue;

            uint16_t first_cluster = (uint16_t)ent[0x1A] | ((uint16_t)ent[0x1B] << 8);
            uint32_t file_size     = (uint32_t)ent[0x1C]
                                   | ((uint32_t)ent[0x1D] <<  8)
                                   | ((uint32_t)ent[0x1E] << 16)
                                   | ((uint32_t)ent[0x1F] << 24);

            if (file_size < 3) return SD_ERR_FAT; /* too small to be "[\n]" */

            _file_dir_lba  = lba;
            _file_dir_idx  = e;
            _file_cluster  = first_cluster;
            _file_size     = file_size;

            /* Reject files not created by this library */
            if (sd_read(cluster_to_lba(first_cluster))) return SD_ERR_IO;
            if (_buf[0] != '[') return SD_ERR_NOTFOUND;

            /* Walk FAT chain to find the last cluster */
            uint16_t cluster = first_cluster;
            for (;;) {
                uint16_t next = fat_read(cluster); /* clobbers _buf */
                if (next >= 0xFFF8) break;
                cluster = next;
            }
            _file_last_cluster = cluster;

            /* Read the last sector and verify the file ends with \n] before
               rewinding.  last_idx > 0 guards the rare case where \n and ]
               straddle a sector boundary — skip rewind rather than reading
               two sectors. */
            uint32_t off_in_cluster = (file_size - 1) % ((uint32_t)_spc * 512);
            uint32_t last_lba       = cluster_to_lba(_file_last_cluster)
                                    + off_in_cluster / 512;
            uint16_t last_idx       = (uint16_t)((file_size - 1) % 512);

            if (sd_read(last_lba)) return SD_ERR_IO;

            if (last_idx > 0
                    && _buf[last_idx]     == ']'
                    && _buf[last_idx - 1] == '\n') {
                _file_size = file_size - 2;
            }

            return SD_OK;
        }
    }
    return SD_ERR_NOTFOUND;
}

/* =========================================================================
 * SD_json_create
 * ========================================================================= */

uint8_t SD_json_create(const char *name)
{
    uint8_t err = fat_init();
    if (err) return err;

    uint8_t target[11];
    make_83(name, target);

    uint32_t root_sectors = _data_lba - _root_lba;
    uint32_t free_lba = 0;
    uint8_t  free_idx = 0;
    uint8_t  found    = 0;
    uint8_t  done     = 0;

    for (uint32_t s = 0; s < root_sectors && !done; s++) {
        uint32_t lba = _root_lba + s;
        if (sd_read(lba)) return SD_ERR_IO;

        for (uint8_t e = 0; e < 16; e++) {
            uint8_t *ent = _buf + (uint16_t)e * 32;
            uint8_t  first = ent[0];

            if (first == 0x00) {
                /* Empty slot — all subsequent entries also empty */
                if (!free_lba) { free_lba = lba; free_idx = e; }
                done = 1;
                break;
            }
            if (first == 0xE5) {
                /* Deleted entry — usable slot */
                if (!free_lba) { free_lba = lba; free_idx = e; }
                continue;
            }
            if (!found && memcmp(ent, target, 11) == 0) {
                /* Found the same filename: free its cluster chain then delete */
                uint16_t fc = (uint16_t)ent[0x1A] | ((uint16_t)ent[0x1B] << 8);
                if (fc >= 2) fat_free_chain(fc); /* invalidates _buf */
                /* Re-read directory sector (fat_free_chain destroyed _buf) */
                if (sd_read(lba)) return SD_ERR_IO;
                ent = _buf + (uint16_t)e * 32;
                ent[0] = 0xE5;
                if (sd_write(lba)) return SD_ERR_IO;
                if (!free_lba) { free_lba = lba; free_idx = e; }
                found = 1;
            }
        }
    }

    if (!free_lba) return SD_ERR_FULL;

    /* Allocate the first cluster for the new file */
    uint16_t cluster = fat_alloc(); /* invalidates _buf */
    if (!cluster) return SD_ERR_FULL;

    /* Write the new directory entry */
    if (sd_read(free_lba)) return SD_ERR_IO;
    uint8_t *ent = _buf + (uint16_t)free_idx * 32;
    memset(ent, 0, 32);
    memcpy(ent, target, 11);
    ent[0x0B] = 0x20;                          /* archive attribute */
    ent[0x1A] = (uint8_t)(cluster);            /* first cluster low */
    ent[0x1B] = (uint8_t)(cluster >> 8);
    if (sd_write(free_lba)) return SD_ERR_IO;

    _file_dir_lba      = free_lba;
    _file_dir_idx      = free_idx;
    _file_size         = 0;
    _file_cluster      = cluster;
    _file_last_cluster = cluster;

    uint8_t open = '[';
    return file_write(&open, 1);
}

/* =========================================================================
 * Internal file write helper
 * Writes `len` bytes from `data` at the current end of the open file.
 * Updates _file_size and the directory entry file-size field.
 * ========================================================================= */

static uint8_t file_write(const uint8_t *data, uint8_t len)
{
    uint32_t cluster_bytes = (uint32_t)_spc * 512;
    uint8_t  written = 0;

    while (written < len) {
        uint32_t off_in_cluster = _file_size % cluster_bytes;

        /* If we are exactly at a cluster boundary (and the file already has
           data), allocate a fresh cluster and link it. */
        if (_file_size > 0 && off_in_cluster == 0) {
            uint16_t nc = fat_alloc();      /* invalidates _buf */
            if (!nc) return SD_ERR_FULL;
            if (fat_write(_file_last_cluster, nc)) return SD_ERR_IO;
            _file_last_cluster = nc;
            off_in_cluster = 0;
        }

        uint8_t  sec_in_cluster = (uint8_t)(off_in_cluster / 512);
        uint16_t byte_in_sector = (uint16_t)(_file_size % 512);
        uint32_t write_lba      = cluster_to_lba(_file_last_cluster) + sec_in_cluster;

        uint16_t space    = 512 - byte_in_sector;
        uint8_t  to_write = (uint8_t)((len - written) < space ? (len - written) : space);

        if (sd_read(write_lba)) return SD_ERR_IO;
        memcpy(_buf + byte_in_sector, data + written, to_write);
        if (sd_write(write_lba)) return SD_ERR_IO;

        written    += to_write;
        _file_size += to_write;
    }

    /* Flush updated file size back to the directory entry */
    if (sd_read(_file_dir_lba)) return SD_ERR_IO;
    uint8_t *ent = _buf + (uint16_t)_file_dir_idx * 32;
    ent[0x1C] = (uint8_t)(_file_size);
    ent[0x1D] = (uint8_t)(_file_size >>  8);
    ent[0x1E] = (uint8_t)(_file_size >> 16);
    ent[0x1F] = (uint8_t)(_file_size >> 24);
    return sd_write(_file_dir_lba);
}

/* =========================================================================
 * SD_json_append
 * ========================================================================= */

uint8_t SD_json_append(const char *key, const char *value)
{
    uint8_t line[71];
    uint8_t pos = 0;

    line[pos++] = '\n';
    line[pos++] = '{';
    line[pos++] = '"';
    for (uint8_t i = 0; key[i]   && pos < 56; i++) line[pos++] = (uint8_t)key[i];
    line[pos++] = '"';
    line[pos++] = ':';
    line[pos++] = ' ';
    for (uint8_t i = 0; value[i] && pos < 68; i++) line[pos++] = (uint8_t)value[i];
    line[pos++] = '}';
    line[pos++] = ',';

    return file_write(line, pos);
}

/* =========================================================================
 * SD_json_close
 * ========================================================================= */

uint8_t SD_json_close(void)
{
    uint8_t tail[2] = {'\n', ']'};
    return file_write(tail, 2);
}
