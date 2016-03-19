#include <EVShield.h>
#include <EVs_NXTLight.h>

EVShield     GEVShield(0x34,0x36); 
EVs_NXTLight GLightSensor;         

void setup()
{

    Serial.begin(9600);
    Serial.println ("-------------------------------------");
    Serial.println ("Starting single sensor line following");
    Serial.println ("-------------------------------------");

    GEVShield.init( SH_HardwareI2C );
    GLightSensor.init(&GEVShield, SH_BAS1);
    GLightSensor.setReflected();
    GEVShield.bank_b.motorReset();

    Serial.println ("Press GO button to continue");
    // Flash LEDs until the GO button is pressed
    while (GEVShield.getButtonState(BTN_GO) != true)
    {
      delay(500);
    }
}

void
loop()
{
    int LLightValue;
    int LThreshold = 643; //calibration value
    int LMotorPowerOutside = 20; //power level. 20 for new, 60 for old battery
     int LMotorPowerInside  =  1; //Uncomment for sharper turns. 

    LLightValue = GLightSensor.readRaw();
    Serial.println(LLightValue);
    if (LLightValue < LThreshold){
        GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, LMotorPowerOutside);
        //GEVShield.bank_b.motorStop(SH_Motor_2, SH_Next_Action_Brake);
         GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Forward, LMotorPowerInside); // Comment the line above, and uncomment this line for sharper turns
    } else {
        GEVShield.bank_b.motorRunUnlimited(SH_Motor_2, SH_Direction_Reverse, LMotorPowerOutside);
        //GEVShield.bank_b.motorStop(SH_Motor_1, SH_Next_Action_Brake);
         GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, LMotorPowerInside);  // Comment the line above, and uncomment this line for sharper turns
    }    
}
