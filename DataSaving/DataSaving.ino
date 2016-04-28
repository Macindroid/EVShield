void setup() {

  int LBefore = 1023;
  int LHigh   = 0;
  int LLow    = 0;
  int LAfter  = 0;
  
  Serial.begin(9600);
  Serial.println("Started...");

  // Run the test on numbers from 0 to 1023
  for (LBefore = 0; LBefore < 1024; LBefore++){
    // Convert the number LBefore into two x 8 byte integers (LLow and LHigh)
    LHigh = ((LBefore >> 8) & 0xff);
    LLow  = LBefore & 0xff;
    
    // Recombine the 8 byte integers (LLow and LHight) into LAfter
    LAfter = (((LHigh & 0xff) << 8) | (LLow & 0xff));
  
    // Display the values LBefore, LHigh, LLow and LAfter
    Serial.print(LBefore); 
    Serial.print(", "); 
    Serial.print(LHigh); 
    Serial.print(", "); 
    Serial.print(LLow);
    Serial.print(", "); 
    Serial.println(LAfter);
  }
  Serial.println("Done...");
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
