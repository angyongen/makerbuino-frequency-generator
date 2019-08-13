#define _set(register, bits) (register |= (bits))  
#define clr(register, bits) (register &= ~(bits))
#define bitAt(position, input) ((input & (1UL<<position))>>position)
#define modify(register, set_bits, clr_bits) (register = ((register & ~(clr_bits)) | (set_bits)))
#define setto(register, target, bits) (register = target ? register | (bits) : register & ~(bits))


//todo:change back to normal names (TCCR2A and etc)

//http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf

//https://sites.google.com/site/qeewiki/books/avr-guide/pwm-on-the-atmega328
//http://www.avrbeginners.net/architecture/timers/timers.html
//https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
//http://www.avrfreaks.net/forum/difference-bewteen-phase-correct-and-fast-pwm


#define TIMER2_A_CTRL TCCR2A //TCCR2A   COM2A1 COM2A0 COM2B1 COM2B0 -     -     WGM21 WGM20 
#define TIMER2_B_CTRL TCCR2B //TCCR2B   FOC2A  FOC2B  -      -      WGM22 CS22  CS21  CS20 

#define PORTB_DIRECTION DDRB
#define PORTD_DIRECTION DDRD

#define TIMER2_MODE_0 _BV(WGM20)
#define TIMER2_MODE_1 _BV(WGM21)
#define TIMER2_MODE_2 _BV(WGM22)
//MODE_2:0 Operation          TOP    Update OCR0x   TOV Flag Set on
//0  0 0 0 Normal             0xFF   Immediate      MAX 
//1  0 0 1 PWM, Phase Correct 0xFF   TOP            BOTTOM 
//2  0 1 0 CTC                OCRA   Immediate      MAX 
//3  0 1 1 Fast PWM           0xFF   BOTTOM         MAX 
//4  1 0 0 Reserved           -      - 
//5  1 0 1 PWM, Phase Correct OCRA   TOP            BOTTOM 
//6  1 1 0 Reserved           -      - 
//7  1 1 1 Fast PWM           OCRA   BOTTOM         TOP


//#define TIMER2_A_COMPARE OCR2A
//#define TIMER2_B_COMPARE OCR2B


#define TIMER2_CLOCK_0 _BV(CS20)
#define TIMER2_CLOCK_1 _BV(CS21)
#define TIMER2_CLOCK_2 _BV(CS22)
//22 21 20 
//0  0  0    Timer/Counter2 Disabled 
//0  0  1    No Prescaling
//0  1  0    Clock / 8
//0  1  1    Clock / 32
//1  0  0    Clock / 64
//1  0  1    Clock / 128
//1  1  0    Clock / 256
//1  1  1    Clock / 1024 

//Non PWM modes:
//Table 17-2. Compare Output Mode, non-PWM Mode
//COM2A1 COM2A0 Description
//0 0 Normal port operation, OC0A disconnected.
//0 1 Toggle OC2A on compare match
//1 0 Clear OC2A on compare match
//1 1 Set OC2A on compare match

#define TIMER2_A_TYPE_0 _BV(COM2A0)
#define TIMER2_A_TYPE_1 _BV(COM2A1)
//A1 A0  
//0  0   OC2A disabled
//0  1   WGM22 = 0: Normal Port Operation, OC2A Disconnected
//       WGM22 = 1: Toggle OC2A on Compare Match
//1  0   Non-inverted mode (HIGH at bottom, LOW on Match)
//1  1   Inverted mode (LOW at bottom, HIGH on Match)
//Applies only to PWM modes

#define TIMER2_B_TYPE_0 _BV(COM2B0)
#define TIMER2_B_TYPE_1 _BV(COM2B1)
//B1 B0
//0  0    OC2B disabled
//0  1    Reserved
//1  0    Non-inverted mode (HIGH at bottom, LOW on Match)
//1  1    Inverted mode (LOW at bottom, HIGH on Match)
//Applies only to PWM modes


__attribute__((always_inline)) inline void prep_timer2a()
{
  enable_timer2_a_output();
  timer2_a_noninverting();
  timer2_set_mode(3);      // fastpwm
  timer2_set_prescaler(1);
}
__attribute__((always_inline)) inline void prep_timer2b()
{
  enable_timer2_b_output();
  timer2_b_noninverting();
  timer2_set_mode(3);      // fastpwm
  timer2_set_prescaler(1);
}

__attribute__((always_inline)) inline void enable_timer2_a_output()
{
  _set(PORTB_DIRECTION, _BV(DDB3));
}
__attribute__((always_inline)) inline void enable_timer2_b_output()
{
  _set(PORTD_DIRECTION, _BV(DDD3));
}
__attribute__((always_inline)) inline void timer2_a_noninverting()
{
  clr(TIMER2_A_CTRL, TIMER2_B_TYPE_0 | TIMER2_B_TYPE_1);
  clr(TIMER2_A_CTRL, TIMER2_A_TYPE_0);
  _set(TIMER2_A_CTRL, TIMER2_A_TYPE_1);
}

__attribute__((always_inline)) inline void timer2_b_noninverting()
{
  clr(TIMER2_A_CTRL, TIMER2_A_TYPE_0 | TIMER2_A_TYPE_1);
  clr(TIMER2_A_CTRL, TIMER2_B_TYPE_0);
  _set(TIMER2_A_CTRL, TIMER2_B_TYPE_1);
}

__attribute__((always_inline)) inline void timer2_a_type(uint8_t type)
{
  setto(TIMER2_A_CTRL, bitAt(0, type), TIMER2_A_TYPE_0);
  setto(TIMER2_A_CTRL, bitAt(1, type), TIMER2_A_TYPE_1);
}

__attribute__((always_inline)) inline void timer2_b_type(uint8_t type)
{
  setto(TIMER2_A_CTRL, bitAt(0, type), TIMER2_B_TYPE_0);
  setto(TIMER2_A_CTRL, bitAt(1, type), TIMER2_B_TYPE_1);
}

__attribute__((always_inline)) inline void timer2_set_mode(uint8_t mode)
{
  setto(TIMER2_A_CTRL, bitAt(0, mode), TIMER2_MODE_0);
  setto(TIMER2_A_CTRL, bitAt(1, mode), TIMER2_MODE_1);
  setto(TIMER2_B_CTRL, bitAt(2, mode), TIMER2_MODE_2);
}

__attribute__((always_inline)) inline void timer2_set_prescaler(uint8_t prescaler)
{
  setto(TIMER2_B_CTRL, bitAt(0, prescaler), TIMER2_CLOCK_0);
  setto(TIMER2_B_CTRL, bitAt(1, prescaler), TIMER2_CLOCK_1);
  setto(TIMER2_B_CTRL, bitAt(2, prescaler), TIMER2_CLOCK_2);
}
