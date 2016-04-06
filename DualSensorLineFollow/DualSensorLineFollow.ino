#include <EVShield.h>
#include <EVs_NXTLight.h>

EVShield     GEVShield(0x34,0x36); 
EVs_NXTLight GLS1;
EVs_NXTLight GLS2;         


void setup() 
{
  
    Serial.begin(9600);
    Serial.println ("-------------------------------------");
    Serial.println ("Starting dual sensor line following");
    Serial.println ("-------------------------------------");
    GEVShield.init( SH_HardwareI2C );
    GLS1.init(&GEVShield, SH_BBS1);
    GLS2.init(&GEVShield, SH_BBS2);
    GLightSensor.setReflected();
    GEVShield.bank_b.motorReset();  

    Serial.println("Press Confirm To Continue");
    while (GEVShield.getButtonState(BTN_GO) != true)
    {
      delay(500);
    }
}

void loop() 
{
  int LLightValue1;
  int LLightValue2;
  // Raw Light Value 
  
  int LTreshhold1 = 643; 
  int LTreshhold2 = 643; 
  // Replace with auto calibration / control board switch
  
  int LMotorPowerOutside;
  int LMotorPowerInside;
  // Motor Turn Ratio

}
