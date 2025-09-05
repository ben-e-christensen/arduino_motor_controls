// Arduino Code (arduino_motor_controls.ino)

#include <AccelStepper.h>

// Define motor pins
#define DIR_PIN 3
#define STEP_PIN 2

// Define the motor interface type: 1 for a stepper driver
#define motorInterfaceType 1

// Create a new AccelStepper object
AccelStepper stepper(motorInterfaceType, STEP_PIN, DIR_PIN);

// Set motor parameters
const int stepsPerRevolution = 6400;

// Variables for communication
char command;
int value;
bool running = false;

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(40000.0);
  stepper.setAcceleration(50000.0);
  
  // Set initial direction
  stepper.setPinsInverted(false, false, false);
}

void loop() {
  // Check for incoming serial data
  if (Serial.available() > 0) {
    command = Serial.read();

    if (command == 'S') { // Set Speed command
      while (Serial.available() == 0) {
        // Wait for value to be sent
      }
      value = Serial.parseInt();
      stepper.setSpeed(value);
      Serial.println("Speed set.");
      running = true;
    } else if (command == 'R') { // Reverse Direction command
      stepper.setPinsInverted(true, false, false);
      Serial.println("Direction reversed.");
    } else if (command == 'D') { // Forward Direction command
      stepper.setPinsInverted(false, false, false);
      Serial.println("Direction set to forward.");
    } else if (command == 'X') { // Stop Motor command
      stepper.stop();
      running = false;
      Serial.println("Motor stopped.");
    }
  }

  // Run the motor if a speed is set
  if (running) {
    stepper.runSpeed();
  }
}