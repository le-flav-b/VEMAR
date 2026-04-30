#include "i2c.h"

#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
static int8_t i2c_wait_bus_idle(void) {
  uint32_t guard = 0;
  while ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc) {
    if (++guard > 100000UL) {
      return I2C_ERR_TIMEOUT;
    }
  }
  return 0;
}

static int8_t i2c_wait_mstatus(uint8_t flag) {
  uint32_t guard = 0;
  while (!(TWI0.MSTATUS & flag)) {
    if (++guard > 100000UL) {
      return I2C_ERR_TIMEOUT;
    }
  }
  return 0;
}
#endif


#if defined(__AVR_ATmega328P__)
static int8_t i2c_wait_twint(void) {
  uint32_t guard = 0;
  while (!(TWCR & (1 << TWINT))) {
    if (++guard > 100000UL) {
      return -5; // timeout waiting for hardware flag
    }
  }
  return 0;
}
#endif

void i2c_init() {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  // Configure pins as inputs for open-drain operation
  // PB0 (SCL) and PB1 (SDA) — TWI0 default pins
  PORTB.DIRCLR = PIN0_bm | PIN1_bm;  // Set PB0 and PB1 as inputs

  // Configure TWI in master mode
  TWI0.MBAUD = TWI_BAUD;
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
  TWI0.MCTRLA |= TWI_ENABLE_bm;
#elif defined(__AVR_ATmega328P__)
  // Configure I2C pins as inputs for open-drain operation
  // A4 (SDA) = PC4, A5 (SCL) = PC5
  DDRC &= ~(1 << DDC4 | 1 << DDC5);  // Set PC4 and PC5 as inputs
  PORTC &= ~(1 << PORTC4 | 1 << PORTC5);  // Disable pull-ups (I2C uses external pull-ups)
  
  TWBR = TWBR_VAL;
  TWSR = 0; // prescaler = 1
  TWCR = (1 << TWEN);
#endif
}

void i2c_init_slave(uint8_t addr) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  // Configure pins as inputs for open-drain operation
  // PB0 (SCL) and PB1 (SDA) — TWI0 default pins
  PORTB.DIRCLR = PIN0_bm | PIN1_bm;  // Set PB0 and PB1 as inputs

  // Configure TWI in slave mode
  TWI0.SADDR = addr << 1;
  TWI0.SCTRLA |= TWI_ENABLE_bm | TWI_APIEN_bm | TWI_DIEN_bm;
#elif defined(__AVR_ATmega328P__)
  TWAR = (addr << 1) | (0 << TWGCE);
  TWCR = (1 << TWEA) | (1 << TWEN);
#endif
}

void i2c_stop_interface(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  #ifdef I2C_MASTER
    TWI0.MCTRLA &= ~TWI_ENABLE_bm;
  #endif
  #ifdef I2C_SLAVE
    TWI0.SCTRLA &= ~TWI_ENABLE_bm;
  #endif
#elif defined(__AVR_ATmega328P__)
  TWCR &= ~(1 << TWEN);
#endif
}

// used when switching from slave to master
void i2c_switch_to_master(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SCTRLA &= ~TWI_ENABLE_bm;
  TWI0.MCTRLA |= TWI_ENABLE_bm;
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWEN);
#endif
}

// used when switching from master to slave
void i2c_switch_to_slave(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLA &= ~TWI_ENABLE_bm;
  TWI0.SCTRLA |= TWI_ENABLE_bm | TWI_APIEN_bm | TWI_DIEN_bm;
#elif defined(__AVR_ATmega328P__)
  TWAR = (SLAVE_ADDR << 1) | (0 << TWGCE);
  TWCR = (1 << TWEA) | (1 << TWEN);
#endif
}

int8_t i2c_start(uint8_t addr_rw, bool_t restart) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  (void)restart;
  if (i2c_wait_bus_idle() != 0) return I2C_ERR_TIMEOUT;
  TWI0.MADDR = addr_rw;
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return I2C_ERR_ARBLOST; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return I2C_ERR_BUSERR; // bus error
#elif defined(__AVR_ATmega328P__)
  (void)restart;
  uint8_t expected_addr_ack = (addr_rw & 0x01) ? TW_MR_SLA_ACK : TW_MT_SLA_ACK;

  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  if (i2c_wait_twint() != 0) return I2C_ERR_TIMEOUT;
  // Accept both 0x08 (START) and 0x10 (REPEATED START) — a STOP+START issued
  // back-to-back can appear as a repeated start before TWSTO clears on the bus.
  {
    uint8_t sr = TWSR & 0xF8;
    if (sr != 0x08 && sr != 0x10) return I2C_ERR_ARBLOST;
  }
  TWDR = addr_rw;
  TWCR = (1<<TWINT) | (1<<TWEN);
  if (i2c_wait_twint() != 0) return I2C_ERR_TIMEOUT;
  if ((TWSR & 0xF8) != expected_addr_ack) return I2C_ERR_BUSERR; // addr_rw not acknowledged
#endif
  return 0;
}

void i2c_stop(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
#elif defined(__AVR_ATmega328P__)
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
#endif
}

int8_t i2c_write(uint8_t data) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MDATA = data;
  if (i2c_wait_mstatus(TWI_WIF_bm) != 0) return I2C_ERR_TIMEOUT;
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return -1; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return -2; // bus error
  if (TWI0.MSTATUS & TWI_RXACK_bm) return -3; // NACK
#elif defined(__AVR_ATmega328P__)
  TWDR = data;
  TWCR = (1<<TWINT) | (1 << TWEN);
  if (i2c_wait_twint() != 0) return -5;
  if ((TWSR & 0xF8) != MT_DATA_ACK)  return -1; // NACK
#endif
  return 0;
}

int16_t i2c_read_ack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc; //trigger read, send ACK
  if (i2c_wait_mstatus(TWI_RIF_bm) != 0) return I2C_ERR_TIMEOUT;
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return -1; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return -2; // bus error
  return TWI0.MDATA;
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
  if (i2c_wait_twint() != 0) return -5;
  if ((TWSR & 0xF8) != MR_DATA_ACK)  return -1; // not data+ACK
  return TWDR;
#endif
}

int16_t i2c_read_nack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_RECVTRANS_gc;

  uint32_t guard = 0;
  while (!(TWI0.MSTATUS & TWI_RIF_bm)) {
    if (++guard > 100000UL) return I2C_ERR_TIMEOUT;
  }
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return I2C_ERR_ARBLOST; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return I2C_ERR_BUSERR; // bus error
  TWI0.MCTRLB  = TWI_MCMD_STOP_gc;
  return TWI0.MDATA;
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEN);
  if (i2c_wait_twint() != 0) return I2C_ERR_TIMEOUT;
  if ((TWSR & 0xF8) != MR_DATA_NACK)  return I2C_ERR_ARBLOST; // not NACK
  return TWDR;
#endif
}

int8_t i2c_write_packet(uint8_t addr, uint8_t *data, uint16_t len) {
  int8_t start_resp = i2c_start(addr << 1, FALSE);
  if (start_resp != 0) return start_resp;
  for (uint16_t i = 0; i < len; i++) {
    int8_t write_resp = i2c_write(data[i]);
    if (write_resp != 0) {
      i2c_stop();
      return write_resp;
    }
  }
  i2c_stop();
  return 0;
}

int32_t i2c_get_read_len(uint8_t addr) {
  int8_t start_resp = i2c_start((addr << 1) | (1), FALSE);
  if (start_resp) {
    i2c_stop();
    return start_resp;
  }
  int16_t len_hi = i2c_read_ack();
  if (len_hi < 0) {
    i2c_stop();
    return len_hi;
  }

  int16_t len_lo = i2c_read_nack();
  if (len_lo < 0) {
    i2c_stop();
    return len_lo;
  }

  uint16_t len = ((uint16_t)len_hi << 8) | (uint16_t)len_lo;
  i2c_stop();
  return len;
}

int8_t i2c_read_packet(uint8_t addr, uint8_t *buffer) {
  int8_t start_resp = i2c_start((addr << 1) | 1, FALSE);
  if (start_resp) {
    i2c_stop();
    return start_resp;
  }

  int16_t len_hi = i2c_read_ack();
  if (len_hi < 0) {
    i2c_stop();
    return (int8_t)len_hi;
  }

  int16_t len_lo = i2c_read_ack();
  if (len_lo < 0) {
    i2c_stop();
    return (int8_t)len_lo;
  }

  uint16_t len = ((uint16_t)len_hi << 8) | (uint16_t)len_lo;
  if (len == 0 || len > I2C_BUFFER_SIZE) {
    i2c_stop();
    return -4; // invalid packet length
  }

  uint16_t i = 0;
  for (i = 0; i < len - 1; i++) {
    int16_t read_resp = i2c_read_ack();
    if (read_resp < 0) {
      i2c_stop();
      return read_resp;
    }
    buffer[i] = (uint8_t)read_resp;
  }
  int16_t last_read = i2c_read_nack();
  if (last_read < 0) {
    i2c_stop();
    return last_read;
  }
  buffer[i] = (uint8_t)last_read;
  i2c_stop();
  return 0;
}

// used in interrupt
uint8_t i2c_slave_receive(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  i2c_slave_ack();
  return TWI0.SDATA;
#elif defined(__AVR_ATmega328P__)
  i2c_slave_ack();
  return TWDR;
#endif
}

// used in interrupt
void i2c_slave_transmit(uint8_t byte) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SDATA = byte;
  i2c_slave_ack();
#elif defined(__AVR_ATmega328P__)
  TWDR = byte;
  i2c_slave_ack();
#endif
}

// used in interrupt
void i2c_slave_ack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc;
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
#endif
}

// used in interrupt
void i2c_slave_nack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.SCTRLB = TWI_ACKACT_NACK_gc | TWI_SCMD_RESPONSE_gc;
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEN);
#endif
}