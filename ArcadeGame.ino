const int swingProbRange[2] = {0, 1};
const int fakeSwingProbRange[2] = {2, 4};
int angleShift = 0;


unsigned long randNumber;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  randomSeed(analogRead(0));


}

void loop() {
  // put your main code here, to run repeatedly:

  // Generate random number
  randNumber = random(100);




  if(randNumber >= swingProbRange[0] && randNumber <= swingProbRange[1]){
    // Turn hammer down
    swingHammer();

  }
  else if(randNumber >= fakeSwingProbRange[0] && randNumber <= fakeSwingProbRange[1]){
    // Turn hammer just a bit, the go back
    fakeSwing();
  }




  delay(100);


}

void fakeSwing(){

}


void swingHammer(){

}