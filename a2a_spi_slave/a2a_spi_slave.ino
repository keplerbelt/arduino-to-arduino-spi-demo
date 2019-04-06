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

  // CLK = 0.5 MHz
  SPCR |= _BV(SPR1);
  SPSR |= _BV(SPI2X);

  pos = 0;
  process_it = false;

  // LED Output
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
}


// SPI ISR
ISR (SPI_STC_vect) {
  byte c = SPDR;

  if (c > 47 && c < 91) {
    // add to buffer if room
    if (pos < sizeof buf) {
      buf[pos++] = c;

      // Z (ASCII 90) means process the buffer
      if (c == 90) {
        process_it = true;
      }
    }
  } else {
    pos = 0;
  }
}

void loop (void) {
  // wait for flag set in ISR
  if (process_it) {
    changeLed(buf[0], buf[1]);
    process_it = false;
  }
}

// update LED output
void changeLed(char pinSel, char valSel) {
  int pin, value;

  if (pinSel == 'A') {
    pin = pinA;
  } else if (pinSel == 'B') {
    pin = pinB;
  } else {
    return;
  }

  if (valSel == '1') {
    value = HIGH;
  } else if (valSel == '0') {
    value = LOW;
  } else {
    return;
  }

  digitalWrite(pin, value);
}
