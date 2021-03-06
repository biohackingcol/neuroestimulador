/*
 * Original source of this code: https://arduino.stackexchange.com/questions/45519/is-it-possible-to-obtain-0-05-duty-cycle-at-1hz-in-arduinoa-500us-square-puls @Edgar Bonet
 * Configure Timer 1 to generate a slow, low duty cycle PWM signal.
 * Output pin = digital 9 = PB1 = OC1A.
 * Timer Reference:
 * atmega328p datasheet https://www.sparkfun.com/datasheets/Components/SMD/ATMega328.pdf
 * http://www.avrbeginners.net/architecture/timers/timers.html
 * https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072
 * https://playground.arduino.cc/Code/BitMath/
 * https://www.arduino.cc/en/Reference/PortManipulation
 * https://www.microchip.com/webdoc/AVRLibcReferenceManual/FAQ_1faq_use_bv.html
 */

//For testing use period=1e-3, width=0.5e-3 or 100e-6
//For deployment use period=1, width=100e-6
float PWM_PERIOD  = 1e-3;     // 1 s
float p1 = 1;
float p2 = 0.5;
float pTest = 1e-3;
float PULSE_WIDTH = 100e-6;//100e-6;  // 100 us
int vdacPin = 10;
int pulsePin = 9;
int buzzerPin = 11;
char a;
bool ON=LOW;
int readPin = 7;
int ledPin = 13;
int freq = 1000;
bool sound = LOW;

int detectPin = A0 ; 
int long time2=0;
int long time1=0;
int delta = 0;



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
/*const*/ uint16_t TIMER_PRESCALER = 256;//notice that to change this you also need to change CSbits of TCCR1B
/*const*/ float F_TIMER = F_CPU / TIMER_PRESCALER;

void setup()
{


    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

    /* For some reason:
    Using the ICR1 Register for defining TOP works well when using fixed TOP values. By using
    ICR1, the OCR1A Register is free to be used for generating a PWM output on OC1A. However,
    if the base PWM frequency is actively changed (by changing the TOP value), using the OCR1A
    as TOP is clearly a better choice due to its double buffer feature.

    NOT SURE IS THIS IS USEFUL IN ANYWAY...
    */
    /* DDRB  |= _BV(PB1);*/

analogWrite(vdacPin, 255); //write on (5 Volt?) for VDAC, 255 for always on, and the pin is well... the pin
//digitalWrite(pulsePin,HIGH); //start the device with the stimulus off, it seems like this works
pinMode(readPin, INPUT);
pinMode(ledPin, OUTPUT);
      ON = HIGH;
      //analogWrite(vdacPin, 255); //write on (5 Volt?) for VDAC, 255 for always on, and the pin is well... the pin
      setupTimer1(pTest, PULSE_WIDTH, TIMER_PRESCALER);

//looks like analogwrite holds between loops
}

void loop() 

{

  //we need to find a way to set the output on and off
  // "The actual OC1x value will only be visible on the port pin if the data direction for the port pin is set as output (DDR_OC1x)."
  // SO maybe by modifying the data direction register we cant sort of set on/off the stimulation
  
  //VDAC could be done with a POT?

  while(Serial.available()) 
    {
    a = Serial.read();// read the incoming data as string  
    }

  if (a == '1')
    {
      if(ON)
      {
      turnOff();
      setupTimer1(p1, PULSE_WIDTH, TIMER_PRESCALER);
      Serial.println("TURNED ON"); //TURN ON STIMULATION
      //changePeriod(1e-3);
      
      }
      /*We should turn off stimuli while chaging this because if you change periods rapidly it will send a direct current for a while and then change accordingly*/
    a='c';
    }
  if (a == '2')
    {
      if(ON)
      {
      turnOff();
      setupTimer1(p2, PULSE_WIDTH, TIMER_PRESCALER);
      Serial.println("TURNED ON"); //TURN ON STIMULATION
      //changePeriod(2e-3);
      }
      a='c';
    }
    
  if (a == '0')
    {   
      turnOff();
      a='c';
    } 
  if (a == '9')
    {
      //DDRB  |= _BV(PB1);
      ON = HIGH;
      //analogWrite(vdacPin, 255); //write on (5 Volt?) for VDAC, 255 for always on, and the pin is well... the pin
      setupTimer1(pTest, PULSE_WIDTH, TIMER_PRESCALER);
      Serial.println("TURNED ON"); //TURN ON STIMULATION
      a='c';
    }

  if (a == 's')
  {
    sound = !sound;
    Serial.println("sound");
    a = "c";
  }

  if (sound)
  {
    tone(buzzerPin, freq); // Send 1KHz sound signal...
  }

  else 
  {
    noTone(buzzerPin);
  }


  //Serial.println(pulseIn(readPin,LOW,50e-6));
  
  //if(ON) 
  {
  if ( digitalRead(readPin) == HIGH) //digitalRead(pulsePin) messes the functioning, gotta do it on another pin
  {
    Serial.println("tone"); //this wont allow to turn it off after maybe a problem reading the serial
    //delay(500);
    //ON = LOW; //this wont allow to change period, it is only to not overflow the serial, this should go to the buzzer anyway
    //maybe do it on the pin 13 (led)

    digitalWrite(ledPin, HIGH);
    
    //delay(1000);        // ...for 1 sec
  
    //delay(500);//maybe is the dealay
    
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
  }

  if(analogRead(detectPin) >= HIGH)
  {
    time2=millis();
    delta = time2 - time1;
    Serial.print("current: ");
    Serial.print(time2);
    //Serial.println(time2);
    Serial.print(" delta: ");
    Serial.println(delta);
    time1=time2; //correcion, al igualar a millis no es exactamente el tiempo anterior.
    //por eso se iguala directamente a time2
    //la correcion fue a partir de 2ArduinosPeriods1, las medidas de un solo arduino fueron con el codigo anterior
  }
}


void changePeriod(float newPeriod)
{
  ICR1   = round(newPeriod * F_TIMER) -1 ;
  Serial.print("Period (ms): ");
  Serial.println(newPeriod * 1000);
}

void setupTimer1 (float PWM_PERIOD, float PULSE_WIDTH, uint16_t TIMER_PRESCALER) //with 256 prescaler
{
    //DO NOTE THAT THIS TURNS OFF THE STIMULI
    float F_TIMER = F_CPU / TIMER_PRESCALER;
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
    DDRB  |= _BV(PB1);    // set PB1 = OC1A as output, comment so it starts turned  off (maybe)

    //in the following n would be the number of the timer in general, x the channel
    //ICRn - Input Capture Register
    ICR1   = round(PWM_PERIOD * F_TIMER) -1 ;//when count is equal to ICR1 the counter is reset (going back to BOTTOM and SET (ON)) because ICR1 in the mode chosen is set to TOP.
    //OCRn - Output Compare Register 
    OCR1A  = round(PULSE_WIDTH * F_TIMER) -1 ;//when count is equal to OCR1A the ouput is set to clear (OFF) because of non-inverting compare output mode
    
    // | -> (bitwise or) 
    //TCCRnx - Timer/Counter Control Register, these are both 8bit register that control timer behaviour
    TCCR1A = _BV(COM1A1) | _BV(COM1A0)  // non-inverting PWM on OC1A Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
                          // In non-inverting Compare Output mode, the Output Compare (OC1x) is cleared on the compare match between TCNT1 and OCR1x, and set at BOTTOM.
                          // That means that when TCNT1 is equal to OCR1A we get OFF, but when we get to the TOP and then resetted to the BOTTOM, at the BOTTOM we get ON. 
                          //COM1A0 ya que el transistor del swithce invierte la señal.
                          //seems like inverting it also fixed the off part? (NOPE) sometimes it works, sometimes it doesnt
           | _BV(WGM11);  // fast PWM mode, TOP = ICR1
                          //The counter counts from BOTTOM to TOP then restarts from BOTTOM
    TCCR1B = _BV(WGM12)   // fast PWM mode, TOP = ICR1
           | _BV(WGM13)   // fast PWM mode, TOP = ICR1
           | _BV(CS12);   // clock at F_CPU/256 = 62.5 kHz, these chooses the prescaler

    Serial.print("Period (ms): ");
    Serial.println(PWM_PERIOD*1000);
    Serial.print("Pulse Width (us): ");
    Serial.println(PULSE_WIDTH*1000000);
    
}

void turnOff(void)
{
      //DDRB  = _BV(0); 
      ON = LOW;
      Serial.println("TURNED OFF"); // TURN OFF STIMULATION
      //digitalWrite(pulsePin, LOW) ; //looks like this messes with the counter , dont use, it wont work for on-off
      //digitalWrite(vdacPin, LOW);
      //analogWrite(pulsePin,0);  //doesnt work
      //OCR1A  = round(0.01e-3 * F_TIMER) -1 ;//when count is equal to OCR1A the ouput is set to clear (OFF) because of non-inverting compare output mode
      //analogWrite(pulsePin, 0);
      digitalWrite(pulsePin, HIGH); //high because is switch is a not
}
