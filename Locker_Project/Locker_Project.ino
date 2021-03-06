// Code for modular locker project. Made with <3 by Daniel Willins, class of 22
#include <Servo.h>

// creates a subclass of Servo that includes state data for each locker  
class Locker : public Servo {
  public:
    boolean isOccupied = false;
    String password = "";
    boolean mirror; 
    String symbol;
    byte ledPin;
    
    Locker (boolean mirror, String symbol, byte ledPin) {
      this->mirror = mirror;
      this->symbol = symbol;
      this->ledPin = ledPin;
    }
    
  void closeLock(String password){
    // the servo is locked, the state is set to full and the password is set
    write(90);
    isOccupied = true;
    this->password = password;
    digitalWrite(ledPin, LOW);
    Serial.println("closing locker: " + symbol);
  }

  void openLock(){
    // the servo is unlocked, the state is set to empty and the password is cleared
    write(mirror ? 180 : 0);
    isOccupied = false;
    password = "";
    Serial.println("opening locker: " + symbol);
  }
};

String input;
const static byte numLockers = 4;

const byte pinA = 9;
const byte pinB = 8;
const byte pinC = 7;
const byte pinD = 6;

// lockers must be pointers so they can be edited in an array
Locker *A = new Locker(false, "A", pinA); 
Locker *B = new Locker(true, "B", pinB); 
Locker *C = new Locker(true, "C", pinC); 
Locker *D = new Locker(false, "D", pinD); 

// Array of lockers to iterate through
Locker *lockers [numLockers] = {A, B, C, D};
// variable used to store designated locker
Locker *designated;

void setup(){
  Serial.begin(9600);  

  // pinout for servos and LEDs
  A->attach(13);
  B->attach(12);
  C->attach(11);
  D->attach(10);

  // sets all LED pins as digital outputs
  for (Locker *locker : lockers){
    pinMode(locker->ledPin, OUTPUT);
  }
  
  // unlocks all servos 
  for (Locker *locker : lockers){
    locker->write(locker->mirror ? 180 : 0);
  }
  
  Serial.println("locker initialized");
}

void loop(){
  // designates a locker and lights it
  digitalWrite(designated->ledPin, LOW);
  for(Locker *locker : lockers) {
    if (!locker->isOccupied) {
      designated = locker;
      digitalWrite(designated->ledPin, HIGH);
      Serial.println("designating locker: " + designated->symbol);
      break;
    }
  }
  
  Serial.println();
  
  // gets the rfid as one string
  input = "";
  while (input.length() < 30) {
    if (Serial.available() > 0) {
      input += Serial.read();
      Serial.flush();
    }
  }
  
  Serial.println("Scan Result: " + input);
  
  // iterates through the lockers, if one has the matching password, opens it
  for (Locker *locker : lockers) {
    if (locker->password.equals(input)) {
      Serial.println("match found");
      locker->openLock();
      // restarts void loop() so the designated locker will not be closed
      return;
    } 
  }

  // if no locker matches the password and the unit isn't full, closes the designated locker
  Serial.println("match not found");
  if (A->isOccupied && B->isOccupied && C->isOccupied && D->isOccupied) {
    Serial.println("unit full");
    blinkLeds();
  } else { 
    designated->closeLock(input);
  }
}

void blinkLeds() {
  for (int i = 0; i < 5; i++) {
    for (Locker *locker : lockers) {
      digitalWrite(locker->ledPin, HIGH);
    }
    delay(100);
    for (Locker *locker : lockers) {
      digitalWrite(locker->ledPin, LOW);
    }
  }
}
