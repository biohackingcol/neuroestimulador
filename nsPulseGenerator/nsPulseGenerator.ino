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
const uint16_t TIMER_PRESCALER = 256;//notice that to change this you also need to change CSbits of TCCR1B
const float F_TIMER = F_CPU / TIMER_PRESCALER;

void setup()
{
    // Configure Timer 1.
    // |= -> (compound bitwise or)
    // _BV (bit value) is a macro for making high whatever bit is inside the ( ), in the context of the bits of the register
    //thus making bitwise or between _BV will set high each particular bit. Other bitwise operations arise more complicated setups.    
    // DDRB - The Port B Data Direction Register - read/write, basically 1 is output, 0 is input, is 8bit sized
    // PORT B is associated with digital pin 8 to 13
    // PB1 would be pin 1 of port B (0->8, 1->9) i think
    //note that Pins 9 and 10 are controlled by timer1 only in the UNO
    //Servo Library uses Timer1. You can’t use PWM on Pin 9, 10 when you use the Servo Library on an Arduino.
    // we can use tone function for doc feedback because it relies on timer2
    DDRB  |= _BV(PB1);    // set PB1 = OC1A as output

    //in the following n would be the number of the timer in general, x the channel
    //ICRn - Input Capture Register
    ICR1   = round(PWM_PERIOD * F_TIMER) -1 ;//when count is equal to ICR1 the counter is reset (going back to BOTTOM and SET (ON)) because ICR1 in the mode chosen is set to TOP.
    //OCRn - Output Compare Register 
    OCR1A  = round(PULSE_WIDTH * F_TIMER) -1 ;//when count is equal to OCR1A the ouput is set to clear (OFF) because of non-inverting compare output mode
    
    // | -> (bitwise or) 
    //TCCRnx - Timer/Counter Control Register, these are both 8bit register that control timer behaviour
    TCCR1A = _BV(COM1A1)  // non-inverting PWM on OC1A Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
                          // In non-inverting Compare Output mode, the Output Compare (OC1x) is cleared on the compare match between TCNT1 and OCR1x, and set at BOTTOM.
                          // That means that when TCNT1 is equal to OCR1A we get OFF, but when we get to the TOP and then resetted to the BOTTOM, at the BOTTOM we get ON. 
           | _BV(WGM11);  // fast PWM mode, TOP = ICR1
                          //The counter counts from BOTTOM to TOP then restarts from BOTTOM
    TCCR1B = _BV(WGM12)   // fast PWM mode, TOP = ICR1
           | _BV(WGM13)   // fast PWM mode, TOP = ICR1
           | _BV(CS12);   // clock at F_CPU/256 = 62.5 kHz, these chooses the prescaler

    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

    /* For some reason:
    Using the ICR1 Register for defining TOP works well when using fixed TOP values. By using
    ICR1, the OCR1A Register is free to be used for generating a PWM output on OC1A. However,
    if the base PWM frequency is actively changed (by changing the TOP value), using the OCR1A
    as TOP is clearly a better choice due to its double buffer feature.

    NOT SURE IS THIS IS USEFUL IN ANYWAY...
    */
}

void loop() 

{

  //we need to find a way to set the output on and off
  //VDAC could be done with a POT?

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
