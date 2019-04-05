#include <SPI.h>
#include "pins_arduino.h"

struct Button {
  int pin = 0;
  int currentState = HIGH;
  unsigned long stateSince = 0;
  int debounce = 100;
  int toggleValue = 0;
  char label = '-';
};

struct Button buttonA;
struct Button buttonB;

SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

void setup (void) {
  // Serial for Debugging
  Serial.begin (9600);

  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin ();

  // Setup button A
  buttonA.pin = 6;
  buttonA.label = 'A';
  pinMode(buttonA.pin, INPUT_PULLUP);

  // Setup button B
  buttonB.pin = 7;
  buttonB.label = 'B';
  pinMode(buttonB.pin, INPUT_PULLUP);
}

void loop (void) {
  bool buttonA_changed = buttonToggle(&buttonA);
  bool buttonB_changed = buttonToggle(&buttonB);

  if (buttonA_changed) {
    xferData(buttonA);
  }

  if (buttonB_changed) {
    xferData(buttonB);
  }
}

void xferData(struct Button button) {
  char c;

  SPI.beginTransaction(spiSettings);

  // Enable Slave Select
  // SS is pin 10
  digitalWrite(SS, LOW);

  // Create payload
  char data[3] = {button.label, ('0' + button.toggleValue), '\n'};
  Serial.println(data);

  // Send data
  for (const char * p = data; c = *p; p++) {
    SPI.transfer(c);
  }

  // Disable Slave Select
  digitalWrite(SS, HIGH);
  SPI.endTransaction();
}


bool buttonToggle(struct Button* button) {
  bool toggleState = false;
  int reading = digitalRead(button->pin);

  if (reading == button->currentState) {
  } else {
    unsigned long int currentTime = millis();

    if (((currentTime - button->stateSince) > button->debounce)
        && button->currentState == LOW) {
      toggleState = true;
      if (button->toggleValue == 0) {
        button->toggleValue = 1;
      } else {
        button->toggleValue = 0;
      }
    }
    button->currentState = reading;
    button->stateSince = currentTime;
  }
  return toggleState;
}
