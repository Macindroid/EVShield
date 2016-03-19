#include <EVShieldAGS.h>
#include <EVs_NXTLight.h>

//hi
int

EVShield          evshield(0x34,0x36);
EVs_NXTLight GLightSensor; 
void Led_Flash(int c1, int c2, int c3)
{
  evshield.bank_a.ledSetRGB(c1,c2,c3);
  delay(500);
  evshield.bank_a.ledSetRGB(0,0,0);
  delay(50);
  evshield.bank_b.ledSetRGB(c1,c2,c3);
  delay(500);
  evshield.bank_b.ledSetRGB(0,0,0);
  delay(50);
}

void setup()
{
  Serial.begin(9600);
  
  evshield.init( SH_HardwareI2C );
  GLightSensor.init(&evshield, SH_BAS1);
  GLightSensor.setReflected(); 




   
}

void loop() 
{
  bool GO = false;
  
  int Black;
  int Green;
  int Silver;
  int Thres;
  
  while (GO == false)
  {
    Led_Flash(100,0,0);
    Serial.println("Press go on Black");
    if ( evshield.getButtonState(BTN_GO) == true ) 
    {
        
        Black = GLightSensor.readRaw();
    }    
    //delay(200);
    
    Serial.println("Press go on Green");
    if ( evshield.getButtonState(BTN_GO) == true ) 
    {
        evshield.bank_a.ledSetRGB(0,100,0);
        delay(500);
        Green = GLightSensor.readRaw();
    }    
    //delay(200);
    
    Serial.println("Press go on Silver");
    if ( evshield.getButtonState(BTN_GO) == true ) 
    {
        evshield.bank_a.ledSetRGB(100,100,100);
        delay(500);
        Silver = GLightSensor.readRaw();
    }    
    //delay(200);

    Serial.println("Calculating");
    
    Thres = (Black + Green) / 2;
    //delay(200);

  }
  evshield.bank_a.ledSetRGB(100,0,0);
    delay(500);
    evshield.bank_a.ledSetRGB(0,0,0);
    delay(50);
    evshield.bank_b.ledSetRGB(100,0,0);
    delay(500);
    evshield.bank_b.ledSetRGB(0,0,0);
    delay(50);

}
