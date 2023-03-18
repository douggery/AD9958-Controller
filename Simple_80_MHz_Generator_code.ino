//Doug Bopp, Last Updated September 13 2016
//This code is meant to interface with an
//AD9958 direct digital synthesizer which controls
//the frequency going into an RF amplfiier connected to an AOM. 
//A reference clock of 10MHz is supplied and 80MHz is output.

const int ssPin=10;
const int SDIO=16;
const int CLK=15;
const int IOUpdate=14;
const int MasterReset=9;
byte eightymegahertz(B01100110);

void setup() {
  pinMode(ssPin,OUTPUT);
  pinMode(IOUpdate,OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(SDIO, OUTPUT);
  pinMode(MasterReset,OUTPUT);
  digitalWrite(ssPin,HIGH);
  digitalWrite(IOUpdate,LOW);
  digitalWrite(CLK,LOW);
  digitalWrite(SDIO,LOW);

  //start with a master reset
  digitalWrite(MasterReset,HIGH);
  delay(1);
  digitalWrite(MasterReset,LOW);
  delay(1000);

  Serial.begin(9600);
}

void send_byte(unsigned char data)
{
  int i;
  digitalWrite(ssPin,LOW);
  for(i=0;i<8;i++)
  {
    if (data & 0x80)
      {digitalWrite(SDIO,HIGH);}
  //send bits 7..0 MSB first
    else
      {digitalWrite(SDIO,LOW);}
    toggleHILO(CLK,1);
    data<<=1;
  }
  digitalWrite(ssPin,HIGH);
}

void toggleHILO(int pinName,int repetitions){
  int j;
  for(j=0;j<repetitions;j++){
    digitalWrite(pinName,HIGH);
    digitalWrite(pinName,LOW);}
}

void updateIO()
{
  digitalWrite(IOUpdate,HIGH);
  toggleHILO(CLK,4);
  digitalWrite(IOUpdate,LOW);
}

void loop() {
  Serial.begin(9600);
     //turn on channel 1 using special register 0x00
   send_byte(0x00);
   send_byte(0x80);
   //configure FR1 with multiplier equal to 20
   //20==00010100
   send_byte(0x01);
   send_byte(B01010000);
   send_byte(B00000000);
   send_byte(B00000000);
   updateIO();
   Serial.println("---this is when it should update---");
   send_byte(0x04);
   send_byte(eightymegahertz);
   send_byte(eightymegahertz);
   send_byte(eightymegahertz);
   send_byte(eightymegahertz);
   updateIO();
   
   delay(5000);
}
