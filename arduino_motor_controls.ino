// Arduino Code (arduino_motor_controls.ino)

#include <AccelStepper.h>

// Define motor pins
#define DIR_PIN 3
#define STEP_PIN 2

// Define the probe pin
#define PROBE_PIN A0

// Define the motor interface type: 1 for a stepper driver
#define motorInterfaceType 1

// Create a new AccelStepper object
AccelStepper stepper(motorInterfaceType, STEP_PIN, DIR_PIN);

// Set motor parameters
const int stepsPerRevolution = 6400;

// Variables for communication and state
char command;
int value;
bool running = false;
bool dirInverted = false; // Tracks the current direction state

// Timing variables for non-blocking probe check
unsigned long lastProbeCheck = 0;
const unsigned long probeInterval = 10; // 10 milliseconds

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(40000.0);
  stepper.setAcceleration(50000.0);
  
  // Configure the probe pin as an input with a pull-up resistor
  pinMode(PROBE_PIN, INPUT_PULLUP);
}

void loop() {
  // Check for incoming serial data (non-blocking)
  if (Serial.available() > 0) {
    command = Serial.read();

    if (command == 'S') { // Set Speed command
      while (Serial.available() == 0) {
        // Wait for value to be sent
      }
      value = Serial.parseInt();
      stepper.setSpeed(value);
      Serial.print("Speed set: ");
      Serial.println(value);
      running = true;
    } else if (command == 'T') { // Toggle Direction command
      dirInverted = !dirInverted; // Flip the state
      stepper.setPinsInverted(dirInverted, false, false);
      Serial.println("Direction toggled.");
    } else if (command == 'X') { // Stop Motor command
      stepper.stop();
      running = false;
      Serial.println("Motor stopped.");
    }
  }

  // Run the motor if a speed is set (non-blocking)
  if (running) {
    stepper.runSpeed();
  }

  // Check the probe pin every 10ms (non-blocking timer)
  if (millis() - lastProbeCheck >= probeInterval) {
    lastProbeCheck = millis();
    
    // Check if the pin is pulled LOW
    if (digitalRead(PROBE_PIN) == HIGH) {
      Serial.println("PROBE_HIGH");
    }
  }
}