#include <EVShield.h>
#include <EVs_NXTLight.h>

EVShield     GEVShield(0x34,0x36); 
EVs_NXTLight GLS1;
EVs_NXTLight GLS2;   
      
const int CModePin = 2;
const int CConfirmPin = 4;
const int CLedPin1 = 7;
const int CLedPin2 = 8;
const int CLedPin3 = 12;

void waitForConfirm()
{
  while (digitalRead(CConfirmPin) != 1)
  {
    delay(50);
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
      delay(500);
    }
}

void loop() 
{
  int LLightValue1;
  int LLightValue2;
  // Raw Light Value 
  
  int LTreshold1; 
  int LTreshold2; 
  // Replace with auto calibration / control board switch
  
  int LMotorPowerOutside = 20;
  int LMotorPowerInside = -30;
  int LFwdPower = 20;
  // Motor Turn Ratio

  if (digitalRead(CModePin) != 1)
  {
    
    GEVShield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Float);
    GEVShield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Float);
    
    int LGreen1;
    int LGreen2;
    int LWhite1;
    int LWhite2;
    int LSilver1;
    int LSilver2;
    
    delay(500);
    digitalWrite(CLedPin1, HIGH); // Green
    
    waitForConfirm();
    LGreen1 = GLS1.readRaw();
    LGreen2 = GLS2.readRaw();
    digitalWrite(CLedPin1, LOW);
    delay(200);

    digitalWrite(CLedPin2, HIGH); // White
    waitForConfirm();
    LWhite1 = GLS1.readRaw();
    LWhite2 = GLS2.readRaw();
    digitalWrite(CLedPin2, LOW);
    delay(200);

    digitalWrite(CLedPin3, HIGH); // Silver
    waitForConfirm();
    LSilver1 = GLS1.readRaw();
    LSilver2 = GLS2.readRaw();
    digitalWrite(CLedPin3, LOW);
    delay(200);
 
    
    LTreshold1 = (LGreen1 + LWhite1) / 2;
    LTreshold2 = (LGreen2 + LWhite2) / 2;
    Serial.println(LTreshold1);
    Serial.println(LTreshold2);
    while(digitalRead(CModePin) != 1)
    {
      digitalWrite(CLedPin1, HIGH);
      digitalWrite(CLedPin2, LOW);
      digitalWrite(CLedPin3, LOW);
      delay(500);
      digitalWrite(CLedPin1, LOW);
      digitalWrite(CLedPin2, HIGH);
      digitalWrite(CLedPin3, LOW);
      delay(500);
      digitalWrite(CLedPin1, LOW);
      digitalWrite(CLedPin2, LOW);
      digitalWrite(CLedPin3, HIGH);
      delay(500);  
    }
    digitalWrite(CLedPin3, LOW);
    delay(200);
  }
  else
  { 
    digitalWrite(CLedPin2, HIGH);
    
    LLightValue1 = GLS1.readRaw();
    LLightValue2 = GLS2.readRaw();
    //Serial.println(LLightValue1);
    //Serial.println(LLightValue2);

    if ((LLightValue1 < LTreshold1) && (LLightValue2 < LTreshold2))      // White & White
    {
      GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LFwdPower);
      GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, LFwdPower); 
   
    }
    else if ((LLightValue1 < LTreshold1) && (LLightValue2 > LTreshold2)) // White & Black
    {
      GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LMotorPowerInside);
      GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, LMotorPowerOutside); 
 
    }
    else if ((LLightValue1 > LTreshold1) && (LLightValue2 < LTreshold2)) // Black & White
    {
      GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LMotorPowerOutside);
      GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, LMotorPowerInside); 
     
    }
    else if ((LLightValue1 > LTreshold1) && (LLightValue2 > LTreshold2)) // Black & Black (Green / Turn Hints)
    {
      Serial.println("Green");
      while (true) 
      {
        GEVShield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Float);
        GEVShield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Float);
      }
    }
    else //Silver
    {
      Serial.println("Silver");
    }
  }     
}
