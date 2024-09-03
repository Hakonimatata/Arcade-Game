#include <Servo.h>

Servo hammer;

int buttonPin = 6;
int hammerStartPos = 0; // deg angle
int hammerEndPos = 60;

bool hammerDown = false;
bool playerFooled = false;

unsigned long randNumber;

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  randomSeed(analogRead(0)); // Set seed for random() function
  hammer.attach(9);  // Set servo to use pin 9
  hammer.write(hammerStartPos);  // Set hammer to start position

}

// Aproach 2
void loop() {

  while (digitalRead(buttonPin) == HIGH) {
    // Wait for button to be pressed
    delay(200);
  }

  // Game starts. TODO: green LED when starting?
  while(!hammerDown) {
    
    int randInt = random(1, 50);

    if (randInt == 1) {
      swingHammer();
    }
    else if (randInt == 2) {
      fakeSwing();
    }

    // Check if button is released
    if(digitalRead(buttonPin) == HIGH) { // button released
      if (!hammerDown) { // Fooled
        playerFooled = true;
        break;
      }
      else {
        hammerDown = true;
      }
    }

    delay(100); 
  }
  // Game is done. Display red indicator if player got fooled
  // Remember to reset hammer to start position

  delay(1000);
  hammer.write(hammerStartPos);

  Serial.println("Game over");

  // Reset parameters
  hammerDown = false;
  playerFooled = false;

}


bool isFake() {
  return random(0, 2) == 1; // generates either 0 or 1. 50% chance
}

void swingHammer() {
  hammer.write(hammerEndPos);
  hammerDown = true;
  Serial.println("Real swing");
}

void fakeSwing() {
  hammer.write(hammerStartPos + 2);
  delay(100);
  hammer.write(hammerStartPos);
  Serial.println("Fake swing");

}

