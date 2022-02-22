// Code for modular locker project. Made with <3 by Daniel Willins, class of 22
#include <Servo.h>

// creates a subclass of Servo that includes state data for each locker  
class Locker : public Servo {
  public:
    boolean isOccupied;
    String password;
    boolean mirror; 
    String symbol;
    byte greenPin;
    byte redPin;
    
    Locker (boolean isOccupied, String password, boolean mirror, String symbol, byte greenPin, byte redPin) {
      this->isOccupied = isOccupied;
      this->password = password;
      this->mirror = mirror;
      this->symbol = symbol;
      this->greenPin = greenPin;
      this->redPin = redPin;
    }

  void closeLock(String password){
    // the servo is locked, the state is set to full and the password is set
    write(90);
    isOccupied =true;
    this->password = password;
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    Serial.println("closing locker: " + symbol);
  }

  void openLock(){
    // the servo is unlocked, the state is set to empty and the password is cleared
    write(mirror ? 180 : 0);
    isOccupied = false;
    password = "";
    digitalWrite(redPin, LOW);
    Serial.println("opening locker: " + symbol);
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

// lockers must be pointers so they can be edited in an array
Locker *A = new Locker(false, "", false, "A", greenA, redA); 
Locker *B = new Locker(false, "", true, "B", greenB, redB); 
Locker *C = new Locker(false, "", true, "C", greenC, redC); 
Locker *D = new Locker(false, "", false, "D", greenD, redD); 

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

  for (Locker *locker : lockers){
    pinMode(locker->greenPin, OUTPUT);
    pinMode(locker->redPin, OUTPUT);
  }
  
  // unlocks all servos 
  for (Locker *locker : lockers){
    locker->write(locker->mirror ? 180 : 0);
  }
  
  Serial.println("locker initialized");
}

void loop(){
  //designates a locker and lights it
  for(Locker *locker : lockers) {
    if (!locker->isOccupied) {
      designated = locker;
      digitalWrite(designated->greenPin, HIGH);
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

  Serial.println("match not found");
  // if no locker matches the password, closes the designated locker
  designated->closeLock(input);
}
