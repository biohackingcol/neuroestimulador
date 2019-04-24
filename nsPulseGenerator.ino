/*
 * Original source of this code: https://arduino.stackexchange.com/questions/45519/is-it-possible-to-obtain-0-05-duty-cycle-at-1hz-in-arduinoa-500us-square-puls @Edgar Bonet
 * Configure Timer 1 to generate a slow, low duty cycle PWM signal.
 * Output pin = digital 9 = PB1 = OC1A.
 */

const float PWM_PERIOD  = 1;     // 1 s
const float PULSE_WIDTH = 100e-6;  // 100 us

/*
 * With a 16 MHz clock and this prescaler, the resolution is 16 us and
 * the maximum period is 1.048576 s.
 * MAX COUNT: 65536 = 2^n_BITSOFTIMER = 2^16
 * RESOLUTION = T_CPU * PRESCALER = PRESCALER / F_CPU
 * MAX_PERIOD= MAX_COUNT * RESOLUTION
 * 
 * with prescaler = 256-> resolution = 16us, max period = 1s
 */
const uint16_t TIMER_PRESCALER = 256;
const float F_TIMER = F_CPU / TIMER_PRESCALER;

void setup()
{
    // Configure Timer 1.
    DDRB  |= _BV(PB1);    // set PB1 = OC1A as output
    ICR1   = round(PWM_PERIOD * F_TIMER) -1 ;
    OCR1A  = round(PULSE_WIDTH * F_TIMER) -1 ;
    TCCR1A = _BV(COM1A1)  // non-inverting PWM on OC1A
           | _BV(WGM11);  // fast PWM mode, TOP = ICR1
    TCCR1B = _BV(WGM12)   // ditto
           | _BV(WGM13)   // ditto
           | _BV(CS12);   // clock at F_CPU/256 = 62.5 kHz
}

void loop() 

{
}
