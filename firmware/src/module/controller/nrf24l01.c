#include "nrf24l01.h"
#include "serial.h"
#include "spi.h"

#define NRF24L01_DELAY_POWERUP 1500 ///< Start up 1500us
#define NRF24L01_DELAY_RX 130       ///< RX Settling for 130us
#define NRF24L01_DELAY_TX 150       ///< TX Settling for 130us

#define R_REGISTER 0x00         ///< Read command and `STATUS` register
#define W_REGISTER 0x20         ///< Write command and `STATUS` register
#define R_RX_PAYLOAD 0x61       ///< Read RX payload
#define W_TX_PAYLOAD 0xA0       ///< Write TX payload
#define FLUSH_TX 0xE1           ///< Flush TX FIFO, used in TX mode
#define FLUSH_RX 0xE2           ///< Flush RX FIFO, used in RX mode
#define REUSE_TX_PL 0xE3        ///< Reuse last transmitted payload
#define ACTIVATE 0x50           ///< Activate `R_RX_PL_WID`, `W_ACK_PAYLOAD`, `W_TX_PAYLOAD_NOACK`
#define R_RX_PL_WID 0x60        ///< Read RX payload width
#define W_ACK_PAYLOAD 0xA8      ///< Write Payload to be transmitted
#define W_TX_PAYLOAD_NOACK 0xB0 ///< Disable `AUTOACK` on this specific packet
#define NOP 0xFF                ///< No Operation

#define CONFIG 0x00      ///< Configuration Register
#define EN_AA 0x01       ///< Enable 'Auto-Acknowledgment'
#define EN_RXADDR 0x02   ///< Enabled RX Addresses
#define SETUP_AW 0x03    ///< Setup of Address Widths
#define SETUP_RETR 0x04  ///< Setup of Automatic Retransmission
#define RF_CH 0x05       ///< RF Channel
#define RF_SETUP 0x06    ///< RF Setup Register
#define STATUS 0x07      ///< Status Register
#define OBSERVE_TX 0x08  ///< Transmit Observe register
#define CD 0x09          ///< Carrier Detect
#define RX_ADDR_P0 0x0A  ///< Receive address data pipe 0
#define RX_ADDR_P1 0x0B  ///< Receive address data pipe 1
#define RX_ADDR_P2 0x0C  ///< Receive address data pipe 2
#define RX_ADDR_P3 0x0D  ///< Receive address data pipe 3
#define RX_ADDR_P4 0x0E  ///< Receive address data pipe 4
#define RX_ADDR_P5 0x0F  ///< Receive address data pipe 5
#define TX_ADDR 0x10     ///< Transmit address
#define RX_PW_P0 0x11    ///< Number of bytes in RX payload in data pipe 0
#define RX_PW_P1 0x12    ///< Number of bytes in RX payload in data pipe 1
#define RX_PW_P2 0x13    ///< Number of bytes in RX payload in data pipe 2
#define RX_PW_P3 0x14    ///< Number of bytes in RX payload in data pipe 3
#define RX_PW_P4 0x15    ///< Number of bytes in RX payload in data pipe 4
#define RX_PW_P5 0x16    ///< Number of bytes in RX payload in data pipe 5
#define FIFO_STATUS 0x17 ///< FIFO Status Register
#define DYNPD 0x1C       ///< Enable dynamic payload length
#define FEATURE 0x1D     ///< Feature Register

#define PRIM_RX 0
#define PWR_UP 1 ///< 1: POWER UP; 0: POWER DOWN
#define CRCO 2   ///< CRC encoding scheme
#define EN_CRC 3 ///< Enable CRC

#define RX_EMPTY 0 ///< RX FIFO empty flag
#define RX_FULL 1  ///< RX FIFO full flag
#define TX_EMPTY 4 ///< TX FIFO empty flag
#define TX_FULL 5  ///< TX FIFO full flag
#define TX_REUSE 6 ///< Reuse last transmitted data packet if set high

#define LNA_HCURR 0 ///< Setup LNA gain
#define RF_DR 3     ///< Air Data Rate (0: 1Mbps, 1: 2Mbps)

byte_t nrf24l01_csn;    ///< CSN pin
byte_t nrf24l01_ce;     ///< CE pin
byte_t nrf24l01_config; ///< CONFIG register
// byte_t nrf24l01_rf;     ///< RF_SETUP register
byte_t nrf24l01_addr_rx[5];
byte_t nrf24l01_addr_tx[5];
byte_t nrf24l01_aw; ///< RX/TX Address width

// enum nrf24l01_enum_mode
// {
//     NRF24L01_MODE_PD,    ///< Power Down
//     NRF24L01_MODE_STBY1, ///< Standby I
//     NRF24L01_MODE_STBY2, ///< Standby II
//     NRF24L01_MODE_RX,    ///< RX Mode
//     NRF24L01_MODE_TX     ///< TX Mode
// } nrf24l01_mode;


static byte_t *NRF24L01_copy_address(byte_t *dst, const byte_t *src, length_t len)
{
    for (length_t i = 0; i < len; ++i)
    {
        dst[i] = src[i];
    }
    return (dst);
}

static inline void NRF24L01_spi_start(void)
{
    PIN_write(nrf24l01_csn, PIN_LOW);
}

static inline void NRF24L01_spi_stop(void)
{
    PIN_write(nrf24l01_csn, PIN_HIGH);
}

static byte_t NRF24L01_get_register(byte_t reg)
{
    NRF24L01_spi_start();
    SPI_transmit(R_REGISTER | reg);
    byte_t retval = SPI_receive();
    NRF24L01_spi_stop();
    return (retval);
}

static void NRF24L01_set_register(byte_t reg, byte_t value)
{
    NRF24L01_spi_start();
    SPI_transmit(W_REGISTER | reg);
    SPI_transmit(value);
    NRF24L01_spi_stop();
}

// static void NRF24L01_read_register(byte_t reg, byte_t *dst, length_t len)
// {
//     NRF24L01_spi_start();
//     SPI_transmit(R_REGISTER | reg);
//     SPI_read(dst, len);
//     NRF24L01_spi_stop();
// }

static void NRF24L01_write_register(byte_t reg, const byte_t *src, length_t len)
{
    NRF24L01_spi_start();
    SPI_transmit(W_REGISTER | reg);
    SPI_write(src, len);
    // while (len > 0)
    // {
    //     --len;
    //     SPI_transmit(buff[len]);
    // }
    NRF24L01_spi_stop();
}

void NRF24L01_init(pin_t ce, pin_t csn)
{
    nrf24l01_ce = ce;
    nrf24l01_csn = csn;

    // set `ce` low and `csn` high
    PIN_mode(nrf24l01_ce, PIN_OUTPUT);
    PIN_mode(nrf24l01_csn, PIN_OUTPUT);
    PIN_write(nrf24l01_ce, PIN_LOW);
    PIN_write(nrf24l01_csn, PIN_HIGH);

    SPI_init();

    delay(NRF24L01_DELAY_POWERUP); // wait for NRF24L01 to stablilize

    // enable CRC
    NRF24L01_set_register(CONFIG, BIT(EN_CRC) | BIT(CRCO)); // enable CRC
    // byte_t crc = (BIT(EN_CRC) | BIT(CRCO));
    // NRF24L01_write_register(CONFIG, &crc, 1);

    nrf24l01_config = NRF24L01_get_register(CONFIG); // read CONFIG register
    // nrf24l01_rf = NRF24L01_read_register(RF_SETUP); // read RF_SETUP register
    nrf24l01_aw = NRF24L01_get_register(SETUP_AW) + 2; // address width
    // nrf24l01_mode = NRF24L01_MODE_STBY1;
}

void NRF24L01_mode_rx(void)
{
    // if (NRF24L01_MODE_RX == nrf24l01_mode)
    // {
        BIT_set(nrf24l01_config, BIT(PRIM_RX));
        NRF24L01_set_register(CONFIG, nrf24l01_config);
        NRF24L01_enable();
        _delay_us(NRF24L01_DELAY_RX);
    //     nrf24l01_mode = NRF24L01_MODE_RX;
    // }
}

void NRF24L01_mode_tx(void)
{
    // if (NRF24L01_MODE_TX != nrf24l01_mode)
    // {
        BIT_clear(nrf24l01_config, BIT(PRIM_RX));
        NRF24L01_set_register(CONFIG, nrf24l01_config);
        NRF24L01_enable();
        _delay_us(NRF24L01_DELAY_TX);
    //     nrf24l01_mode = NRF24L01_MODE_TX;
    // }
SERIAL_print(str, "/tx CE: ");
SERIAL_println(uint, PIN_read(nrf24l01_ce));
}

void NRF24L01_standby(void)
{
    // if (NRF24L01_MODE_STBY1 != nrf24l01_mode)
    // {
        NRF24L01_clear_status();
        NRF24L01_disable();
        NRF24L01_flush_rx();
        NRF24L01_flush_tx();
    //     nrf24l01_mode = NRF24L01_MODE_STBY1;
    // }
// SERIAL_print(str, "/standby CE: ");
// SERIAL_println(uint, PIN_read(nrf24l01_ce));
}

void NRF24L01_setup(rf_rate_t rate, rf_power_t power, bool_t lna)
{
    byte_t setup = (rate) | (power) | (lna ? 1 : 0);
    NRF24L01_write_register(RF_SETUP, &setup, 1);
}

byte_t NRF24L01_status(void)
{
    return (NRF24L01_get_register(STATUS));
}

void NRF24L01_clear_status(void)
{
    byte_t clr = (NRF24L01_RX_DR | NRF24L01_TX_DS | NRF24L01_MAX_RT);
    NRF24L01_write_register(STATUS, &clr, 1);
}

void NRF24L01_enable(void)
{
    PIN_write(nrf24l01_ce, PIN_HIGH);
}

void NRF24L01_disable(void)
{
    PIN_write(nrf24l01_ce, PIN_LOW);
}


void NRF24L01_primary_rx(void)
{
    BIT_set(nrf24l01_config, BIT(PRIM_RX));
    NRF24L01_write_register(CONFIG, &nrf24l01_config, 1);
}

void NRF24L01_primary_tx(void)
{
    BIT_clear(nrf24l01_config, BIT(PRIM_RX));
    NRF24L01_write_register(CONFIG, &nrf24l01_config, 1);

    NRF24L01_write_register(RX_ADDR_P0, nrf24l01_addr_tx, nrf24l01_aw);
    byte_t rxaddr = NRF24L01_get_register(EN_RXADDR) | BIT(NRF24L01_PIPE_0);
    NRF24L01_write_register(EN_RXADDR, &rxaddr, 1);
}

void NRF24L01_set_frequency(length_t frequency)
{
    frequency = (frequency & 0x7F); // bits [6:0]
    NRF24L01_write_register(RF_CH, &frequency, 1);
}

// void NRF24L01_set_pa(pa_t level)
// {
//     BIT_set(nrf24l01_rf, (level & 0x03) << 1); // bits [2:1]
//     NRF24L01_write_register(RF_SETUP, &nrf24l01_rf, 1);
// }

// void NRF24L01_enable_lna(void)
// {
//     BIT_set(nrf24l01_rf, BIT(LNA_HCURR));
//     NRF24L01_write_register(RF_SETUP, &nrf24l01_rf, 1);
// }

// void NRF24L01_disable_lna(void)
// {
//     BIT_clear(nrf24l01_rf, BIT(LNA_HCURR));
//     NRF24L01_write_register(RF_SETUP, &nrf24l01_rf, 1);
// }

void NRF24L01_flush_tx(void)
{
    NRF24L01_spi_start();
    SPI_transmit(FLUSH_TX);
    NRF24L01_spi_stop();
}

void NRF24L01_flush_rx(void)
{
    NRF24L01_spi_start();
    SPI_transmit(FLUSH_RX);
    NRF24L01_spi_stop();
}

bool_t NRF24L01_is_rx_empty(void)
{
    return (0 != (NRF24L01_get_register(FIFO_STATUS) & BIT(RX_EMPTY)));
}

bool_t NRF24L01_is_tx_empty(void)
{
    return (0 != (NRF24L01_get_register(FIFO_STATUS) & BIT(TX_EMPTY)));
}

bool_t NRF24L01_is_rx_full(void)
{
    return (0 != (NRF24L01_get_register(FIFO_STATUS) & BIT(RX_FULL)));
}

bool_t NRF24L01_is_tx_full(void)
{
    return (0 != (NRF24L01_get_register(FIFO_STATUS) & BIT(TX_FULL)));
}

void NRF24L01_read_payload(byte_t *buff, length_t len)
{
    NRF24L01_spi_start();
    SPI_transmit(R_RX_PAYLOAD);
    SPI_read(buff, len);
    // while (len > 0)
    // {
    //     --len;
    //     buff[len] = SPI_receive();
    // }
    NRF24L01_spi_stop();
}

// void NRF24L01_set_data_rate(data_rate_t rate)
// {
//     byte_t setup = NRF24L01_read_register(RF_SETUP);
//     BIT_write(setup, rate, BIT(RF_DR));
//     NRF24L01_write_register(RF_SETUP, &setup, 1);
// }

void NRF24L01_write_payload(const byte_t *buff, length_t len)
{
    NRF24L01_spi_start();
    SPI_transmit(W_TX_PAYLOAD);

    // (void)buff;
    // (void)len;
    SPI_write(buff, len);

    NRF24L01_spi_stop();
}

static void NRF24L01_print_register(const char *tag, byte_t reg, bool_t hex)
{
    byte_t value = NRF24L01_get_register(reg);
    SERIAL_print(str, tag);
    if (hex)
    {
        SERIAL_println(hex, value, 2);
    }
    else
    {
        SERIAL_println(uint, value);
    }
}

void NRF24L01_power_up(void)
{
    // if (NRF24L01_MODE_PD == nrf24l01_mode)
    // {
        NRF24L01_disable();
        BIT_set(nrf24l01_config, BIT(PWR_UP));
        NRF24L01_write_register(CONFIG, &nrf24l01_config, 1);
        _delay_us(NRF24L01_DELAY_POWERUP);
    //    nrf24l01_mode = NRF24L01_MODE_STBY1;
    // }
}

void NRF24L01_power_down(void)
{
    // if (NRF24L01_MODE_PD != nrf24l01_mode)
    // {
        BIT_clear(nrf24l01_config, BIT(PWR_UP));
        NRF24L01_write_register(CONFIG, &nrf24l01_config, 1);
        NRF24L01_disable();
    //     nrf24l01_mode = NRF24L01_MODE_PD;
    // }
}

void NRF24L01_set_payload_size(pipe_t pipe, length_t len)
{
    NRF24L01_write_register(RX_PW_P0 + pipe, &len, 1);
}

void NRF24L01_disable_pipe(pipe_t pipe)
{
    NRF24L01_set_payload_size(pipe, 0);
    // NRF24L01_write_register(RX_PW_P0 + pipe, 0);
}

void NRF24L01_disable_retransmit(void)
{
    NRF24L01_set_retransmit(0, 0);
    // NRF24L01_write_register(SETUP_RETR, 0);
}

void NRF24L01_set_retransmit(length_t count, length_t delay)
{
    byte_t retr = ((count & 0x0F) | ((delay & 0x0F) << 4));
    NRF24L01_write_register(SETUP_RETR, &retr, 1);
}

void NRF24L01_set_address_rx(pipe_t pipe, const byte_t *addr)
{
    if (2 > pipe)
    {
        NRF24L01_write_register(RX_ADDR_P0 + pipe, addr, nrf24l01_aw);
    } // if pipe 0 or 1
    else
    {
        NRF24L01_write_register(RX_ADDR_P0 + pipe, addr, 1);
    }
    byte_t rxaddr = (NRF24L01_get_register(EN_RXADDR) | BIT(pipe));
    NRF24L01_write_register(EN_RXADDR, &rxaddr, 1);
}

void NRF24L01_set_address_tx(const byte_t *addr)
{
    NRF24L01_write_register(RX_ADDR_P0, addr, nrf24l01_aw);
    NRF24L01_write_register(TX_ADDR, addr, nrf24l01_aw);
    NRF24L01_copy_address(nrf24l01_addr_tx, addr, nrf24l01_aw);
}

static void NRF24L01_print_status(void)
{
    byte_t status = NRF24L01_get_register(STATUS);
    SERIAL_print(str, "STATUS     = 0x");
    SERIAL_print(hex, status, 2);
    SERIAL_print(str, " RX_DR=");
    SERIAL_print(uint, (status & NRF24L01_RX_DR) >> 5);
    SERIAL_print(str, " TX_DS=");
    SERIAL_print(uint, (status & NRF24L01_MAX_RT) >> 4);
    SERIAL_print(str, " RX_PIPE=");
    SERIAL_print(uint, (status & 0x0E) >> 1);
    SERIAL_print(str, " TX_FULL=");
    SERIAL_println(uint, status & 0x01);
}

static void NRF24L01_print_address(pipe_t pipe)
{
    byte_t addr[5];
    NRF24L01_spi_start();
    SPI_transmit(R_REGISTER | (RX_ADDR_P0 + pipe));
    if (pipe < 2)
    {
        SPI_read(addr, nrf24l01_aw);
    }
    else
    {
        addr[nrf24l01_aw - 1] = SPI_receive();
    }
    NRF24L01_spi_stop();
    SERIAL_print(str, "PIPE ");
    SERIAL_print(uint, pipe);
    SERIAL_print(str, ": 0x");
    for (length_t i = 0; i < nrf24l01_aw; ++i)
    {
        SERIAL_print(hex, addr[nrf24l01_aw - 1 - i], 2);
        if (pipe > 1)
        {
            break;
        }
    }
    SERIAL_println(str, "");
}

static void NRF24L01_print_address_tx(void)
{
    byte_t addr[5];
    NRF24L01_spi_start();
    SPI_transmit(R_REGISTER | TX_ADDR);
    SPI_read(addr, nrf24l01_aw);
    NRF24L01_spi_stop();
    SERIAL_print(str, "TX_ADDR    = 0x");
    for (length_t i = 0; i < nrf24l01_aw; ++i)
    {
        SERIAL_print(hex, addr[nrf24l01_aw - 1 - i], 2);
    }
    SERIAL_println(str, "");
}

static void NRF24L01_print_setup(void)
{
    byte_t setup = NRF24L01_get_register(RF_SETUP);
    if (BIT_is_set(setup, BIT(RF_DR)))
    {
        SERIAL_println(str, "Data Rate  = 2 MBPS");
    }
    else
    {
        SERIAL_println(str, "Data Rate  = 1 MBPS");
    }
    switch ((setup & 0x06) >> 1)
    {
    case 0x00:
        SERIAL_println(str, "RF output  = -18dBm");
        break;
    case 0x01:
        SERIAL_println(str, "RF output  = -12dBm");
        break;
    case 0x02:
        SERIAL_println(str, "RF output  = -6dBm");
        break;
    case 0x03:
        SERIAL_println(str, "RF output  = 0dBm");
        break;
    default:
        break;
    }
    if (BIT_is_set(setup, BIT(LNA_HCURR)))
    {
        SERIAL_println(str, "LNA gain   = enabled");
    }
    else
    {
        SERIAL_println(str, "LNA gain   = disabled");
    }
}

void NRF24L01_print(void)
{
    NRF24L01_print_status();
    for (length_t i = 0; i < 6; ++i)
    {
        NRF24L01_print_address(i);
    }
    NRF24L01_print_address_tx();
    SERIAL_println(str, "Payload width:");
    NRF24L01_print_register(" - Pipe 0  = ", RX_PW_P0, 0);
    NRF24L01_print_register(" - Pipe 1  = ", RX_PW_P1, 0);
    NRF24L01_print_register(" - Pipe 2  = ", RX_PW_P2, 0);
    NRF24L01_print_register(" - Pipe 3  = ", RX_PW_P3, 0);
    NRF24L01_print_register(" - Pipe 4  = ", RX_PW_P4, 0);
    NRF24L01_print_register(" - Pipe 5  = ", RX_PW_P5, 0);
    NRF24L01_print_register("EN_AA      = 0x", EN_AA, 1);
    NRF24L01_print_register("EN_RXADDR  = 0x", EN_RXADDR, 1);
    NRF24L01_print_register("RF_CH      = 0x", RF_CH, 1);
    NRF24L01_print_register("RF_SETUP   = 0x", RF_SETUP, 1);
    NRF24L01_print_register("CONFIG     = 0x", CONFIG, 1);
    NRF24L01_print_register("DYNPD      = 0x", DYNPD, 1);
    NRF24L01_print_register("FEATURE    = 0x", FEATURE, 1);
    NRF24L01_print_setup();

    NRF24L01_print_register("SETUP_AW   = 0x", SETUP_AW, 1);
    NRF24L01_print_register("SETUP_RETR = 0x", SETUP_RETR, 1);
    NRF24L01_print_register("OBSERVE_TX = 0x", OBSERVE_TX, 1);
}


void NRF24L01_debug_config(void)
{
    byte_t config = NRF24L01_get_register(CONFIG);
    SERIAL_println(str, "CONFIG:");
    SERIAL_print(str, " - MASK_RX_DR  = ");
    SERIAL_println(bool, BIT_is_set(config, (1 << 6)));
    SERIAL_print(str, " - MASK_TX_DS  = ");
    SERIAL_println(bool, BIT_is_set(config, (1 << 5)));
    SERIAL_print(str, " - MASK_MAX_RT = ");
    SERIAL_println(bool, BIT_is_set(config, (1 << 4)));
    SERIAL_print(str, " - EN_CRC      = ");
    SERIAL_println(bool, BIT_is_set(config, (1 << 3)));
    SERIAL_print(str, " - CRC         = ");
    SERIAL_println(uint, (config & 0x04) / 4 + 1);
    SERIAL_print(str, " - PWR_UP      = ");
    SERIAL_println(bool, BIT_is_set(config, (1 << 1)));
    SERIAL_print(str, " - PRIM_RX     = ");
    SERIAL_println(bool, BIT_is_set(config, (1 << 0)));
}
