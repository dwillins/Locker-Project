// Code for modular locker project. Written by Daniel Willins, class of 22
#include <Servo.h>

const byte numLockers = 4;
byte i;

String input = "";

Servo A, B, C, D;

// creates lists of the servos, their states, and their passwords
Servo locks [numLockers] = {A, B, C, D};
bool isOccupied [numLockers] = {false, false, false, false};
String Ids [numLockers] = {"", "", "", ""};
// frist pin is green, second is red
byte ledPins[numLockers][2] = {{9, 8}, {7, 6}, {5, 4}, {3,2}};

void setup(){
	Serial.begin(9600);
	// creates the servos and makes them stay in place
	A.attach(13);
	B.attach(12);
	C.attach(11);
	D.attach(10);
	A.write(0);
	B.write(0);
	C.write(0);
	D.write(0);

  for (i = 2; i < 10; i++) {
    pinMode(i,OUTPUT);
  }
}
  
void loop(){
  // designates the first empty locker for use
  for (i = 0; i < numLockers; i++) {
    if (!isOccupied[i]) {
      digitalWrite(ledPins[i][0],HIGH);
      break;
    }  
  }
  
  // gets the rfid as one string
  while (input.length() < 30) {
    if (Serial.available() > 0) {
      input += Serial.read();
      Serial.flush();
    }
  }
  
  Serial.println(input);
  // iterates through the lockers, if one has the matching password, opens it
  for (i = 0; i < numLockers; i++){
    if (input.equals(Ids[i])) {
      openLock(i);
      //breaks the loop and returns to the start
  	  return;
    } 
  }
  
  // iterates through the lockers and reserves the first available one
  for (i = 0; i < numLockers; i++) {
    if (!isOccupied[i]){
      closeLock(i);
      //breaks the loop and returns to the start
      return;
    }
  }
}

void closeLock(int index){
  Serial.print("closing locker : ");
  Serial.println(index + 1);
  locks[index].write(90);
  isOccupied[index] = true;
  Ids[index] = input;
  input = "";
  digitalWrite(ledPins[index][1], HIGH);
  digitalWrite(ledPins[index][0], LOW);
}

void openLock(int index){
  Serial.print("opening locker : ");
  Serial.println(index + 1);
	locks[index].write(0);
	isOccupied[index] = false;
  Ids[index] = ""; 
  input = "";
  digitalWrite(ledPins[index][1], LOW);
}
