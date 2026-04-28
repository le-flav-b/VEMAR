#include "i2c.h"

#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
static int8_t i2c_wait_bus_idle(void) {
  uint32_t guard = 0;
  while ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc) { // wait until bus is idle
    if (++guard > 100000UL) {
      return I2C_ERR_TIMEOUT; // timeout waiting for bus idle
    }
  }
  return 0;
}

static int8_t i2c_wait_mstatus(uint8_t flag) {
  uint32_t guard = 0;
  while (!(TWI0.MSTATUS & flag)) { // wait until the requested status flag is set
    if (++guard > 100000UL) {
      return I2C_ERR_TIMEOUT; // timeout waiting for flag
    }
  }
  return 0;
}
#endif


#if defined(__AVR_ATmega328P__)
static int8_t i2c_wait_twint(void) {
  uint32_t guard = 0;
  while (!(TWCR & (1 << TWINT))) { // wait until TWI interrupt flag is set (operation complete)
    if (++guard > 100000UL) {
      return -5; // timeout waiting for hardware flag
    }
  }
  return 0;
}
#endif

void i2c_init() {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLA |= TWI_ENABLE_bm;        // enable master
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc; // force bus state to idle
  TWI0.MBAUD = TWI_BAUD;               // set baud rate
#elif defined(__AVR_ATmega328P__)
  TWBR = TWBR_VAL;    // set baud rate register
  TWSR = 0;           // prescaler = 1
  TWCR = (1 << TWEN); // enable TWI
#endif
}

void i2c_init_slave(uint8_t addr) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SADDR = addr << 1;                                        // set slave address (shifted left, LSB is R/W bit)
  TWI0.SCTRLA |= TWI_ENABLE_bm | TWI_APIEN_bm | TWI_DIEN_bm;   // enable slave, address/stop interrupt, data interrupt
#elif defined(__AVR_ATmega328P__)
  TWAR = (addr << 1) | (0 << TWGCE); // set slave address, disable general call recognition
  TWCR = (1 << TWEA) | (1 << TWEN);  // enable TWI and ACK
#endif
}

void i2c_stop_interface(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  #ifdef I2C_MASTER
    TWI0.MCTRLA &= ~TWI_ENABLE_bm; // disable master
  #endif
  #ifdef I2C_SLAVE
    TWI0.SCTRLA &= ~TWI_ENABLE_bm; // disable slave
  #endif
#elif defined(__AVR_ATmega328P__)
  TWCR &= ~(1 << TWEN); // disable TWI
#endif
}

// used when switching from slave to master
void i2c_switch_to_master(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SCTRLA &= ~TWI_ENABLE_bm;       // disable slave
  TWI0.MCTRLA |= TWI_ENABLE_bm;        // enable master
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc; // force bus state to idle
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWEN); // enable TWI as master (no TWEA = no slave ACK)
#endif
}

// used when switching from master to slave
void i2c_switch_to_slave(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLA &= ~TWI_ENABLE_bm;                                 // disable master
  TWI0.SCTRLA |= TWI_ENABLE_bm | TWI_APIEN_bm | TWI_DIEN_bm;   // enable slave, address/stop interrupt, data interrupt
#elif defined(__AVR_ATmega328P__)
  TWAR = (SLAVE_ADDR << 1) | (0 << TWGCE); // set slave address, disable general call recognition
  TWCR = (1 << TWEA) | (1 << TWEN);        // enable TWI and ACK
#endif
}

int8_t i2c_start(uint8_t addr_rw, bool_t restart) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  (void)restart;
  if (i2c_wait_bus_idle() != 0) return I2C_ERR_TIMEOUT; // wait for bus idle before sending START
  TWI0.MADDR = addr_rw;                                         // write address+R/W to MADDR, hardware sends START condition
  if (i2c_wait_mstatus(TWI_WIF_bm) != 0) return I2C_ERR_TIMEOUT; // wait for address phase to complete
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return I2C_ERR_ARBLOST;  // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return I2C_ERR_BUSERR;    // bus error
  if (TWI0.MSTATUS & TWI_RXACK_bm) return I2C_ERR_NACK;        // slave NACKed address
#elif defined(__AVR_ATmega328P__)
  uint8_t expected_addr_ack = (addr_rw & 0x01) ? TW_MR_SLA_ACK : TW_MT_SLA_ACK; // expect SLA+R ACK or SLA+W ACK

  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); // send START condition
  if (i2c_wait_twint() != 0) return I2C_ERR_TIMEOUT;
  if (restart) {
    if ((TWSR & 0xF8) != 0x10) return I2C_ERR_ARBLOST; // RESTART not acknowledged
  }
  else {
    if ((TWSR & 0xF8) != 0x08) return I2C_ERR_ARBLOST; // START not acknowledged
  }
  TWDR = addr_rw;                      // load address + R/W bit into data register
  TWCR = (1<<TWINT) | (1<<TWEN);      // transmit address
  if (i2c_wait_twint() != 0) return I2C_ERR_TIMEOUT;
  if ((TWSR & 0xF8) != expected_addr_ack) return I2C_ERR_NACK; // slave NACKed address
#endif
  return 0;
}

void i2c_stop(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLB |= TWI_MCMD_STOP_gc;              // send STOP condition
#elif defined(__AVR_ATmega328P__)
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);  // send STOP condition
#endif
}

int8_t i2c_write(uint8_t data) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MDATA = data;                                        // load byte into data register, hardware transmits it
  if (i2c_wait_mstatus(TWI_WIF_bm) != 0) return I2C_ERR_TIMEOUT; // wait for write interrupt flag (byte sent)
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return -1; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return -2;  // bus error
  if (TWI0.MSTATUS & TWI_RXACK_bm) return -3;   // received NACK from slave
#elif defined(__AVR_ATmega328P__)
  TWDR = data;                          // load byte into data register
  TWCR = (1<<TWINT) | (1 << TWEN);     // transmit byte
  if (i2c_wait_twint() != 0) return -5;
  if ((TWSR & 0xF8) != TW_MT_DATA_ACK)  return -1; // received NACK
#endif
  return 0;
}

int16_t i2c_read_ack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;                          // trigger read, send ACK after byte received
  if (i2c_wait_mstatus(TWI_RIF_bm) != 0) return I2C_ERR_TIMEOUT; // wait for read interrupt flag (byte ready)
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return -1; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return -2;  // bus error
  return TWI0.MDATA;                             // return received byte
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN); // trigger read, send ACK after byte received
  if (i2c_wait_twint() != 0) return -5;
  if ((TWSR & 0xF8) != TW_MR_DATA_ACK)  return -1; // not data+ACK
  return TWDR;                                   // return received byte
#endif
}

int16_t i2c_read_nack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_RECVTRANS_gc; // trigger read, send NACK after byte received

  uint32_t guard = 0;
  while (!(TWI0.MSTATUS & TWI_RIF_bm)) { // wait for read interrupt flag (byte ready)
    if (++guard > 100000UL) return I2C_ERR_TIMEOUT;
  }
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return I2C_ERR_ARBLOST; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return I2C_ERR_BUSERR;   // bus error
  return TWI0.MDATA;                 // return received byte (caller sends STOP via i2c_stop)
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEN); // trigger read, no TWEA = send NACK after byte (signals last byte)
  if (i2c_wait_twint() != 0) return I2C_ERR_TIMEOUT;
  if ((TWSR & 0xF8) != TW_MR_DATA_NACK)  return I2C_ERR_ARBLOST; // not NACK
  return TWDR;                        // return received byte
#endif
}

int8_t i2c_write_packet(uint8_t addr, uint8_t *data, uint16_t len) {
  int8_t start_resp = i2c_start(addr << 1, FALSE); // send START + address (write mode)
  if (start_resp != 0) return start_resp;
  for (uint16_t i = 0; i < len; i++) {
    int8_t write_resp = i2c_write(data[i]); // send each byte
    if (write_resp != 0) {
      i2c_stop(); // abort on error
      return write_resp;
    }
  }
  i2c_stop(); // send STOP after all bytes written
  return 0;
}

int32_t i2c_get_read_len(uint8_t addr) {
  int8_t start_resp = i2c_start((addr << 1) | (1), FALSE); // send START + address (read mode)
  if (start_resp) {
    i2c_stop();
    return start_resp;
  }
  int16_t len_hi = i2c_read_ack(); // read high byte of length, send ACK
  if (len_hi < 0) {
    i2c_stop();
    return len_hi;
  }

  int16_t len_lo = i2c_read_nack(); // read low byte of length, send NACK (last byte)
  if (len_lo < 0) {
    i2c_stop();
    return len_lo;
  }

  uint16_t len = ((uint16_t)len_hi << 8) | (uint16_t)len_lo; // reassemble 16-bit length
  i2c_stop();
  return len;
}

int8_t i2c_read_packet(uint8_t addr, uint8_t *buffer) {
  int8_t start_resp = i2c_start((addr << 1) | 1, FALSE); // send START + address (read mode)
  if (start_resp) {
    i2c_stop();
    return start_resp;
  }

  int16_t len_hi = i2c_read_ack(); // read high byte of payload length, send ACK
  if (len_hi < 0) {
    i2c_stop();
    return (int8_t)len_hi;
  }

  int16_t len_lo = i2c_read_ack(); // read low byte of payload length, send ACK
  if (len_lo < 0) {
    i2c_stop();
    return (int8_t)len_lo;
  }

  uint16_t len = ((uint16_t)len_hi << 8) | (uint16_t)len_lo; // reassemble 16-bit length
  if (len == 0 || len > I2C_BUFFER_SIZE) {
    i2c_stop();
    return -4; // invalid packet length
  }

  uint16_t i = 0;
  for (i = 0; i < len - 1; i++) {
    int16_t read_resp = i2c_read_ack(); // read each byte, send ACK to request next byte
    if (read_resp < 0) {
      i2c_stop();
      return read_resp;
    }
    buffer[i] = (uint8_t)read_resp;
  }
  int16_t last_read = i2c_read_nack(); // read last byte, send NACK to signal end of read
  if (last_read < 0) {
    i2c_stop();
    return last_read;
  }
  buffer[i] = (uint8_t)last_read;
  i2c_stop(); // send STOP
  return 0;
}

// used in interrupt
uint8_t i2c_slave_receive(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  i2c_slave_ack();    // send ACK to master
  return TWI0.SDATA;  // return received byte
#elif defined(__AVR_ATmega328P__)
  i2c_slave_ack(); // send ACK to master
  return TWDR;     // return received byte
#endif
}

// used in interrupt
void i2c_slave_transmit(uint8_t byte) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SDATA = byte; // load byte into slave data register
  i2c_slave_ack();   // send ACK and transmit byte
#elif defined(__AVR_ATmega328P__)
  TWDR = byte;     // load byte into data register
  i2c_slave_ack(); // send ACK and transmit byte
#endif
}

// used in interrupt
void i2c_slave_ack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc; // send ACK and continue transaction
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN); // clear interrupt flag, enable ACK, keep TWI enabled
#endif
}

// used in interrupt
void i2c_slave_nack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SCTRLB = TWI_ACKACT_NACK_gc | TWI_SCMD_RESPONSE_gc; // send NACK and continue (reject byte)
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEN); // clear interrupt flag, no TWEA = send NACK
#endif
}
