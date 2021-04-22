int Lo1 = 39;
int Lo2 = 40;

void setup() {
  Serial.begin(9600);
  pinMode(Lo1, INPUT); // Setup LO+
  pinMode(Lo2, INPUT); // Setup LO-
}

void loop() {
  
  if((digitalRead(Lo1) == 1)||(digitalRead(Lo2) == 1)){ // This shows that the ECG leads are Disconnected
    Serial.println("Leads are disconnected!");
  }
  else{
      Serial.println(analogRead(A14)); // reads in data from pin 14 and plots the data on the serial monitor.
      delay(1); // Thi is done for stability in reading in the values
}
