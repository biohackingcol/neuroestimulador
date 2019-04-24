/*
 * Pulse Count
 */


int detectPin = A0 ; 
int long time2=0;
int long time1=0;
int delta = 0;
void setup()
{
    Serial.begin(9600);
    pinMode(detectPin,INPUT);
}

void loop() 

{
  //time2 = millis();
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
  
  //val = analogRead(detectPin);  // read the input pin
  //Serial.println(val);          // debug value
}
