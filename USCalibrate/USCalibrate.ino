#include <EVShield.h>
#include <Wire.h>
#include <EVs_EV3Infrared.h>

EVShield        GEVShield(0x34,0x36);
EVs_EV3Infrared GUS1;

void setup() {
  Serial.begin(9600);
  GEVShield.init( SH_HardwareI2C );
  GUS1.init( &GEVShield, SH_BAS2 );
  GUS1.setMode(MODE_Infrared_Proximity);

}

void loop() {
  int LProximity;

   LProximity = GUS1.readProximity();
   Serial.println(LProximity);
   delay(30);
   
}
