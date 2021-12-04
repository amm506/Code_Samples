#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

/********************************************************************************************************************
 * To give some context, this file was for a Jr. Design project that was completed Dec 2020. The goal of this project
 * was to make a game that was similar to a Bop-It. I was solely responsible for the software design of this project. 
 * The game had a music theme, and the inputs for it were a button (Drum), a sliding potentiometer (Trombone) and 
 * a joystick(Guitar). This project used an I2C screen to display the score at the end of the game and it also 
 * displayed the expected input. In addition, there was a buzzer that made a sound that coincided with desired input.
 * *****************************************************************************************************************/
 
 //Assigning to pins on the Arduino
 const int PotWiper = PC0;
 const int XJoy = A2;
 const int YJoy = A1;
 const int DrumIn = PD0;

  //bool values named so that True and False makes it clear.
  // I.E. correctInput = True/1 is a correct input and correctInput = False/0 is incorrect
  bool currentGame = 0;
  unsigned int currentScore = 0;
  bool drumButtonIn = 0;
  bool joystickIn = 0;
  bool potIn = 0;
  bool correctInput = 0;
  int inputSelect = 0;
  unsigned int waitTime = 0;

  LiquidCrystal_I2C lcd(0x27, 16, 2);

  int potVal = 0;
  int XJoyVal = 0;
  int YJoyVal = 0;

void setup() {
  // setup all of the inputs/outputs
 pinMode(PotWiper, INPUT);
 pinMode(XJoy, INPUT);
 pinMode(YJoy, INPUT);
 pinMode(DrumIn, INPUT);

 //initialize the screen
 lcd.init();
 lcd.backlight();
  

}

void loop() {
  // this is the main loop of the program. While(1==1) was not neccesary but was put for redundancy
  //I broke this loop into 4 function calls, which are outlined below.
while(1 == 1){
variableInitialize();
standbyState();
unsigned int finalScore = gameplay();
output();
}

}
  
  



void variableInitialize(){
  // initializes all the variables that will be used for gameplay
  
  currentGame = 0;
  currentScore = 0;
  drumButtonIn = 0;
  joystickIn = 0;
  potIn = 0;
  correctInput = 0;
  inputSelect = 0;
  waitTime = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press drum to");
  lcd.setCursor(0,1);
  lcd.print("begin new game");
  delay(100);

}

void standbyState(){
  // hangs in standby until a game is started
  
  while(digitalRead(DrumIn) != LOW){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("New game started");
    delay(1000);
    currentGame = 1;
   }
  
  // when we exit the dead loop on drum button press, start the game
 
}


int randomVarForIn(){
  // selects one of the three inputs to expect
  // 1 is drum, 2 is guitar, 3 is trombone
  return random(1, 4); // random number between min(1) and max(which is maximum -1)
}

void inputSound(int inSel){
  switch(inSel){
      case 1: // drum
        tone(1, 2000, 100);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Drum");
        delay(1000);
        break;
      case 2: // guitar
        tone(1, 2200, 250);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Guitar");
        delay(1000);
        break;
      case 3: // trombone
        tone(1, 2400, 400);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Trombone");
        delay(1000);
        break;
      default:
        break;
  }
}

unsigned int setTimer(){
  //As the score was lower, it gave more time in between the input and when the player "failed"
  //As the score went up, it gave less time

  //Maximum score of 100

  if(currentScore <= 10){
    waitTime = 5000;
  }
  else if(currentScore <= 20){
    waitTime = 4500;
  }
  else if(currentScore <= 30){
    waitTime = 4000;
  }
  else if(currentScore <= 40){
    waitTime = 3500;
  }
  else if(currentScore <= 50){
    waitTime = 3000;
  }
  else if(currentScore <= 60){
    waitTime = 2500;
  }
  else if(currentScore <= 70){
    waitTime = 2000;
  }
  else if(currentScore <= 80){
    waitTime = 1500;
  }
  else if(currentScore <= 90){
    waitTime = 1000;
  }
  else if(currentScore <= 100){
    waitTime = 500;
  }
  else{
    waitTime = 0;
  }

  return waitTime;

}
 
bool validateInput(int in, unsigned int ti){
  //Checked for correct/incorrect input WRT desired input. Also stopped when time expired.
  unsigned int currTime = millis();
  lcd.clear();

  while(currTime + ti >= millis()){
    switch(in){
      case 1:
        while(drumButtonIn == false){
          if(digitalRead(DrumIn) != LOW){
           delay(200);
           return true;
          }
          if(((analogRead(XJoy) > 515 || analogRead(XJoy) < 502) || (analogRead(YJoy) > 515 || analogRead(YJoy) < 490)) || analogRead(PotWiper) > 100 || currTime + ti < millis()){
            return false;
          }
        }
        break;
      case 2:
        while(joystickIn == false){
          if((analogRead(XJoy) > 515 || analogRead(XJoy) < 502) || (analogRead(YJoy) > 515 || analogRead(YJoy) < 490)){
          delay(500);
          return true;
          }
          if(digitalRead(DrumIn) != LOW || analogRead(PotWiper > 100) || currTime + ti < millis()){
          return false;
         }
        }
        break;
      case 3:
         while(potIn == false){
         if(analogRead(PotWiper) > 100){
          delay(500);
          return true;
         }
         if((analogRead(XJoy) > 515 || analogRead(XJoy) < 502) || (analogRead(YJoy) > 515 || analogRead(YJoy) < 490) || digitalRead(DrumIn) != LOW || currTime + ti < millis()){
          return false;
         }
         }
       break;
    }
  }
  return false;
}


int gameplay(){
  while(currentGame == 1){
    int inVar = randomVarForIn();
    inputSound(inVar);
    int timer = setTimer();
    correctInput = validateInput(inVar, timer);
    if(correctInput == 1){
    	currentScore++;
    } else{
    	currentGame = 0;
      return currentScore;
    }
  }
}

  void output(){
  	if(currentGame == 0 && currentScore > 0){ 
  		lcd.clear();
  		lcd.setCursor(0,0);
    	lcd.print(currentScore);
    	delay(5000);
      currentScore = 0;
    }
  }
