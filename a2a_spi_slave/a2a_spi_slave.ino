#include "pins_arduino.h"

char buf [100];
volatile byte pos;
volatile boolean process_it;

int pinA = 6;
int pinB = 7;


void setup (void) {
  // Serial for debugging
  Serial.begin (9600);

  // Act as slave, MISO should be OUTPUT
  pinMode(MISO, OUTPUT);

  // Turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // Turn on interrupts
  SPCR |= _BV(SPIE);

  pos = 0;
  process_it = false;

  // LED Output
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
}


// SPI ISR
ISR (SPI_STC_vect) {
  byte c = SPDR;

  // add to buffer if room
  if (pos < sizeof buf) {
    buf [pos++] = c;

    // newline means time to process buffer
    if (c == '\n') {
      process_it = true;
    }
  }
}

void loop (void) {
  // wait for flag set in ISR
  if (process_it) {
    buf [pos] = 0;
    Serial.println(buf);
    changeLed(buf);
    pos = 0;
    process_it = false;
  }
}

// update LED output
void changeLed(char * buf) {
  int pin, value;

  if (buf[0] == 'A') {
    pin = pinA;
  } else if (buf[0] == 'B') {
    pin = pinB;
  } else {
    return;
  }

  if (buf[1] == '1') {
    value = HIGH;
  } else if (buf[1] == '0') {
    value = LOW;
  } else {
    return;
  }

  digitalWrite(pin, value);
}
