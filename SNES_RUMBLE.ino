#include <Arduino.h>

// Pin aliases (Arduino uses the physical number of the digital pin)
#define LATCH            2   // PD2 → INT0
#define CLOCK            3   // PD3 → INT1
#define DATA             4   // PD4
#define VIBRATION_MOTOR  5   // PD5 → OC0B (PWM)

const uint8_t RUMBLE_SENTRY = 0b01110010;
volatile uint16_t dataPacket = 0;
volatile uint8_t clockCycle = 0;

void ISR_LATCH() {
  // Negative edge trigger in PD2
  dataPacket = 0;
  clockCycle = 0;
}

void ISR_CLOCK() {
  // Negative edge trigger in PD3
  dataPacket <<= 1;
  dataPacket |= digitalRead(DATA);

  if (clockCycle == 31) {
    if ((dataPacket >> 8) == RUMBLE_SENTRY) {
      uint8_t pwmValue = ((dataPacket & 0xF0) | ((dataPacket & 0x0F) << 4));
      analogWrite(VIBRATION_MOTOR, pwmValue);
    }
  }

  clockCycle++;
}

void setup() {
  // Inputs
  pinMode(LATCH, INPUT);
  pinMode(CLOCK, INPUT);
  pinMode(DATA, INPUT);

  // Outputs
  pinMode(VIBRATION_MOTOR, OUTPUT);

  // External interrupt setup
  attachInterrupt(digitalPinToInterrupt(LATCH), ISR_LATCH, FALLING);
  attachInterrupt(digitalPinToInterrupt(CLOCK), ISR_CLOCK, FALLING);

  // PWM initialization on PD5
  analogWrite(VIBRATION_MOTOR, 0);
}

void loop() {
  // Processing occurs in the ISRs
}
