/*
 * Original source of this code: https://arduino.stackexchange.com/questions/45519/is-it-possible-to-obtain-0-05-duty-cycle-at-1hz-in-arduinoa-500us-square-puls @Edgar Bonet
 * Configure Timer 1 to generate a slow, low duty cycle PWM signal.
 * Output pin = digital 9 = PB1 = OC1A.
 */

//For testing use period=1e-3, width=0.5e-3
//For deployment use period=1, width=100e-6
float PWM_PERIOD  = 1e-3;     // 1 s
float PULSE_WIDTH = 0.5e-3;//100e-6;  // 100 us
int analogPin = 10;
char a;

/*
 * With a 16 MHz clock and this prescaler, the resolution is 16 us and
 * the maximum period is 1.048576 s.
 * MAX COUNT: 65536 = 2^n_BITSOFTIMER = 2^16
 * TIMER1 is 16bit timer, the other 2 (timer 0 and 2) are 8bit
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
    // |= -> (compound bitwise or)
    // _BV (bit value) is a macro for
    // DDRB - The Port B Data Direction Register - read/write, basically 1 is output, 0 is input
    // PORT B is associated with digital pin 8 to 13
    // PB1 would be pin 1 of port B (0->8, 1->9) i think
    DDRB  |= _BV(PB1);    // set PB1 = OC1A as output
    ICR1   = round(PWM_PERIOD * F_TIMER) -1 ;
    OCR1A  = round(PULSE_WIDTH * F_TIMER) -1 ;
    // | -> (bitwise or) 
    TCCR1A = _BV(COM1A1)  // non-inverting PWM on OC1A
           | _BV(WGM11);  // fast PWM mode, TOP = ICR1
    TCCR1B = _BV(WGM12)   // ditto
           | _BV(WGM13)   // ditto
           | _BV(CS12);   // clock at F_CPU/256 = 62.5 kHz

    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() 

{

  while(Serial.available()) 
    {
    a = Serial.read();// read the incoming data as string  
    }

  if (a == '1'){changePeriod(1e-3);a='0';}
  if (a == '2'){changePeriod(2e-3);a='0';}
  
  analogWrite(analogPin, 255);
}


void changePeriod(float newPeriod)
{
  ICR1   = round(newPeriod * F_TIMER) -1 ;
  Serial.print("Period at: ");
  Serial.println(newPeriod);
}
