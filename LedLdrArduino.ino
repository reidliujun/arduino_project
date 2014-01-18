#include <TinkerKit.h>
TKLightSensor ldr(I0); 
TKLed led(O0);   
void setup() {
  
  Serial.begin(9600);
  // configure output pin:
  //pinMode (analogLed, OUTPUT);
  // set XBee's destination address:
  setDestination();
  // blink the TX LED indicating that the main program's about to start:
  //blink(analogLed, 3);
}

void setDestination(){
  Serial.print("+++");
  // wait for the radio to respond with "OK\r"
  char thisByte = 0;
  while (thisByte != '\r') {
    if (Serial.available() > 0) {
      thisByte = Serial.read();
    }
  }
  Serial.print("ATDH0, DL5678\r");
  Serial.print("ATMY1234\r");
  Serial.print("ATID1111\r");
  Serial.print("ATCN\r");
 
}
void loop() {
//  String line= "";
  short brightnessVal = ldr.get();
  Serial.print("C 1 11 ");
  Serial.println(brightnessVal);
  delay(500);
  readCommand();
}

void readCommand(){
  while (Serial.available() > 0){
    char c=Serial.read();
    if (c=='C'){
      int commendID = Serial.parseInt();
      
      if (commendID == 2){
        Serial.println("Get command from server:");
        int nodeID = Serial.parseInt();
        if (nodeID == 11){
          int ledValue = Serial.parseInt();
          Serial.print("Set LED value to:");
          Serial.println(ledValue);
          led.brightness(ledValue);
          sendAck(ledValue);
        }
      }
      
    }else{
      while (c!= '\n' && Serial.available()>0){
        c=Serial.read(); 
      }
    }
  }
}

void sendAck(int ledValue){
  Serial.print("C 3 11 ");
  Serial.println(ledValue);
}




//void sendBrightValue(short brightnessVal){
//  Serial.write(1);
////  Serial.write(brightnessVal);
//  Serial.write(11);
//  Serial.write(brightnessVal>>8);
//  Serial.write(brightnessVal);
//}
//void readCommand(){
//  char buffer[4];
//  Serial.readBytes(buffer,4);
//  int val=0;
//  
//  if (buffer[0]==2){
////    val = 900;
//    //led.brightness(900);
//    val = val | (0x000000FF & buffer[2]);
//    val = val << 8;
//    val = val | (0x000000FF & buffer[3]);
//    led.brightness(val);
//  }
////  led.brightness(val);
//}
