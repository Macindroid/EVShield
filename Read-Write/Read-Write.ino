/*
 The Arduino EEPROM access is very basic. You can read or write one byte 
 at the time, by specifying the address of the byte. Remember, a byte is a 
 number between 0..255 so anything you write will have to be squished down
 to fit.

 The analog ports on the Arduino will return a four byte number between 
 0..1023, or a four byte number. If you want to save this to EEPROM, you 
 have two options:

 Divide the four byte number by four so it fits in the range 0..255; or
 Save it as four bytes (I will have to think about the math to do this).

*/

#include <EEPROM.h>

void setup() {
  const int CAdrs1 = 0;
  int LValue = 0;
 
  // Initialize the serial monitor
  Serial.begin(9600);

  //Serial.println("About to write to EEPROM");
  //EEPROM.write(CAdrs1, 123);
  //Serial.println("The value 123 has been written to EEPROM byte 0");
  
  LValue = EEPROM.read(CAdrs1);
  Serial.print("The value "); 
  Serial.print(LValue); 
  Serial.println(" has been read from EEPROM byte 0");
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
