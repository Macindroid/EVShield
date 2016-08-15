#include <EVShield.h>
#include <Wire.h>
#include <EVs_NXTLight.h>
#include <EVs_EV3Infrared.h>
#include <EVs_NXTServo.h>
#include <EEPROM.h>

EVShield		    GEVShield(0x34,0x36);
EVs_EV3Infrared GUS1;
EVs_NXTLight    GLS1;
EVs_NXTLight    GLS2;
EVs_NXTLight    GLSMiddle;

const int CLedPin1		= 2;
const int CLedPin2		= 4;
const int CLedPin3		= 7;
const int CConfirmPin = 8;
const int CModePin    = 12;

const int CPower = 10;//Water Tower
const int CLineFollowPowerForward = 10; //Line Follow
const int CLineFollowPowerOutside = 10; //Line Follow
const int CLineFollowPowerInside  = -10;//Line Follow

int GThresholdRight;
int GThresholdLeft;
int GSilverRight;
int GSilverLeft;

bool Run = true;

struct LSArray{
  int Right;
  int Left;
};

enum greyScale{
  gsBlack,
  gsWhite,
  gsSilver
};
/***** Motor Contols *****/

/* Tacho Movements */
unsigned long getAverageEncoder(){
  unsigned long LReturn;
  LReturn = 
    (abs(GEVShield.bank_a.motorGetEncoderPosition(SH_Motor_1)) +
     abs(GEVShield.bank_b.motorGetEncoderPosition(SH_Motor_1))) / 2;
  return LReturn;
}

void rotateMotors(int APowerLeft, int APowerRight, int ARotations){
  rotateMotorsDeg(APowerLeft, APowerRight, ARotations * 360);
}

void rotateMotorsDeg(int APowerLeft, int APowerRight, int ADegrees){
  int LDegrees = abs(ADegrees);
  motorsOn(APowerLeft, APowerRight);
  while (getAverageEncoder() <= LDegrees){
    delay(50);
  }
  motorsOff();  
}

/* Basic Movements */

void motorLeftOn(int APower){
  GEVShield.bank_b.motorResetEncoder(SH_Motor_1);
  GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, APower);
}

void motorRightOn(int APower){
  GEVShield.bank_a.motorResetEncoder(SH_Motor_1);
  GEVShield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, APower);
}

void motorsOn(int APowerLeft, int APowerRight){
  motorLeftOn(APowerLeft);
  motorRightOn(APowerRight);
}

void motorsOff(){
  GEVShield.bank_a.motorStop(SH_Motor_1, SH_Next_Action_Brake );
  GEVShield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Brake );
}

/***** Light Sensor Algorithm *****/

/* Declarations */


void setLSValues(LSArray &ALSRaw){
  ALSRaw.Right = GLS1.readRaw();
  ALSRaw.Left = GLS2.readRaw();
}

/* Brains */

int LSValueToGreyScale(int ALSValue, int ABlackWhiteTH, int ASilverTH){
  Serial.println("");
  Serial.println("LS Value:");
  Serial.println(ALSValue);
  Serial.println("");
  Serial.println("Black & White TH:");
  Serial.println(ABlackWhiteTH);
  Serial.println("");
  Serial.println("White & Silver TH:");
  Serial.println(ASilverTH);
  if (ALSValue >= ABlackWhiteTH && ALSValue >= ASilverTH)
    return gsBlack;
  else if (ALSValue <= ABlackWhiteTH && ALSValue >= ASilverTH)
    return gsWhite;
  else //if (ALSValue <= ABlackWhiteTH && ALSValue <= ASilverTH)
    return gsSilver;
}

int LSValuesToGreyScale(LSArray ALSValues, LSArray &AGreyScale){
  AGreyScale.Right = LSValueToGreyScale(ALSValues.Right, GThresholdRight, GSilverRight);
  AGreyScale.Left = LSValueToGreyScale(ALSValues.Left, GThresholdLeft, GSilverLeft);
}

void printLSValues(LSArray ALDRValues){
  Serial.print(ALDRValues.Right); Serial.print(", "); 
  Serial.print(ALDRValues.Left); Serial.println(""); 
}

/***** Main Line Follow Loop *****/

void doLineFollow()
{
  int LProximity;
  LProximity = GUS1.readProximity();
  
  LSArray LLSValues;
  LSArray LGreyScales;

  setLSValues(LLSValues);
  LSValuesToGreyScale(LLSValues, LGreyScales);

  //printLSValues(LGreyScales);

  if ((LGreyScales.Right == gsWhite) and (LGreyScales.Left == gsWhite)){          // White & White
    motorsOn(CLineFollowPowerForward, CLineFollowPowerForward);                   // Forward
  } 
  else if ((LGreyScales.Right == gsBlack) and (LGreyScales.Left == gsWhite)){     // Black & White
    motorsOn(CLineFollowPowerOutside, CLineFollowPowerInside);                    // Right
  } 
  else if ((LGreyScales.Right == gsWhite) and (LGreyScales.Left == gsBlack)){     // White & Black
    motorsOn(CLineFollowPowerInside, CLineFollowPowerOutside);                    // Left
  } 
  else if ((LGreyScales.Right == gsBlack) and (LGreyScales.Left == gsBlack)){     // Black & Black
    turnHints();                                                                  // Turn Hints
  }
  else if ((LGreyScales.Right == gsSilver) and (LGreyScales.Left == gsSilver)){   // Silver & Silver
    //Serial.println("Silver");
    //Serial.println(LGreyScales.Right);
    //Serial.println(LGreyScales.Left);
    silverCan();                                                                  // Can Tile
  }
  else if (LProximity < 200){                                                     // Ultrasonic Reading < 80
    motorsOff();                                                                  // Water Tower
    waterTower();                                                                 
  }
}

/***** Individual Tiles *****/

/* Water Tower */
void waterTower(){
  int LLSValue1 = 1;
  
  rotateMotorsDeg(CPower, -CPower, 160);
  delay(50);
  rotateMotorsDeg(CPower, CPower, 380);
  delay(50);
  rotateMotorsDeg(-CPower, CPower, 160);
  delay(50);
  rotateMotorsDeg(CPower, CPower, 480);
  delay(50);
  motorsOn(CPower, CPower / 0.26);
  while (LLSValue1 < GThresholdRight){
    LLSValue1 = GLS1.readRaw();
    delay(30);
  }
  motorsOff();
  rotateMotorsDeg(CPower, CPower, 60);
  delay(50);
  rotateMotorsDeg(CPower, -CPower, 160);
  motorsOff();
}

/* Final Tile (Can Tile) */ 
void silverCan(){ 
  int LProximity; 
  //int LLSValue1 = 2550;
  
  GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, 10);
  delay(300);
  GEVShield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Brake );
  rotateMotorsDeg(CPower, CPower - 2, 680);
  motorsOn(CPower,-CPower);
  delay(50);
  while (LProximity > 250){
    LProximity = GUS1.readProximity();
    delay(30);
  }
  motorsOff();
  delay(500);
  if (LProximity > 250){
    rotateMotorsDeg(-CPower, CPower, 40);
  } 
  delay(50);
  rotateMotorsDeg(CPower, CPower, 270);
  delay(50);
  GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, 10);
  delay(300);
  GEVShield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Brake );
  delay(50);
  motorsOn(CPower, CPower - 2);
  //while (LLSValue1 > GThresholdRight);{
    //LLSValue1 = GLS1.readRaw();
    //delay(50);
  //}
  delay(1100);
  motorsOff();
  GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, 10);
  delay(300);
  GEVShield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Brake );
  delay(5000);
}




/******DataSaving/Reading******/

void saveToFile(int AMemAdrs1, int AMemAdrs2, int AValue)
{
	int LHigh = 0;
	int LLow	= 0;
	int LValue = 0;
	int LValue2 = 0;

	LHigh = ((AValue >> 8) & 0xff);
	LLow	= AValue & 0xff;

	EEPROM.write(AMemAdrs1, LHigh);
	EEPROM.write(AMemAdrs2, LLow);

	LValue = EEPROM.read(AMemAdrs1);
	LValue2 = EEPROM.read(AMemAdrs2);


}

int readFromFile(int AMemAdrs1, int AMemAdrs2)
{
	int LAfter = 0;
	int LHigh	= 0;
	int LLow	 = 0;

	LHigh = EEPROM.read(AMemAdrs1);
	LLow = EEPROM.read(AMemAdrs2);

	LAfter = (((LHigh & 0xff) << 8) | (LLow & 0xff));

	return LAfter;
}


/******TurnHints******/


void turnHints()
{
	int LOriginalReading;

	LOriginalReading = GLSMiddle.readRaw();
  GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 5);
  GEVShield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 5);  

	while ((abs(GLSMiddle.readRaw() - LOriginalReading)) < 5);
	{
    //Serial.println((abs(GLSMiddle.readRaw() - LOriginalReading)));
		delay(500);
	}
  GEVShield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Float);
  GEVShield.bank_a.motorStop(SH_Motor_1, SH_Next_Action_Float);
  delay(1000);
  if (LOriginalReading > GLSMiddle.readRaw())
  {
    Serial.println("Right");
    RHLineFollow();
  }
  else if(LOriginalReading < GLSMiddle.readRaw())
  {
    Serial.println("Left");
    LHLineFollow();
  }
  
}

void LHLineFollow()
{
	int LLightValue2;
  unsigned long LTimer = (3000 + millis());

	while(LTimer > millis())
	{
		LLightValue2 = GLS2.readRaw();
		if (LLightValue2 > GThresholdLeft)
		{
			GEVShield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 15);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 5);
		}
		else
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 15);
			GEVShield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 5);
		}
	}
 Serial.println("LH Linefollow completed");
}

void RHLineFollow()
{
	int LLightLeft;
  unsigned long LTimer = (3000 + millis());

	while(LTimer > millis())
	{
    Serial.println();
		LLightLeft = GLS1.readRaw();
		if (LLightLeft > GThresholdRight)
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 10);
			GEVShield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 5);
		}
		else
		{
			GEVShield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 10);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 5);
		}
	}
 Serial.println("RH Linefollow completed");
}


/******HelpfulFunctions******/


void waitForConfirm()
{
	while (digitalRead(CConfirmPin) != 1)
	{
		delay(20);
	}
}
/*
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    else if (inputString = 's')
    {
      Run = false;
      stringComplete = true;
    }
    else if (inputString = 'f') 
    {
      unsigned long LTimer = (5000 + millis());
      while (LTimer > millis())
      {
        GEVShield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 10);
        GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 10);
      }
        GEVShield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Brake);
        GEVShield.bank_a.motorStop(SH_Motor_1, SH_Next_Action_Brake);
      }
      stringComplete = true;
  }
}
*/


void setup()
{
		pinMode(CConfirmPin, INPUT);
		pinMode(CModePin, INPUT);
		pinMode(CLedPin1, OUTPUT);
		pinMode(CLedPin2, OUTPUT);
		pinMode(CLedPin3, OUTPUT);


		Serial.begin(9600);
		Serial.println ("-------------------------------------");
		Serial.println ("Starting dual sensor line following");
		Serial.println ("-------------------------------------");
 

		GEVShield.init(SH_HardwareI2C);
		GLS1.init(&GEVShield, SH_BAS1);
		GLS2.init(&GEVShield, SH_BBS1);
    GLSMiddle.init(&GEVShield, SH_BBS2);
		GLS1.setReflected();
		GLS2.setReflected();
    GLSMiddle.setReflected();

    GUS1.init( &GEVShield, SH_BAS2 );
    GUS1.setMode(MODE_Infrared_Proximity);
    
		GEVShield.bank_b.motorReset();
    GEVShield.bank_a.motorReset();
    Serial.println("Press Confirm To Continue");
    waitForConfirm();
    Serial.println("");
}


/******MainProgram******/


void loop()
{ 
  GThresholdRight = readFromFile(0, 1);
  GThresholdLeft = readFromFile(2, 3);
  GSilverRight = readFromFile(4, 5);
  GSilverLeft = readFromFile(6, 7);

	if (digitalRead(CModePin) != 1)
	{
    GEVShield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Float);
    GEVShield.bank_a.motorStop(SH_Motor_1, SH_Next_Action_Float); 
    Serial.println("Calibration Sequence Commencing");

		/******CALIBRATION******/

		int LGreen1;
		int LGreen2;
		int LWhite1;
		int LWhite2;
		int LSilver1;
		int LSilver2;

		int LSave1;
		int LSave2;
    int LSave3;
    int LSave4;
   
		delay(500);

		digitalWrite(CLedPin1, HIGH); // Green
		waitForConfirm();

		LGreen1 = GLS1.readRaw();
		LGreen2 = GLS2.readRaw();
    Serial.println("Green 1: ");
    Serial.println(LGreen1);
    Serial.println("");
    Serial.println("Green 2: ");
		Serial.println(LGreen2);
    Serial.println("");
		digitalWrite(CLedPin1, LOW);
		delay(200);



		digitalWrite(CLedPin2, HIGH); // White
		waitForConfirm();

		LWhite1 = GLS1.readRaw();
		LWhite2 = GLS2.readRaw();
    Serial.println("White 1: ");
    Serial.println(LWhite1);
    Serial.println("");
    Serial.println("White 2: ");
		Serial.println(LWhite2);
    Serial.println("");
		digitalWrite(CLedPin2, LOW);
		delay(200);

		digitalWrite(CLedPin3, HIGH); // Silver
		waitForConfirm();

		LSilver1 = GLS1.readRaw();
		LSilver2 = GLS2.readRaw();
    Serial.println("Silver 1: ");
    Serial.println(LSilver1);
    Serial.println("");
    Serial.println("Silver 2: ");
    Serial.println(LSilver2);
    Serial.println("");
		digitalWrite(CLedPin3, LOW);
		delay(200);


		LSave1 = (LGreen1 + LWhite1) / 2;
		LSave2 = (LGreen2 + LWhite2) / 2;
    LSave3 = (LSilver1 + LWhite1) / 2;
    LSave4 = (LSilver2 + LWhite2) / 2;
		saveToFile(0, 1, LSave1);
		saveToFile(2, 3, LSave2);
    saveToFile(4, 5, LSave3);
    saveToFile(6, 7, LSave4);

    Serial.println("Current EEPROM Save");
    Serial.println("Light Sensor 1: ");
		Serial.println(LSave1);
    Serial.println("Light Sensor 2: ");
		Serial.println(LSave2);
    
		while(digitalRead(CModePin) != 1)
		{
			digitalWrite(CLedPin1, HIGH);
			digitalWrite(CLedPin2, LOW);
			digitalWrite(CLedPin3, LOW);
			delay(100);
			digitalWrite(CLedPin1, LOW);
			digitalWrite(CLedPin2, HIGH);
			digitalWrite(CLedPin3, LOW);
			delay(100);
			digitalWrite(CLedPin1, LOW);
			digitalWrite(CLedPin2, LOW);
			digitalWrite(CLedPin3, HIGH);
			delay(100);
		}
		digitalWrite(CLedPin1, HIGH);
		digitalWrite(CLedPin2, HIGH);
		digitalWrite(CLedPin3, HIGH);
		delay(200);
    Serial.println("Line Follow Commencing");
	}




	else
	{
    doLineFollow();
	}
}
