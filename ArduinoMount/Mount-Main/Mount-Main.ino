#include <Servo.h>

#include <Servo.h>
#include <SoftwareSerial.h>
#include <AFMotor.h>

//Stepper
#define STEPS_PER_REV 4096
#define STEPPER_PORT 2
#define STEPPER_SPEED 20 //rpm
#define MANUAL_STEPS 5

//Servo
#define SERVO_LEFT_PIN 10
#define SERVO_RIGHT_PIN 9
#define SERVO_STEP 1

//HC-05
#define BT_RX 2
#define BT_TX_UNUSED 0

//Bounds
#define ALT_LOW_BOUND 11
#define ALT_HIGH_BOUND 90

//Enums
enum ReceiveMode {MANUAL, COORD};
enum CoordType {ALT, AZ};

//Hardware components delcarations
AF_Stepper stepper(STEPS_PER_REV/2, STEPPER_PORT) ; 
Servo leftServo, rightServo;
SoftwareSerial BTSerial(BT_RX, BT_TX_UNUSED); 


class DirectionClass {
  private:
    int alt;
    int az;

  public:

    DirectionClass() {
      //Initialise values
      alt = ALT_LOW_BOUND;
      az = 0;

      //Move to initial positions
      moveToAlt(ALT_LOW_BOUND);
      moveToAz(0);
    }

    void moveLeftServo(int pAlt) {
      //for C577 servo
      //left range: 90 -> 0
      leftServo.write(map(pAlt, 0, 90, 90, 0));
    }

    void moveRightServo(int pAlt) {
      //for C577 servo
      //right range: 90 -> 180
      rightServo.write(map(pAlt, 0, 90, 90, 180));
    }

    //Steppers
    void moveToAlt(int pAlt) {
      int leftAngle, rightAngle;
      const int increment = 10;

      if (pAlt >= ALT_LOW_BOUND && pAlt <= ALT_HIGH_BOUND) {                      
        
        //Move each servo in steps to keep the two in sync
        if (pAlt > alt) {
                    
          for(int i = increment; (alt + i) <= pAlt; i += increment) {                         
            moveLeftServo(alt+i);
            moveRightServo(alt+i);
            delay(100);
          }
        } else if (pAlt < alt) {          
          for(int i = increment; alt - i >= pAlt; i += increment) {
            moveLeftServo(alt-i);
            moveRightServo(alt-i);
            delay(100);
          } 
        }

        //Move the remainder
        moveLeftServo(pAlt);
        moveRightServo(pAlt);
        
        alt = pAlt; //update current alt
      }
    }

    void manualAltIncrease(void) {
      moveToAlt(alt+SERVO_STEP);
    }

    void manualAltDecrease(void) {
      moveToAlt(alt-SERVO_STEP);
    }

    void moveToAz(int pAz) {      
      
      int direction;
      int degToMove = pAz-az;
      int stepsToMove;

      if (degToMove < 0) {
        direction = BACKWARD;
      } else {
        direction = FORWARD;
      }

      stepsToMove = map(abs(degToMove), 0, 360, 0, STEPS_PER_REV);

      stepper.step(stepsToMove, direction, INTERLEAVE);
      //Serial.println(degToMove);

      az = pAz;
      
    }
    
    manualAzIncrease(void) {
      az += MANUAL_STEPS;
      stepper.step(MANUAL_STEPS, FORWARD, INTERLEAVE);  
    }

    manualAzDecrease(void) {
      az -= MANUAL_STEPS;
      stepper.step(MANUAL_STEPS, BACKWARD, INTERLEAVE);  
    }
};

//Global Vars
DirectionClass direction;
String altStr;
String azStr;
enum ReceiveMode receiveMode;
enum CoordType coordType;

//Function Headers
int handleManualChar(char input);
int handleCoordChar(char input);

void setup()
{
  //Serial Setup
  Serial.begin(115200);

  //BT Setup
  BTSerial.begin(38400);

  //Stepper Setup
  stepper.setSpeed(STEPPER_SPEED);

  //Servo Setup
  leftServo.attach(SERVO_LEFT_PIN);
  rightServo.attach(SERVO_RIGHT_PIN);
  
  //Flags
  receiveMode = MANUAL;
  coordType = ALT;
}

void loop()
{     
  if(BTSerial.available()) {
    char input = BTSerial.read();
    //Serial.println(input);
    switch (receiveMode) {
      
      case MANUAL:
        handleManualChar(input);
        break;
      
      case COORD:
        handleCoordChar(input);
        break;      
    }
  }
}

int handleManualChar(char input) {
  switch (input) {    
    case 'r':  //Move Right
      direction.manualAzIncrease();
      break;
    case 'l':  //Move Left
      direction.manualAzDecrease();
      break;                
    case 'u':  //Move Up
      direction.manualAltIncrease();
      break;
    case 'd':  //Move Down
      direction.manualAltDecrease();
      break;
    case '(':  //Enter Coord Mode
      receiveMode = COORD;
      altStr = "";
      azStr = "";
      coordType = ALT;
      break;
  }
}

int handleCoordChar(char input) {
  switch (input) {
    case ')':  //Switch to manual mode              
      direction.moveToAlt(altStr.toInt());
      direction.moveToAz(azStr.toInt());
      altStr = "";
      azStr = "";
      receiveMode = MANUAL;
      break;
    case ',':  //Start taking for az value
      coordType = AZ;
      break;
    default:
      switch(coordType) {
        case ALT:
          altStr += input;
          break;
        case AZ: 
          azStr += input;
          break;
      }
  }
}


