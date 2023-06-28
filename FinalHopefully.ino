#include <MCP23017.h>

#define INT0  2
#define INT1  3

MCP23017 mcp = MCP23017(0x26);
MCP23017 mcpsignal = MCP23017(0x27);

const int Track = 7;
const int buzzerPin = 8;

// Initial switch positions
int switch1 = 1;
int switch2 = 0;
int switch3 = 0;
int switch4 = 0;

// Sensor triggers
int sens0 = 0;
int sens1 = 0;

// Train index in the array
int t1 = -1;
int t3 = -1;

// Train motion value
int StopT1 = 0;
int StopT3 = 0;

// Station value for each train
int stationtrain3_1 = 0;
int stationtrain3_2 = 0;
int stationtrain1_1 = 0;
int stationtrain1_2 = 0;

// Switch related instructions and addresses
const unsigned int sw1inst = 0x81;
const unsigned int sw2inst = 0x82;
const unsigned int sw3inst = 0x84;
const unsigned int sw4inst = 0x88;
const unsigned int idleinst = 0x80;
const unsigned int DCCaddressswitch = 0x06;

//IntCap A sensors
const unsigned int d22 = 0x7F;
const unsigned int d21 = 0xBF;
const unsigned int d13 = 0xDF;
const unsigned int d12 = 0xEF;
const unsigned int d11 = 0xF7;
const unsigned int d10 = 0xFB;
const unsigned int d9 = 0xFD;
const unsigned int d8 = 0xFE;

//intcap B Sensors
const unsigned int d7 = 0x7F;
const unsigned int d6 = 0xBF;
const unsigned int d5 = 0xDF;
const unsigned int d4 = 0xEF;
const unsigned int d3 = 0xF7;
const unsigned int d2 = 0xFB;
const unsigned int d1 = 0xFD;
const unsigned int d0 = 0xFE;
const unsigned int na = 0xFF;

// Initial train positions
String train3 = "d11";
String train1 = "d8";
String train1prev = "d10";
String train3prev = "d12";

// Emergency stop
int emergency = 0;

// Microcontroller Switch
const int PinButton = 9;
// Train address
const unsigned int DCCtrain3 = 0x03;
const unsigned int DCCtrain1 = 0x01;

// Train speeds
const unsigned int DCCinst_forward = 0x68;
const unsigned int step1 = 0x62;
const unsigned int step2 = 0x72;
const unsigned int step3 = 0x63;
const unsigned int step4 = 0x73;
const unsigned int step5 = 0x64;
const unsigned int step6 = 0x74;
const unsigned int step7 = 0x65;
const unsigned int step8 = 0x75;
const unsigned int step9 = 0x66;
const unsigned int step10 = 0x76;
const unsigned int step11 = 0x67;
const unsigned int step12 = 0x77;
const unsigned int step13 = 0x68;
const unsigned int step14 = 0x78;
const unsigned int step15 = 0x69;
const unsigned int step16 = 0x79;
const unsigned int step17 = 0x6A;
const unsigned int step18 = 0x7A;
const unsigned int step19 = 0x6B;
const unsigned int step20 = 0x7B;
const unsigned int step21 = 0x6C;
const unsigned int step22 = 0x7C;
const unsigned int step23 = 0x6D;
const unsigned int step24 = 0x7D;
const unsigned int step25 = 0x6E;
const unsigned int step26 = 0x7E;
const unsigned int step27 = 0x6F;
const unsigned int step28 = 0x7F;

// Outer loop array
String sensorPositions[] = {"d21", "d2", "d22", "d1", "d0", "d13", "d12", "d10", "d8", "d7","d6", "d4"};
int numSensors = sizeof(sensorPositions) / sizeof(sensorPositions[0]);

// Inner loop array
String innerPos[] = {"d21","d2", "d22", "d1", "d0", "d13", "d12","d11","d5","d6","d7","d8","d9","d3"};
int innerSens = sizeof(innerPos)/sizeof(innerPos[0]);

int red = 0;
int collision = 0;

void setup() {
 Wire.begin();
 pinMode(INT0, INPUT_PULLUP);
 pinMode(INT1, INPUT_PULLUP);
 pinMode(LED_BUILTIN, OUTPUT);
 pinMode(Track, OUTPUT);
 init_mcp();
 init_interrupts();
//  pinMode(buzzerPin, OUTPUT);
 pinMode(PinButton, INPUT);
 mcpsignal.init();
 mcpsignal.writeRegister(MCP23017Register::IODIR_A, (unsigned char )0x00); // set IO direction to output
 mcpsignal.writeRegister(MCP23017Register::IODIR_B, (unsigned char )0x00);
 mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);
 mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);
}

void loop() {
  int stateButton = digitalRead(PinButton); 

  // if (newbutton = 1 && flag = 0){
  //   train3 = "d13";
  //   train3prev = "d12";
  //   train1prev = "d10";
  //   train1 = "d8";
  //   flag = 1;
  // }
  if(emergency || stateButton){
    DCC_send_command(DCCtrain1,0x61,1);
    DCC_send_command(DCCtrain3,0x61,1);
    // tone(buzzerPin, 31);
  }
  if ( emergency == 0){
    // tone(buzzerPin, LOW);
    if(StopT1 == 0 ){
      DCC_send_command(DCCtrain1,step10,1);
    }
    else if (StopT1 == 1){
      DCC_send_command(DCCtrain1,0x61,1);
    }
    if(StopT3 == 0 ){
      DCC_send_command(DCCtrain3,step8,1);
    }
    else if(StopT3 == 1  ){
      DCC_send_command(DCCtrain3,0x00,1);
    }
  }
  

  // This will check which sensor got triggered and assign it to train1 or train3 accordingly
  if (sens0 == 1){
    delayMicroseconds(2000);
    int sensor_data = mcp.readRegister(MCP23017Register::INTCAP_A);
    // Serial.print(sensor_data);
    String pos= findMatchingVariable(sensor_data,'b');
    // Serial.print(pos);
    checkTrainPos(pos);
    sens0=0;
  }
  if (sens1 == 1){
    delayMicroseconds(2000);
    int sensor_data = mcp.readRegister(MCP23017Register::INTCAP_B);
    // Serial.print(sensor_data);
    String pos= findMatchingVariable(sensor_data,'a');
    // Serial.print(pos);
    checkTrainPos(pos);
    sens1=0;
  }
  
  // Switch switch 1 when train 3 is at d12.
  if (train3 == "d12" && switch1 == 0){
    DCC_send_command(DCCaddressswitch,sw1inst,1);
    DCC_send_command(DCCaddressswitch,sw1inst,1);
    Serial.println("Switch 1 should be in loop now");       
    switch1=1;
  }
  // Switch SWT1 back when train 3 enters inside and crosses d11.
  if(train3 == "d11" and switch1==1){
    DCC_send_command(DCCaddressswitch,sw1inst,1);
    DCC_send_command(DCCaddressswitch,idleinst,1);
    Serial.println("Switch 1 should be straight now");  
    switch1=0;
  }

  // Switch SW3 when train 3 is at d5
  if(train3 == "d5" && switch3==0){
    DCC_send_command(DCCaddressswitch,sw3inst,1);
    DCC_send_command(DCCaddressswitch,idleinst,1);
    Serial.println("Switch 3 should be in loop now");  
    switch3=1;
  }

  // Switch SW3 back when train 3 is at d6
  if(train3 == "d6" && switch3==1){
    DCC_send_command(DCCaddressswitch,sw3inst,1);
    Serial.println("Switch 3 should be straight now");  
    switch3=0;
  }

  //Switch SW4 when train3 is at d8
  if(train3 == "d8" && switch4==0){
    DCC_send_command(DCCaddressswitch,sw4inst,1);
    DCC_send_command(DCCaddressswitch,idleinst,1);
    Serial.println("Switch 4 should be in loop now");  
    switch4=1;
  }
  // Switch SwT4 when train3 is at d9
  if(train3 == "d9" && switch4==1){
    DCC_send_command(DCCaddressswitch,sw4inst,1);
    Serial.println("Switch 4 should be straight now");  
    switch4=0;
  }

  // Switch SWT2 when train3 is at d3
  if(train3 == "d3" && switch2==0){
    DCC_send_command(DCCaddressswitch,sw2inst,1);
    Serial.println("Switch 2 should be in loop now");  
    switch2=1;
  }
  //Swich SWT2 back when train3 is at d2
  if(train3 == "d2" && switch2==1){
    DCC_send_command(DCCaddressswitch,sw2inst,1);
    DCC_send_command(DCCaddressswitch,idleinst,1);
    Serial.println("Switch 2 should be straight now");  
    switch2=0;
  }

  // Stopping train  1 at station 1
  if(stationtrain1_1 == 0)
  {
    if(train1 == "d21"){
      DCC_send_command(DCCtrain1,step8,1);
      Serial.println("Train 1 is slowing down");
      // tone(buzzerPin, 31);

    }
    if(train1 == "d2"){
      Serial.println("Train 1 is at station 1");
      tone(buzzerPin, LOW);
      StopT1 = 2;
      DCC_send_command(DCCtrain1,0x61,100);
      stationtrain1_1 = 1;
      StopT1 = 0;
    }
    
  }
  if (train1 == "d21")
  {
    stationtrain1_1 = 0;
  }

  if(stationtrain3_2 == 0)
  {
    if (train3 == "d13"){
      DCC_send_command(DCCtrain3,step4,1);
      Serial.println("Train 3 is slowing down");
      // tone(buzzerPin, 31);
    }
    if (train3 == "d12"){
      StopT3 = 2;
      Serial.println("Train 3 is at station 2");
      DCC_send_command(DCCtrain3,0x00,100);
      tone(buzzerPin, LOW);
      stationtrain3_2 = 1;
      StopT3 = 0;
    }
  }
  if(train3 == "d13")
  {
    stationtrain3_2 = 0;
  }

  if (t1 != -1 && t3 != -1 && (abs(t1-t3) == 1 || abs(t1-t3) == numSensors - 1)){
    // Serial.println("Train3 at"+train3+"Train1 at "+train1+"Index train3:"+String(t3)+"Index train1:"+String(t1));
    collision = 1;
    emergency = 1;
  }

  if (train1 == "d4" && (train3 == "d3" || train3 == "d9")){
  // Serial.print("collision S1");
  // tone(buzzerPin, 31);
  collision = 1;
  StopT1 = 1;
  }
  if(train1 == "d4"&& train3 == "d22"){
    // Serial.print("Collision S1 avoided");
    collision = 0;
    tone(buzzerPin, LOW);
    StopT1 = 0;
  }
  if (train3 == "d3" && (train1 == "d21" || train1 == "d2" )){
    // Serial.print("collision S2 or S3");
    // tone(buzzerPin, 31);
    collision = 1;
    StopT3 = 1;
  }
  if (train3 == "d3" && train1 == "d22"){
    // Serial.print("COllision S2 and S3 avoided");
    collision = 0;
    tone(buzzerPin, LOW);
    StopT3 = 0;
  }
  if ( train3 == "d11" && (train1 == "d6"  || train1 == "d7" || train1 == "d8" )){
    // Serial.print("Collision S4,S5 or S6");
    // tone(buzzerPin, 31);
    collision = 1;
    StopT3 = 1;
  }
  if (train1 == "d4" && train3 == "d11"){
    // Serial.print("Collision S4, S6 and S5 avoided");
    tone(buzzerPin, LOW);
    collision = 0;
    StopT3 = 0;
  }
  if ((train1 == "d10" || train1 == "d12") && (train3 == "d7" || train3 == "d8")){
    // Serial.print("Collision S8 or S7");
    // tone(buzzerPin, 31);
    collision = 1;
    StopT1 = 1;
  }
  if ((train1 == "d10" || train1 == "d12") && train3=="d9"){
    // Serial.print("Collision S7 or S8 avoided");
    collision = 0;
    tone(buzzerPin, LOW);
    StopT1 = 0;
  }
  if (train3 != "d22" || train3 != "d1"){
    setSignalLightsGreen();
  }
  if ( train3 == "d22"){
    setSignalLight8();
    red = 1;
  }
  if (train3 == "d1"&& red == 1){
    setSignalLight8();
    DCC_send_command(DCCtrain3,0x61,100);
    red = 0;
  }
  
}

void checkTrainPos(String pos) {
  int train1Index = -1;
  int train3Index = -1;
  int posIndex = -1;
  int obstacle = 1;
  

  // Here you check if the triggered sensor is in the inner loop array.
  for(int i=0;i<innerSens;i++){
    if(innerPos[i] == pos){
      posIndex = i;
    }
    if (innerPos[i] == train3){
      train3Index = i;
    }
  }
  // Check if the triggered position is adjacent to train 3
  if (train3Index>-1 && posIndex == (train3Index + 1) % innerSens) {
    obstacle = 0;
    train3prev = train3;
    train3 = pos;
    // Serial.println("TRAIN 3 is at " + pos);
  }

  // Here you check if the triggered sensor is in the outer loop array.
  for (int i=0;i<numSensors;i++){
    if(sensorPositions[i] == pos){
      posIndex = i;
    }
    if (sensorPositions[i] == train1){
      train1Index = i;
    }
    // Here, we will get the index of train3 in the outer array. Doing this only because we need train 3 index in the outerloop array.
    if (sensorPositions[i] == train3){
      t3 = i;
    }
  }

  // Check if the triggered position is adjacent to train 1.
  if (train1Index>-1 && posIndex == (train1Index + 1) % numSensors && StopT1 == 0) {
    train1prev = train1;
    train1 = pos;
    obstacle = 0;
    // Here, we will get the index of train1 in the outer array.    
    t1 = train1Index+1;
    // Serial.println("TRAIN 1 is at " + pos);
  }

  if (obstacle && train1!=pos && train3!=pos && train1prev != pos && train3prev!= pos){
    Serial.println("OSTACLE DETECTED AT: "+pos);
    emergency = 1;
  }
  if(pos == "d5" || pos == "d7" || pos == "d11" || pos == "d9"){
    t3=-1;
  }
  Serial.println("|| Train 1 is at "+train1+"|| Train 3 is at "+train3+"|| ");
  if(collision){
    Serial.println("COLLISION DETECTED");
  }
}

void init_mcp() {
   
   mcp.init();
   // Initialisation of MCP registers, documentation on registers is available at Niels/Abel/Robert/Natalia
   mcp.writeRegister(MCP23017Register::IODIR_A, (unsigned char )0xff);
   mcp.writeRegister(MCP23017Register::IODIR_B, (unsigned char )0xff);
   mcp.writeRegister(MCP23017Register::IPOL_A, (unsigned char )0x00);
   mcp.writeRegister(MCP23017Register::IPOL_B, (unsigned char )0x00);
   mcp.writeRegister(MCP23017Register::DEFVAL_A, (unsigned char )0xff);
   mcp.writeRegister(MCP23017Register::DEFVAL_B, (unsigned char )0xff);
   mcp.writeRegister(MCP23017Register::INTCON_A, (unsigned char )0xff);
   mcp.writeRegister(MCP23017Register::INTCON_B, (unsigned char )0xff);
   mcp.writeRegister(MCP23017Register::IOCON, (unsigned char )0x2);
   mcp.writeRegister(MCP23017Register::GPPU_A, (unsigned char )0xff);
   mcp.writeRegister(MCP23017Register::GPPU_B, (unsigned char )0xff);
}

void init_interrupts() {
   // Clear current interrupts
   mcp.readRegister(MCP23017Register::GPIO_A);
   mcp.readRegister(MCP23017Register::GPIO_B);
   // Register callbacks
   attachInterrupt(digitalPinToInterrupt(INT0), on_int0_change, FALLING);
   attachInterrupt(digitalPinToInterrupt(INT1), on_int1_change, FALLING);
   // Enable interrupts on the MCP
   mcp.writeRegister(MCP23017Register::GPINTEN_A, (unsigned char )0xff);
   mcp.writeRegister(MCP23017Register::GPINTEN_B, (unsigned char )0xff);
   
}

String findMatchingVariable(unsigned int hexValue, char option) {
  if (option == 'a') {
    if (hexValue == d22) {
      return "d22";
    } else if (hexValue == d21) {
      return "d21";
    } else if (hexValue == d13) {
      return "d13";
    } else if (hexValue == d12) {
      return "d12";
    } else if (hexValue == d11) {
      return "d11";
    } else if (hexValue == d10) {
      return "d10";
    } else if (hexValue == d9) {
      return "d9";
    } else if (hexValue == d8) {
      return "d8";
    }
  } else if (option == 'b') {
    if (hexValue == d7) {
      return "d7";
    } else if (hexValue == d6) {
      return "d6";
    } else if (hexValue == d5) {
      return "d5";
    } else if (hexValue == d4) {
      return "d4";
    } else if (hexValue == d3) {
      return "d3";
    } else if (hexValue == d2) {
      return "d2";
    } else if (hexValue == d1) {
      return "d1";
    } else if (hexValue == d0) {
      return "d0";
    }
  }
  return "";  // Return an empty string if no match is found
}

void on_int0_change() {
  // Serial.print("One of the sensors in d0 to d7 got triggered");
  sens0 = 1;
}

void on_int1_change() {
  // Serial.print("One of the sensors in d22 to d2 got triggered");
  sens1 = 1; 
}

void DCC_send_command(unsigned int address, unsigned int inst, unsigned int repeat_count)
{
    uint64_t command = 0x0000000000000000; // __int64 is the 64-bit integer type
    uint64_t temp_command = 0x0000000000000000;
    uint64_t prefix = 0x3FFF; // 14 "1" bits needed at start
    int error = 0x00; //error byte
    //calculate error detection byte with xor
    error = address ^ inst;
    //combine packet bits in basic DCC format
    command = (prefix<<28)|(address<<19)|(inst<<10)|((error)<<1)|0x01;
    int i=0;
    //repeat DCC command lots of times
    while(i < repeat_count) {
        temp_command = command;
        //loops through packet bits encoding and sending out digital pulses for a DCC command
        for (int j=0; j<64; j++) {
             if((temp_command&0x8000000000000000)==0) { //test packet bit           
               digitalWrite(Track, LOW);
               delayMicroseconds(100);
               digitalWrite(Track, HIGH);
               delayMicroseconds(100);
            } else {
                digitalWrite(Track, LOW);
                delayMicroseconds(58);
                digitalWrite(Track, HIGH);
                delayMicroseconds(58);
            }
            // next bit in packet
            temp_command = temp_command<<1;
        }
        i++;
    }
}

void setSignalLight8()
{
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111110);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111011);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11101111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b10111111);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111110);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111011);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11101111);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);//red
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b01111111);
}

void setSignalLightsGreen() {

    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111110);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111011);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11101111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b10111111);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111110);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11111011);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b11101111);//green
    mcpsignal.writeRegister(MCP23017Register::OLAT_A, (unsigned char )0b11111111);
    mcpsignal.writeRegister(MCP23017Register::OLAT_B, (unsigned char )0b10111111);//green
}