#include <EVShield.h>

EVShield     GEVShield(0x34,0x36);          

void setup()
{
    GEVShield.init( SH_HardwareI2C );
    GEVShield.bank_b.motorReset();
    GEVShield.bank_a.motorReset();
    Serial.begin(9600);
}
void loop(){
  GEVShield.bank_b.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 100);
  GEVShield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 100); // Comment the line above, and uncomment this line for sharper turns
  Serial.println("Test");
}
