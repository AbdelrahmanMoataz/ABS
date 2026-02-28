String x;
unsigned long time = 1000;
unsigned long last = 0;

void setup() {
  Serial.begin(9600);
  //Serial.setTimeout(10);
}

void loop() {
    while(!Serial.available());
    x = Serial.readString();
    Serial.print(x);
  
    
  if(x == "S")
    time*= 2;

  if(x == "s")
    time/= 2;

  if(millis() - last > time)
  {
    digitalWrite(9,!digitalRead(9));
    last = millis();
  }
    


}