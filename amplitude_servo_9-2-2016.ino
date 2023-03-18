//Doug Bopp, Last Updated September 2 2016
//This code is meant to interface with an
//AD9958 direct digital synthesizer which controls
//the amplitude and the frequency going into an
//RF amplfiier connected to an AOM. A reference
//clock of 10MHz is supplied and 80MHz is output.
//Feedback is supplied from a photodiode measuring the 
//pump laser power and this device can then act to servo 
//the power by modulating the amplitude. The DDS takes in
//32 bit frequency words and 10 bit amplitude words
//all accessed over SPI. The pro micro microcontroller
//has a 10 bit ADC but this has a 100microsec acquisition
//time so the servo can never exceed 10kHz. The SPI protocol
//should probably be done with a built in package instead 
//of my current hard-wired bit banging setup. Additional 
//testing of the effects of changing the amplitude quickly 
//needs to be measured to detect any spurious noise that may
//be introduced inadvertently. The servo loop speed also needs
//to be measured and the servo may need a "turn on" command
//to see if the power fluctuations are below a threshold
//which is reasonable so that it doesn't unncessarily servo
//the system.

const int ssPin=10;
const int SDIO=16;
const int CLK=15;
const int IOUpdate=14;
const int MasterReset=9;
const int MeasuredAmplitude=A0;
const int ServoON=7;
const int AvgON=8;
  unsigned int average=0;
  unsigned int amplitude=0;
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
  
  pinMode(MeasuredAmplitude,INPUT);
  pinMode(ServoON,INPUT);
  pinMode(AvgON,INPUT);
  digitalWrite(AvgON,HIGH);
  digitalWrite(ServoON,HIGH);

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
  //send bits 7..0 MSB first
  for(i=0;i<8;i++)
  {
    if (data & 0x80)
      {digitalWrite(SDIO,HIGH);}
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

void updateAmplitude(unsigned int amp){
  send_byte(0x06);
  send_byte(0x00);
  send_byte(((amp>>8)&0xff)|0x10);
  send_byte(amp&0xff);
  updateIO();
}

void loop() {
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
   send_byte(0x04);
   send_byte(eightymegahertz);
   send_byte(eightymegahertz);
   send_byte(eightymegahertz);
   send_byte(eightymegahertz);
   updateIO();
   updateAmplitude(921);      //1024*.9=921=0x399

  while(digitalRead(AvgON)==1){
    amplitude=921;
    updateAmplitude(amplitude);
    Serial.println(amplitude);
    Serial.println(analogRead(MeasuredAmplitude));
    average=analogRead(MeasuredAmplitude)/16+average-average/16;
    //computationally efficient moving average with 16 members
    Serial.println("the averaging is on!");
    delay(100);
  }

  while(digitalRead(ServoON)==1){
    //unsigned int setpoint=average;
    //signed int delta=setpoint-analogRead(MeasuredAmplitude);
    //amplitude=amplitude+delta;
    //updateAmplitude(amplitude);
    //trying to make this loop faster
    updateAmplitude(amplitude+average-analogRead(MeasuredAmplitude));
    delay(100);
    Serial.println("the servo is on!");
    Serial.println(amplitude);
    Serial.println(analogRead(MeasuredAmplitude));
  }
   delay(125);
}
