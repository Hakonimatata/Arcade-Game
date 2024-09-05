#include <Servo.h>

Servo hammer; // Init servo object

// init pins:
int buttonPin = 6;
int potentiometerPin = A0;

// tunable parameters: 
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
    hammerStartPos = updateHammerStartPos();
    hammer.write(hammerStartPos);
    delay(100);
  }

  // Game starts. TODO: green LED when starting?
  while(!hammerDown) {
    
    int randInt = random(1, 100);

    if (randInt == 1) {
      if(isFake()) {
        fakeSwing();
      }
      else {
        swingHammer();
      }
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

    delay(50); 
  }

  // Game is done. Print results
  
  Serial.println("Game over");

  if(playerFooled) {
    Serial.println("You got fooled!");
  }

  // Return hammer to start pos
  delay(1000);
  hammer.write(hammerStartPos);

  // Reset parameters
  hammerDown = false;
  playerFooled = false;
  delay(1000);
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

int updateHammerStartPos() {
  int potentiometerVal = analogRead(potentiometerPin);
  int scaledVal = map(potentiometerVal, 0, 1023, 0, 60); // Potentiometer from 0 to 60 deg
  return scaledVal;
}
