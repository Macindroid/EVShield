#include <EVShield.h>
#include <EVs_NXTLight.h>
#include <EEPROM.h>

EVShield		 GEVShield(0x34,0x36);
EVs_NXTLight GLS1;
EVs_NXTLight GLS2;
EVs_NXTLight GLSMiddle;

const int CModePin		= 2;
const int CConfirmPin = 4;
const int CLedPin1		= 7;
const int CLedPin2		= 8;
const int CLedPin3		= 12;

int GThreshold1;
int GThreshold2;



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

	GLSMiddle.init(&GEVShield, );									//add port for middle light sensor
	GLSMiddle.setReflected();

	LOriginalReading = GLSMiddle.readRaw();
  GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 20)
  GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, 5)

	while (LOriginalReading - GLSMiddle.readRaw()  < 5);
	{
		delay(50);
	}
  GEVShield.bank_b.motorStop(SH_Motor_1);
  GEVShield.bank_b.motorStop(SH_Motor_2);
  

}

void LHLineFollow()
{
	int LCount;
	int LLightValue1;

	while(LCount > 10)
	{
		LLightValue1 = GLS2.readRaw();
		if (LLightValue1 < GThreshold1)
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 20);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, 5);
		}
		else
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, 20);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 5);
		}
		LCount = LCount + 1;
	}
}

void RHLineFollow()
{
	int LCount;
	int LLightValue2;

	while(LCount > 10)
	{
		LLightValue2 = GLS2.readRaw();
		if (LLightValue2 < GThreshold2)
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, 20);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 5);
		}
		else
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 20);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, 5);
		}
		LCount = LCount + 1;
	}
}


/******HelpfulFunctions******/


void waitForConfirm()
{
	while (digitalRead(CConfirmPin) != 1)
	{
		delay(20);
	}
}










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

		GEVShield.init( SH_HardwareI2C );
		GLS1.init(&GEVShield, SH_BBS1);
		GLS2.init(&GEVShield, SH_BBS2);
		GLS1.setReflected();
		GLS2.setReflected();
		GEVShield.bank_b.motorReset();


		Serial.println("Press Confirm To Continue");
		while (digitalRead(CConfirmPin) != 1)
		{
			delay(50);
		}
}


/******MainProgram******/


void loop()
{
	int LLightValue1;
	int LLightValue2;
	// Raw Light Value

	int LMotorPowerOutside = 10;
	int LMotorPowerInside = -5;
	int LFwdPower = 10;
	// Motor Turn Ratio

	if (digitalRead(CModePin) != 1)
	{


		/******CALIBRATION******/

		int LGreen1;
		int LGreen2;
		int LWhite1;
		int LWhite2;
		int LSilver1;
		int LSilver2;

		int LSave1;
		int LSave2;

		delay(1000);

		digitalWrite(CLedPin1, HIGH); // Green
		waitForConfirm();

		LGreen1 = GLS1.readRaw();
		LGreen2 = GLS2.readRaw();
		Serial.println(LGreen2);
		digitalWrite(CLedPin1, LOW);
		delay(200);

		digitalWrite(CLedPin2, HIGH); // White
		waitForConfirm();

		LWhite1 = GLS1.readRaw();
		LWhite2 = GLS2.readRaw();
		Serial.println(LWhite2);
		digitalWrite(CLedPin2, LOW);
		delay(200);

		digitalWrite(CLedPin3, HIGH); // Silver
		waitForConfirm();

		LSilver1 = GLS1.readRaw();
		LSilver2 = GLS2.readRaw();
		digitalWrite(CLedPin3, LOW);
		delay(200);


		LSave1 = (LGreen1 + LWhite1) / 2;
		LSave2 = (LGreen2 + LWhite2) / 2;
		saveToFile(0, 1, LSave1);
		saveToFile(2, 3, LSave2);

		Serial.println(LSave1);
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
	}




	else
	{

	 GThreshold1 = readFromFile(0, 1);
	 GThreshold2 = readFromFile(2, 3);
		/******LineFollow******/

		LLightValue1 = GLS1.readRaw();
		LLightValue2 = GLS2.readRaw();

		if ((LLightValue1 < GThreshold1) && (LLightValue2 < GThreshold2))			// White & White
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LFwdPower);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, LFwdPower);
		}
		else if ((LLightValue1 < GThreshold1) && (LLightValue2 > GThreshold2)) // White & Black
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LMotorPowerInside);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, LMotorPowerOutside);
		}
		else if ((LLightValue1 > GThreshold1) && (LLightValue2 < GThreshold2)) // Black & White
		{
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LMotorPowerOutside);
			GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, LMotorPowerInside);

		}
		else if ((LLightValue1 > GThreshold1) && (LLightValue2 > GThreshold2)) // Black & Black (Green / Turn Hints)
		{
			digitalWrite(CLedPin1, HIGH);
			Serial.println("Green");
			Serial.println("Light Sensor 1:");
			Serial.println(LLightValue1);
			Serial.println(GThreshold1);
			Serial.println("Light Sensor 2:");
			Serial.println(LLightValue2);
			Serial.println(GThreshold2);

			while (true)
			{
				GEVShield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Float);
				GEVShield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Float);
			}
		}
		else //Silver
		{
			Serial.println("Silver");
			Serial.println(LLightValue1);
			Serial.println(GThreshold1);
		}
	}
}
