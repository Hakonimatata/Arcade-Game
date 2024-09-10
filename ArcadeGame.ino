#include <Servo.h>
#include <OLED_I2C.h>
#include <CapacitiveSensor.h>

extern uint8_t SmallFont[];

OLED myOLED(SDA, SCL); // Init oled screen
Servo hammer; // Init servo object

// pins:
const int buttonPin = 6;
const int servoPin = 9;
const int potentiometerPin = A0;
const int minStartAngle = 0;
const int maxStartAngle = 90;
const int touchPin_1 = 2;
const int touchPin_2 = 3;
const int greenLedPin = 4;
const int redLedPin = 5;

// Hammer parameters
int hammerStartPos = 0; // deg angle
int hammerEndPos = 100;

// init variables
bool hammerDown = false;
bool playerHit = false;
bool playerFooled = false;
unsigned long randNumber;
unsigned long swingTime;
unsigned long reactionTime;
unsigned long bestReactionTime = 999999;


// Capacitive sensor parameters
const int numReadings  = 10;
long readings [numReadings];
int readIndex  = 0;
long total  = 0;
const int sensitivity  = 1000;
const int thresh  = 250;
const int csStep  = 10000;
CapacitiveSensor cs  = CapacitiveSensor(touchPin_1, touchPin_2);


void setup() {
  Serial.begin(9600);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);

  randomSeed(analogRead(0)); // Set seed for random() function
  hammer.attach(servoPin);  // Set servo to use pin 9
  hammer.write(hammerStartPos);  // Set hammer to start position

  myOLED.begin();
  myOLED.setFont(SmallFont);

  //Init capacitive sensor
  
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);
  
}


void loop() {

  // while (true) {
  //   long touchValue = cs.capacitiveSensor(sensitivity);
  //   Serial.println(touchValue);
  // }

  // Wait for player to start
  waitForPlayer();

  // Game starts. 
  playGame();

  // Update high score
  updateHighScore();

  // Game is done. Print results
  printResults();
  delay(5000); // Print result for this amount of time

  // Reset all parameters
  resetGame();
  delay(1000);
}

void waitForPlayer() {

  String diff = "";

  while (!isButtonDown()) { // Wait for button to be pressed
    hammerStartPos = readPotentiometerPosition();
    hammer.write(hammerStartPos);

    if      (hammerStartPos < 30) { diff = "Easy   "; }
    else if (hammerStartPos < 50) { diff = "Hard   "; }
    else if (hammerStartPos < 65) { diff = "Brutal "; }
    else { diff = "Insane "; }

    printOLED("Difficulty: ", diff + String(hammerStartPos) + " deg.", "Hold button to play", "Best: " + String(bestReactionTime) + " ms");

    delay(10);
  }
}


void playGame() {

  printOLED("Game started", "", "", "");
  digitalWrite(greenLedPin, HIGH); // green LED when starting
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
      
      // Red light only
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);

      break;
    }
    else if(hammerDown && isButtonDown) { // Hammer swings, button still down
      // update reaction time
      reactionTime = millis() - swingTime;
      
      if (!isButtonDown()) { // button released 
        break; // break when button is released
      } 
    }
    
    delay(1); 
  }

  if (!playerFooled) {
    checkForHit();
  }
}

void checkForHit() {
  long startTime = millis();

  // Check if finger got hit
  while (millis() - startTime < 1000) { // check for one second
    long touchValue = cs.capacitiveSensor(sensitivity);
    Serial.println(touchValue);

    if(touchValue > thresh) {
      // Player got hit
      playerHit = true;
      Serial.println("Player hit");

      // Red light only
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);

    }
  }
}

void updateHighScore() {
  if (reactionTime < bestReactionTime && !playerFooled && !playerHit) {
    bestReactionTime = reactionTime;
  }
}

void printResults() {
  Serial.println("Game over");

  if(playerFooled) {
    printOLED("You got fooled!", "", "DUMBASS", "");
    Serial.println("You got fooled!");
  }
  else if (playerHit) {
    String msg1 = "";
    String msg2 = "";
    if (hammerStartPos < 30) {msg1 = "come on.."; msg2 = "even on easy??";}
    printOLED("", "You got hit", msg1, msg2);
  }
  else {
    String msg = "";
    if (reactionTime == bestReactionTime) {msg = "NEW BEST TIME!";}
    printOLED(msg, "Your time: " + String(reactionTime) + "ms", "", "Best time: " + String(bestReactionTime) + " ms");

    Serial.print("Reaction Time: ");
    Serial.print(reactionTime);
    Serial.println(" ms");
  }
}


void resetGame() {
  // Return hammer to start pos
  hammer.write(hammerStartPos);

  // Turn off lights
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);

  // Reset parameters
  hammerDown = false;
  playerFooled = false;
  reactionTime = 99999;
  playerHit = false;
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


