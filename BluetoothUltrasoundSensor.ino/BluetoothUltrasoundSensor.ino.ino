#include <ArduinoBLE.h>
BLEService newService("180A");

BLEByteCharacteristic readChar("2A58", BLERead);
BLEByteCharacteristic writeChar("2A57", BLEWrite);

long previousMillis = 0;

const int trigPin = 8;
const int echoPin = 10;
long duration;
int distanceCm, distanceInch;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  while(!Serial);
  if (!BLE.begin()){
    Serial.println("Waiting for ArduinoBLE");
    while(1);
  }

  BLE.setDeviceName("Danika and Alice");
  BLE.setAdvertisedService(newService);
  newService.addCharacteristic(readChar);
  newService.addCharacteristic(writeChar);
  BLE.addService(newService);
  
  readChar.writeValue(0);
  writeChar.writeValue(0);

  BLE.advertise();
  Serial.println("Bluetooth device active");
}

void loop() {

  BLEDevice central = BLE.central(); // wait for a BLE central

  if (central) {  // if a central is connected to the peripheral
    Serial.print("Connected to central: ");
    
    Serial.println(central.address()); // print the central's BT address
    
    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection

    while (central.connected()) { // while the central is connected:
      long currentMillis = millis();
      
      if (currentMillis - previousMillis >= 200) { 
        previousMillis = currentMillis;

        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        duration = pulseIn(echoPin, HIGH);
        
        distanceCm = duration * (0.0343/2); // speed of sound in cm/us
        distanceInch = duration * (0.013504/2); // speed of sound in in/us
        Serial.print("Distance: ");
        Serial.print(distanceCm);
        Serial.print(" cm/");
        Serial.print(distanceInch);
        Serial.println(" in");
        delay(1000);
        
        readChar.writeValue(distanceCm);
        Serial.println("Distance printed to peripheral");
      }
    }

  }

}
