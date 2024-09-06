#include <Servo.h>
#include <OLED_I2C.h>
extern uint8_t SmallFont[];

OLED myOLED(SDA, SCL); // Init oled screen
Servo hammer; // Init servo object

// pins:
const int buttonPin = 6;
const int servoPin = 9;
const int potentiometerPin = A0;
const int minStartAngle = 0;
const int maxStartAngle = 90;

// tunable parameters: 
int hammerStartPos = 0; // deg angle
int hammerEndPos = 110;

bool hammerDown = false;
bool playerFooled = false;
unsigned long randNumber;
unsigned long swingTime;
unsigned long reactionTime;
unsigned long bestReactionTime = 999999;

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
  // Wait for player to start
  waitForPlayer();

  // Game starts. TODO: green LED when starting?
  playGame();

  // Update high score
  updateHighScore();

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

    printOLED("Difficulty (degrees): ", String(hammerStartPos), "Hold button to start", "");

    delay(10);
  }
}

void playGame() {

  printOLED("Game started", "", "", "");

  Serial.println("Starting Game");
  delay(1000); // delay before the hammer has a chance to start

  while(true) {
    
    randNumber = random(1, 1000);

    if (randNumber == 1 && !hammerDown) {
      if(isRealSwing()) {
        swingHammer();
      }
      else {
        fakeSwing();
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

void updateHighScore() {
  if (reactionTime < bestReactionTime) {
    bestReactionTime = reactionTime;
  }
}

void printResults() {
  Serial.println("Game over");

  if(playerFooled) {
    printOLED("You got fooled!", "dumbass", "", "");
    
    Serial.println("You got fooled!");
  } else {
    printOLED("Your reaction time is", String(reactionTime) + "ms", "", "Best time: " + String(bestReactionTime) + " ms");

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


bool isRealSwing() {
  return random(0, 4) == 1; // generates 0 - 3. 1/4 chance
}

void swingHammer() {
  hammer.write(hammerEndPos);
  hammerDown = true;
  swingTime = millis(); // Record the time when hammer swings
  Serial.println("Real swing");
}

void fakeSwing() {
  hammer.write(hammerStartPos + 10);
  delay(100);
  hammer.write(hammerStartPos);
  Serial.println("Fake swing");
}

int readPotentiometerPosition() {
  int potentiometerVal = analogRead(potentiometerPin);
  int scaledVal = map(potentiometerVal, 0, 1023, minStartAngle, maxStartAngle);
  return scaledVal;
}

bool isButtonDown() {
  return digitalRead(buttonPin) == HIGH; // Set to high because button is normally down
}

void printOLED(String item1, String item2, String item3, String item4){
  myOLED.clrScr();
  myOLED.print(item1, CENTER, 0);
  myOLED.print(item2, CENTER, 16);
  myOLED.print(item3, CENTER, 32);
  myOLED.print(item4, CENTER, 48);
  myOLED.update();
}
