// Code for modular locker project. Written by Daniel Willins, class of 22
#include <Servo.h>

const byte numLockers = 1;
byte i;

String input = "";

Servo A, B, C, D;

// creates lists of the servos, their states, and their passwords
Servo locks [numLockers] = {A};
bool isOccupied [numLockers] = {false};
String Ids [numLockers] = {""};

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
}
  
void loop(){
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
}

void openLock(int index){
  Serial.print("opening locker : ");
  Serial.println(index + 1);
	locks[index].write(0);
	isOccupied[index] = false;
  Ids[index] = ""; 
  input = "";
}