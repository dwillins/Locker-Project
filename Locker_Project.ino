// Code for modular locker project. Made with <3 by Daniel Willins, class of 22
#include <Servo.h>

// creates a subclass of Servo that includes state data for each locker  
class Locker : public Servo {
  public:
    boolean isOccupied;
    boolean mirror;
    String password; 
    String symbol;
    byte greenPin;
    byte redPin;
    
    Locker (boolean full, boolean swapped, String id, String letter, byte green, byte red) {
      isOccupied = full;
      mirror = swapped;
      password = id;
      symbol = letter;
      greenPin = green;
      redPin = red;
    }
  
  void closeLock(String code){
    Serial.println("closing locker: " + symbol);
    // the servo is locked, the state is set to full and the password is set
    write(90);
    isOccupied = true;
    password = code;
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
  }

  void openLock(){
    Serial.println("opening locker: " + symbol);
    // the servo is unlocked, the state is set to empty and the password is cleared
    write(mirror ? 180 : 0);
    isOccupied = false;
    password = "";
    digitalWrite(redPin, LOW);
  }
};

String input;
const byte numLockers = 4;

const byte redA = 9;
const byte greenA = 8;
const byte redB = 7;
const byte greenB = 6;
const byte redC = 5;
const byte greenC = 4;
const byte redD = 3;
const byte greenD = 2;

Locker A(false, false, "", "A", greenA, redA); 
Locker B(false, true, "", "B", greenB, redB); 
Locker C(false, true, "", "C", greenC, redC); 
Locker D(false, false, "", "D", greenD, redD); 
  
Locker lockers [numLockers] = {A, B, C, D};
Locker * designated;

void setup(){
  Serial.begin(9600);  

  // pinout for servos and LEDs
  A.attach(13);
  B.attach(12);
  C.attach(11);
  D.attach(10);

  for (Locker locker : lockers){
    pinMode(locker.greenPin, OUTPUT);
    pinMode(locker.redPin, OUTPUT);
  }
  
  // unlocks all servos 
  for (Locker locker : lockers){
    locker.write(locker.mirror ? 180 : 0);
  }
  Serial.println("lockers unlocked");
}

void loop(){
  //designates a locker and lights it
  for (Locker locker : lockers) {
    if (locker.isOccupied == false){
      designated = &locker;
      digitalWrite(designated->greenPin, HIGH);
      Serial.println("designating locker: " + locker.symbol);
      break;
    }
  }
  
  input = "";
  Serial.println("input cleared");
  // gets the rfid as one string
  while (input.length() < 30) {
    Serial.println("waiting");
    if (Serial.available() > 0) {
      input += Serial.read();
      Serial.flush();
    }
  }
  
  Serial.println(input);
  
  // iterates through the lockers, if one has the matching password, opens it
  for (int i = 0; i < 4; i++) {
    if (lockers[i].password.equals(input)) {
      Serial.println("password found");
      lockers[i].openLock();
      // restarts void loop() so the designated locker will not be closed
      return;
    } 
  }

  Serial.println("password not found");
  
  // if no locker matches the password, closes the designated locker
  designated->closeLock(input);
}
