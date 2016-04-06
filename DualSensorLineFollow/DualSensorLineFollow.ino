#include <EVShield.h>
#include <EVs_NXTLight.h>

EVShield     GEVShield(0x34,0x36); 
EVs_NXTLight GLS1;
EVs_NXTLight GLS2;         

const int CConfirmPin = 4;

void setup() 
{
    pinMode(CConfirmPin, INPUT);
    
  
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
  
  int LTreshold1 = 643; 
  int LTreshold2 = 643; 
  // Replace with auto calibration / control board switch
  
  int LMotorPowerOutside;
  int LMotorPowerInside;
  // Motor Turn Ratio

  LLightValue1 = GLS1.readRaw();
  LLightValue2 = GLS2.readRaw();
  Serial.println(LLightValue1);
  Serial.println(LLightValue2);

  if (LLightValue1 < LTreshold1 && LLightValue2 < LTreshold2)      // White & White
  {
    GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, LMotorPowerOutside);
    GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, LMotorPowerOutside); 
  }
  else if (LLightValue1 < LTreshold1 && LLightValue2 > LTreshold2) // White & Black
  {
    GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LMotorPowerOutside);
    GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, LMotorPowerInside); 
  }
  else if (LLightValue1 > LTreshold1 && LLightValue2 < LTreshold2) // Black & White
  {
    GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LMotorPowerOutside);
    GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, LMotorPowerInside); 
  }
  else if (LLightValue1 > LTreshold1 && LLightValue2 > LTreshold2) // Black & Black (Green / Turn Hints)
  {
    Serial.println("Green");
  }
  else //Silver
  {
    Serial.println("Silver");
  }
  
}
