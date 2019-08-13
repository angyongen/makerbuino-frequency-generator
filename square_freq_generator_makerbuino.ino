
#include "gamebuino_main_alt.h"
Gamebuino gb;

uint8_t repeatTime = 3;

uint8_t prescaler = 1;
uint8_t cycles = 128;
uint8_t cursorA;
bool cursorBSelected = false;
uint8_t cursorB;
void setup() {
  gb.begin();
  timer2_set_mode(2); //CTC Mode
  timer2_b_type(1); //Toggle OC2B on compare match [OCR2B = pin 3 (PD3)]
  timer2_set_prescaler(prescaler);
  OCR2A = cycles;
  enable_timer2_b_output();
}
void displaystats() {
  uint8_t prescaler_multiplier = 1 << prescaler;
  float freq = ((float)F_CPU) / (float)(cycles * (uint32_t)prescaler_multiplier * 2);
  if (cursorA == 0) {
    gb.display.print(F("\20"));
    gb.display.print(F("prescaler:  "));
    if (cursorBSelected) {
      gb.display.print(F("\20"));
    } else {
      gb.display.print(F(" "));
    }
  } else {
    gb.display.print(F(" "));
    gb.display.print(F("prescaler:   "));
  }
  gb.display.print(prescaler);
  gb.display.print(F("(x"));
  gb.display.print(prescaler_multiplier);
  gb.display.println(F(")"));

  if (cursorA == 1) {
    gb.display.print(F("\20"));
    gb.display.print(F("timer value:"));
    if (cursorBSelected) {
      gb.display.print(F("\20"));
    } else {
      gb.display.print(F(" "));
    }
  } else {
    gb.display.print(F(" "));
    gb.display.print(F("timer value: "));
  }
  gb.display.println(cycles);


  gb.display.println(F("half-period cycles: "));
  gb.display.println(cycles * prescaler_multiplier);

  gb.display.println(F("frequency: "));
  gb.display.print(freq);
  gb.display.println(F("Hz"));

  freq /= 1000;
  gb.display.print(F("("));
  gb.display.print(freq);
  gb.display.print(F("kHz)"));
}
void loop() {
  if (gb.update()) {
    displaystats();

    if (gb.buttons.pressed(BTN_A)) {
      cursorBSelected = true;
      switch (cursorA) {
        case 0:
          selectPrescaler();
          break;
        case 1:
          selectCycles();
          break;
      }
      cursorBSelected = false;
    }

    if (gb.buttons.pressed(BTN_UP)) --cursorA;
    if (gb.buttons.pressed(BTN_DOWN)) ++cursorA;
    if (cursorA == 2) cursorA = 1;
    if (cursorA > 2) cursorA = 0;

  }
}

void selectPrescaler()
{
  while (true) {
    if (gb.update()) {
      displaystats();
      if (gb.buttons.repeat(BTN_UP, repeatTime)) ++prescaler;
      if (gb.buttons.repeat(BTN_DOWN, repeatTime)) --prescaler;
      prescaler &= 7;
      timer2_set_prescaler(prescaler);
      if (gb.buttons.pressed(BTN_A)) return;
      if (gb.buttons.pressed(BTN_B)) return;
    }
  }
}

void selectCycles()
{
  while (true) {
    if (gb.update()) {
      displaystats();
      if (gb.buttons.repeat(BTN_UP, repeatTime)) ++cycles;
      if (gb.buttons.repeat(BTN_RIGHT, repeatTime)) cycles+=10;
      if (gb.buttons.repeat(BTN_DOWN, repeatTime)) --cycles;
      if (gb.buttons.repeat(BTN_LEFT, repeatTime)) cycles-=10;
      OCR2A = cycles;
      if (gb.buttons.pressed(BTN_A)) return;
      if (gb.buttons.pressed(BTN_B)) return;
    }
  }
}
