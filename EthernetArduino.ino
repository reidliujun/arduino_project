/*
  Reading a serial ASCII-encoded string.
 
 This sketch demonstrates the Serial parseInt() function.
 It looks for an ASCII string of comma-separated values.
 It parses them into ints, and uses those to fade an RGB LED.
 
 Circuit: Common-anode RGB LED wired like so:
 * Red cathode: digital pin 3
 * Green cathode: digital pin 5
 * blue cathode: digital pin 6
 * anode: +5V
 
 created 13 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 */

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177);

// Enter the IP address of the server you're connecting to:
//IPAddress server(1,1,1,1); 
char server[] = "ec2-107-22-111-91.compute-1.amazonaws.com";
// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 23 is default for telnet;
// if you're using Processing's ChatServer, use  port 10002):
EthernetClient client;

// pins for the LEDs:
//const int redPin = 3;
//const int greenPin = 5;
//const int bluePin = 6;
int incomingByte = 0;
String inputString = "";
int i;
char buffer[4];

void setup() {
  // configure serial communications:
//  Serial.begin(9600);
  // configure output pin:
  //pinMode (analogLed, OUTPUT);
  // set XBee's destination address:
//  setDestination();
  Ethernet.begin(mac, ip);
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  setDestination();//? not runnning for the next code
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 8080)) {
    Serial.println("connected");
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  

  
}

void setDestination() {
  // put the radio in command mode:
  Serial.print("+++");
  // wait for the radio to respond with "OK\r"
  char thisByte = 0;
  while (thisByte != '\r') {
    if (Serial.available() > 0) {
      thisByte = Serial.read();
    }
  }
  // set the destination address, using 16-bit addressing.
  // if you're using two radios, one radio's destination
  // should be the other radio's MY address, and vice versa:
  Serial.print("ATDH0, DL1234\r");
  // set my address using 16-bit addressing:
  Serial.print("ATMY5678\r");
  // set the PAN ID. If you're working in a place where many people
  // are using XBees, you should set your own PAN ID distinct
  // from other projects.
  Serial.print("ATID1111\r");
  // put the radio in data mode:
  Serial.print("ATCN\r");
  
}

void loop() {
//    // if there are incoming bytes available 
//  // from the server, read them and print them:
//  if (client.available()) {
//    char c = client.read();
//    Serial.print(c);
//  }

  // as long as there are bytes in the serial queue,
  // read them and send them out the socket if it's open:
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == 'C'){
      int commandID = Serial.parseInt();
      int nodeId = Serial.parseInt();
      
      if (client.connected()) {
        if (commandID == 1){
          int ldrValue = Serial.parseInt();
          Serial.print("Send to Server: ");
          Serial.print("C");
          Serial.print(" ");
          Serial.print(commandID);
          Serial.print(" ");
          Serial.print(nodeId);
          Serial.print(" ");
          Serial.println(ldrValue);
        //Serial.print(inChar);
          client.print("C");
          client.print(" ");
          client.print(commandID);
          client.print(" ");
          client.print(nodeId);
          client.print(" ");
          client.println(ldrValue);
        }
        if(commandID == 3){
          int ledValue = Serial.parseInt();
          Serial.print("Send ACK to Server: ");
          Serial.print("C");
          Serial.print(" ");
          Serial.print(commandID);
          Serial.print(" ");
          Serial.print(nodeId);
          Serial.print(" ");
          Serial.println(ledValue);
        //Serial.print(inChar);
          client.print("C");
          client.print(" ");
          client.print(commandID);
          client.print(" ");
          client.print(nodeId);
          client.print(" ");
          client.println(ledValue);
        }
      }
      
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing:
  }
  
  
  
  while (client.available() > 0) {
    char c = client.read();
    if (c == 'C') {
      int commandId = client.parseInt();
      int nodeId = client.parseInt();
      if (commandId == 2) {
        int ledValue = client.parseInt();
        Serial.println("Got command ");
        Serial.print("C 2 11");
        Serial.print(" ");
        Serial.println(ledValue);
      } 
    }
  }
}









