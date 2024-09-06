#include <Servo.h>
#include <OLED_I2C.h>
extern uint8_t SmallFont[];


OLED  myOLED(SDA, SCL);

Servo hammer; // Init servo object

// pins:
const int buttonPin = 6;
const int servoPin = 9;
const int potentiometerPin = A0;

// tunable parameters: 
int hammerStartPos = 0; // deg angle
int hammerEndPos = 90;


bool hammerDown = false;
bool playerFooled = false;
unsigned long randNumber;
unsigned long swingTime;
unsigned long reactionTime;

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  randomSeed(analogRead(0)); // Set seed for random() function
  hammer.attach(servoPin);  // Set servo to use pin 9
  hammer.write(hammerStartPos);  // Set hammer to start position

  myOLED.begin();
  myOLED.setFont(SmallFont);
}


void loop() {

  waitForPlayer();

  // Game starts. TODO: green LED when starting?
  playGame();

  // Game is done. Print results
  printResults();
  delay(1000);

  // Reset all parameters
  resetGame();
  delay(1000);
}

void waitForPlayer() {
  while (!isButtonDown()) { // Wait for button to be pressed
    hammerStartPos = readPotentiometerPosition();
    hammer.write(hammerStartPos);

    printOLED("Difficulty: ", hammerStartPos, "Score", 0);

    delay(100);
  }
}

void playGame() {
  
  Serial.println("Starting Game");

  while(true) {
    
    randNumber = random(1, 5000);

    if (randNumber == 1 && !hammerDown) {
      if(isFake()) {
        fakeSwing();
      }
      else {
        swingHammer();
      }
    }
    
    // Check if button is released
    if(!isButtonDown() && !hammerDown) { // button released when hammer is still up
        playerFooled = true;
        break;
    }
    else if(hammerDown && isButtonDown) { // Hammer swings, button still down
      // update reaction time
      reactionTime = millis() - swingTime;
      
      if (!isButtonDown()) break; // break when button is released
    }

    delay(1); 
  }
}



void printResults() {
  Serial.println("Game over");

  if(playerFooled) {
    Serial.println("You got fooled!");
  } else {
    Serial.print("Reaction Time: ");
    Serial.print(reactionTime);
    Serial.println(" ms");
  }
}


void resetGame() {
  // Return hammer to start pos
  hammer.write(hammerStartPos);

  // Reset parameters
  hammerDown = false;
  playerFooled = false;
  reactionTime = 0;
}


bool isFake() {
  return random(0, 2) == 1; // generates either 0 or 1. 50% chance
}

void swingHammer() {
  hammer.write(hammerEndPos);
  hammerDown = true;
  swingTime = millis(); // Record the time when hammer swings
  Serial.println("Real swing");
}

void fakeSwing() {
  hammer.write(hammerStartPos + 4);
  delay(100);
  hammer.write(hammerStartPos);
  Serial.println("Fake swing");
}

int readPotentiometerPosition() {
  int potentiometerVal = analogRead(potentiometerPin);
  int scaledVal = map(potentiometerVal, 0, 1023, 0, 60); // Potentiometer from 0 to 60 deg
  return scaledVal;
}

bool isButtonDown() {
  return digitalRead(buttonPin) == LOW;
}

void printOLED(String item1, int item2, String item3, int item4){
  myOLED.clrScr();
  myOLED.print(item1, CENTER, 0);
  myOLED.print(String(item2) + "%", CENTER, 16);
  myOLED.print(item3, CENTER, 32);
  myOLED.print(String(item4) + " ms", CENTER, 48);
  myOLED.update();
}
