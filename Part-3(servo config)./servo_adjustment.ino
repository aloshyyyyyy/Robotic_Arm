#include <Servo.h>

Servo servo;

int currentPos = 0;  // Tracks where the servo currently is
int targetPos = 0;   // Stores the angle you type in the Serial Monitor
int stepDelay = 10;  // Adjust this to change speed (higher = slower)

void setup() {
  Serial.begin(9600);   // Start serial communication
  servo.attach(9);      // Servo signal on pin 9
  servo.write(currentPos); // Initialize at 0 degrees
  
  Serial.println("Enter an angle between 0 and 180:");
}

void loop() {
  // The code stays right here doing nothing until you input a new angle
  if (Serial.available() > 0) {
    
    // Read the incoming integer
    int input = Serial.parseInt();
    
    // Clear any leftover characters (like spaces or newlines) so they don't mess up the next input
    while(Serial.available() > 0) {  
      Serial.read(); 
      delay(2); 
    }

    // Make sure the input is a valid servo angle
    if (input >= 0 && input <= 180) {
      targetPos = input;
      Serial.print("Moving slowly to: ");
      Serial.println(targetPos);
      
      // Move SLOWLY to the target position
      moveSlowlyTo(targetPos);
    } else {
      Serial.println("Invalid angle! Please enter a value between 0 and 180.");
    }
  }
}

// Function to smoothly step the servo to the new position
void moveSlowlyTo(int target) {
  // If the target is greater than the current position, move forward
  if (currentPos < target) {
    for (int i = currentPos; i <= target; i++) {
      servo.write(i);
      delay(stepDelay);
    }
  } 
  // If the target is less than the current position, move backward
  else if (currentPos > target) {
    for (int i = currentPos; i >= target; i--) {
      servo.write(i);
      delay(stepDelay);
    }
  }
  
  // Update the current position tracker
  currentPos = target; 
  Serial.print("Holding position at: ");
  Serial.println(currentPos);
}
