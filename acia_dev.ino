int E    =  1; // Enable            (pin 14)
int RS   = 0; // Register Select   (pin 11)
int IRQ  = 11; // Interrupt Request (pin  7)
int RW   = 2; // Read/Write        (pin 13)
//int CS2  = 13; // Chip Select 2     (pin  9)
int DIO[] = {10,9,8,7,6,5,4,3};
int i = 0;
int endloop = 0;
char msg[] = "I'm a Terminal: \n";
//int msg1 = 65;
//int msg[] ={73,32,108,111,118,101,32,109,121,32,112,114,105,110,99,101,115,115,32,82,111,115,101,109,97,114,121,32,60,51};

int incomingByte = 0; // for incoming serial data

void enablePulse() {
  digitalWrite(E, HIGH); delayMicroseconds(100); digitalWrite(E, LOW);delayMicroseconds(50);
}

void writeTDR(byte txByte) {
  int i;
  digitalWrite(RS, HIGH); digitalWrite(RW, LOW);
  for (i=0; i < 8; i++) {pinMode(DIO[i], OUTPUT);};
  for (i=0; i < 8; i++) {digitalWrite(DIO[i], bitRead(txByte,i));};
  enablePulse();
}

void writeMessage(char msg[]) {
   int i = 0;
   while (msg[i] != '\0') {
    if (msg[i] == '\n') {
      writeTDR(0xA);delay(1);writeTDR(0xD);
    } else {
      writeTDR(msg[i]);
    };
    i = i + 1;
  }
}
byte readRDR() {
  int i;
  byte rxByte;
  digitalWrite(RS, HIGH); digitalWrite(RW, HIGH);
  enablePulse();
  for (i=0; i < 8; i++) {pinMode(DIO[i], INPUT);};
  for (i=0; i < 8; i++) {bitWrite(rxByte, i, digitalRead(DIO[i]));};
  return rxByte;
}

void setup() {
  Serial.begin(115200); delay(1000);
  // initialize digital
  //pinMode(CS2, OUTPUT); digitalWrite(CS2,HIGH);
  pinMode(E,   OUTPUT); digitalWrite(E, LOW);
  pinMode(RS,  OUTPUT); pinMode(IRQ, INPUT); pinMode(RW,  OUTPUT);
  //
  digitalWrite(RW, HIGH);
  // master reset
  digitalWrite(RS, LOW);
  for (i=0; i < 8; i++) {pinMode(DIO[i], OUTPUT);};
  digitalWrite(DIO[0], HIGH); digitalWrite(DIO[1], HIGH);
  digitalWrite(DIO[2], HIGH);  digitalWrite(DIO[3], LOW);  digitalWrite(DIO[4], HIGH); // WordSelect: 8 bits + 1 Stop     
  digitalWrite(DIO[5], LOW);  digitalWrite(DIO[6], LOW);  digitalWrite(DIO[8], HIGH);   
  digitalWrite(RW, LOW);
  enablePulse();
  // set divide by 16
  digitalWrite(RS, LOW); digitalWrite(RW, LOW);
  digitalWrite(DIO[0], LOW); digitalWrite(DIO[1], HIGH);
  // IRQ enabled
  digitalWrite(DIO[7], HIGH);
  enablePulse();
  writeMessage(msg);
  

}


void loop() {
    //Recive data
     if (digitalRead(IRQ) == LOW) {
    Serial.write(readRDR());
  };
  //Transmit data
    if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    writeTDR(incomingByte);
    }
 
}
