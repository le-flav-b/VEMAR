#ifndef SD_H
#define SD_H

#include <stdint.h>

/* Return codes */
#define SD_OK        0
#define SD_ERR_INIT  1  /* card init / power-on sequence failed  */
#define SD_ERR_IO    2  /* block read or write failed            */
#define SD_ERR_FAT   3  /* filesystem not FAT16 (try FAT32?)    */
#define SD_ERR_FULL      4  /* no free directory slot or cluster     */
#define SD_ERR_NOTFOUND  5  /* named file does not exist             */

/**
 * Initialize SD card and configure hardware SPI (ATmega328P).
 * SPI is set to MODE0, MSB first. Call once before any other SD function.
 * After init the SPI bus runs at F_CPU/4 (~4 MHz at 16 MHz).
 *
 * @param cs_ddr   Pointer to DDR register of CS pin  (e.g. &DDRB)
 * @param cs_port  Pointer to PORT register of CS pin (e.g. &PORTB)
 * @param cs_mask  Bitmask of the CS pin              (e.g. (1 << PB2))
 * @return SD_OK on success, SD_ERR_INIT on failure
 */
uint8_t SD_init(volatile uint8_t *cs_ddr,
                volatile uint8_t *cs_port,
                uint8_t           cs_mask);

/**
 * Open an existing file for appending.
 * Finds the file, walks its FAT chain to locate the last cluster, then
 * rewinds the write position past the trailing '\n]' written by
 * SD_json_close() so that subsequent SD_json_append() calls extend the
 * array.  Call SD_json_close() again when done.
 *
 * Returns SD_ERR_NOTFOUND if the file does not exist — use that as a
 * signal to call SD_json_create() instead.
 *
 * @param name  Base name, max 8 ASCII chars (e.g. "SENSORS")
 * @return SD_OK on success, SD_ERR_NOTFOUND if absent, error code otherwise
 */
uint8_t SD_json_open(const char *name);

/**
 * Create (or overwrite) a file in the SD root directory.
 * FAT 8.3 filenames support only a 3-character extension, so the file is
 * created with a ".TXT" extension regardless of the name you pass — the
 * content is still valid JSON.  Example: "SENSORS" → SENSORS.TXT
 *
 * @param name  Base name, max 8 ASCII chars, no extension (e.g. "SENSORS")
 * @return SD_OK on success, error code on failure
 */
uint8_t SD_json_create(const char *name);

/**
 * Append one JSON object to the open array.
 * The value is written unquoted — pass the numeric string directly.
 * Handles commas automatically: no comma before the first entry,
 * comma+newline before every subsequent one.
 *
 * Example:
 *   SD_json_append("temp",     "32.07");
 *   SD_json_append("humidity", "65.30");
 *   SD_json_close();
 *
 * Produces:
 *   [
 *   {"temp": 32.07},
 *   {"humidity": 65.30},
 *   ]
 *
 * @param key    Field name  (e.g. "temp")
 * @param value  Field value as string (e.g. "32.07")
 * @return SD_OK on success, error code on failure
 */
uint8_t SD_json_append(const char *key, const char *value);

/**
 * Close the JSON array by writing '\n]'.
 * Call once when logging is complete. After this the file is valid JSON.
 * @return SD_OK on success, error code on failure
 */
uint8_t SD_json_close(void);

#endif /* SD_H */
