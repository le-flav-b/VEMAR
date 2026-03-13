#include "i2c.h"

void i2c_init() {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLA |= TWI_ENABLE_bm;
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
  TWI0.MBAUD = TWI_BAUD;
#elif defined(__AVR_ATmega328P__)
  TWBR = TWBR_VAL;
  TWSR = 0; // prescaler = 1
  TWCR = (1 << TWEN);
#endif
}

void i2c_init_slave(uint8_t addr) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
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

int8_t i2c_start(uint8_t addr_rw, bool restart) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  (void)restart;
  while ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc);
  TWI0.MADDR = addr_rw;
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return -1; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return -2; // bus error
#elif defined(__AVR_ATmega328P__)
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
  while (!(TWCR & (1<<TWINT)));
  if (restart) {
    if ((TWSR & 0xF8) != 0x10) return -1; // RESTART not acknowledged
  }
  else {
    if ((TWSR & 0xF8) != 0x08) return -1; // START not acknowledged
  }
  TWDR = addr_rw;
  TWCR = (1<<TWINT) | (1<<TWEN);
  while (!(TWCR & (1 << TWINT)));
  if ((TWSR & 0xF8)!= MT_DATA_ACK) return -2; // addr_rw not acknowledged
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
  while (!(TWI0.MSTATUS & TWI_WIF_bm));
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return -1; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return -2; // bus error
  if (TWI0.MSTATUS & TWI_RXACK_bm) return -3; // NACK
#elif defined(__AVR_ATmega328P__)
  TWDR = data;
  TWCR = (1<<TWINT) | (1 << TWEN);
  while (!(TWCR & (1<<TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK)  return -1; // NACK
#endif
  return 0;
}

int16_t i2c_read_ack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc; //trigger read, send ACK
  while (!(TWI0.MSTATUS & TWI_RIF_bm)); // wait for Read Interrupt Flag
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return -1; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return -2; // bus error
  return TWI0.MDATA;
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  if ((TWSR & 0xF8) != MT_DATA_ACK)  return -1; // NACK
  return TWDR;
#endif
}

int16_t i2c_read_nack(void) {
#if defined(__AVR_ATtiny412__) || defined(__AVR_ATtiny1614__)
  TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;
  while (!(TWI0.MSTATUS & TWI_RIF_bm)); 
  if (TWI0.MSTATUS & TWI_ARBLOST_bm) return -1; // arbitration lost
  if (TWI0.MSTATUS & TWI_BUSERR_bm) return -2; // bus error
  return TWI0.MDATA;
#elif defined(__AVR_ATmega328P__)
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  if ((TWSR & 0xF8) != MR_DATA_NACK)  return -1; // not NACK
  return TWDR;
#endif
}

int8_t i2c_write_packet(uint8_t addr, uint8_t *data, size_t len) {
  int8_t start_resp = i2c_start(addr << 1, false);
  if (start_resp != 0) return start_resp;
  for (int8_t i = 0; i < len; i++) {
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
  int8_t start_resp = i2c_start((addr << 1) | (1), false);
  if (start_resp) {
    i2c_stop();
    return start_resp;
  }
  uint16_t len = 0;
  for (uint8_t i = 0; i < 2; i++) {
    int16_t read_resp = i2c_read_ack();
    if (read_resp < 0) {
      i2c_stop();
      return read_resp;
    }
    len |= (read_resp << (8 * (1 - i)));
  }
  return len;
}

int8_t i2c_read_packet(uint8_t addr, uint8_t *buffer) {
  int32_t len = i2c_get_read_len(addr);
  if (len <= 0) {
    return (int8_t)len;
  }
  uint16_t i;
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