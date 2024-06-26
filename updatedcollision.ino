#include <MCP23017.h>

//intcapA
int trainpos = 0;
String train3 = "d7";
String train1 = "d12";
int switch1 = 1;
int switch2 = 0;
int switch3 = 0;
int switch4 = 0;
int sens0 = 0;
int sens1 = 0;
int t1 = -1;
int t3 = -1;
const unsigned int sw1inst = 0x81;
const unsigned int sw2inst = 0x82;
const unsigned int sw3inst = 0x84;
const unsigned int sw4inst = 0x88;
const unsigned int idleinst = 0x80;
const unsigned int DCCaddressswitch = 0x06;
const unsigned int d22 = 0x7F;
const unsigned int d21 = 0xBF;
const unsigned int d13 = 0xDF;
const unsigned int d12 = 0xEF;
const unsigned int d11 = 0xF7;
const unsigned int d10 = 0xFB;
const unsigned int d9 = 0xFD;
const unsigned int d8 = 0xFE;

//intcapB
const unsigned int d7 = 0x7F;
const unsigned int d6 = 0xBF;
const unsigned int d5 = 0xDF;
const unsigned int d4 = 0xEF;
const unsigned int d3 = 0xF7;
const unsigned int d2 = 0xFB;
const unsigned int d1 = 0xFD;
const unsigned int d0 = 0xFE;
const unsigned int na = 0xFF;

const int Track = 7;
const int buzzerPin = 8;
#define INT0  2
#define INT1  3
MCP23017 mcp = MCP23017(0x26);
const unsigned int DCCtrain3 = 0x03;
const unsigned int DCCtrain1 = 0x01;
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


String sensorPositions[] = {"d21", "d2", "d22", "d1", "d0", "d13", "d12", "d10", "d8", "d7","d6", "d4"};
int numSensors = sizeof(sensorPositions) / sizeof(sensorPositions[0]);

String innerPos[] = {"d21","d2", "d22", "d1", "d0", "d13", "d12","d11","d5","d6","d7","d8","d9","d3"};
int innerSens = sizeof(innerPos)/sizeof(innerPos[0]);

void setup() {
 pinMode(INT0, INPUT_PULLUP);
 pinMode(INT1, INPUT_PULLUP);
 pinMode(LED_BUILTIN, OUTPUT);
 pinMode(Track, OUTPUT);
 init_mcp();
 init_interrupts();
 pinMode(buzzerPin, OUTPUT);

}


void loop() {

DCC_send_command(DCCtrain1,step9,1);
DCC_send_command(DCCtrain3,step7,1);


if (sens0 == 1){
  delayMicroseconds(2000);
  int sensor_data = mcp.readRegister(MCP23017Register::INTCAP_A);
  String pos= findMatchingVariable(sensor_data,'b');
  checkTrainPos(pos);
  sens0=0;
}
if (sens1 == 1){
  delayMicroseconds(2000);
  int sensor_data = mcp.readRegister(MCP23017Register::INTCAP_B);
  String pos= findMatchingVariable(sensor_data,'a');
  checkTrainPos(pos);
  sens1=0;
}

if (train3 == "d12" && switch1 == 0){
  DCC_send_command(DCCaddressswitch,sw1inst,1);
  DCC_send_command(DCCaddressswitch,sw1inst,1);
  Serial.println("Switch 1 should be in loop now");  
  switch1=1;
}
if(train3 == "d11" and switch1==1){
  DCC_send_command(DCCaddressswitch,sw1inst,1);
  DCC_send_command(DCCaddressswitch,idleinst,1);
  Serial.println("Switch 1 should be straight now");  
  switch1=0;
}
if(train3 == "d5" && switch3==0){
  DCC_send_command(DCCaddressswitch,sw3inst,1);
  DCC_send_command(DCCaddressswitch,idleinst,1);
  Serial.println("Switch 3 should be in loop now");  
  switch3=1;
}
if(train3 == "d6" && switch3==1){
  DCC_send_command(DCCaddressswitch,sw3inst,1);
  Serial.println("Switch 3 should be straight now");  
  switch3=0;
}
if(train3 == "d8" && switch4==0){
  DCC_send_command(DCCaddressswitch,sw4inst,1);
  DCC_send_command(DCCaddressswitch,idleinst,1);
  Serial.println("Switch 4 should be in loop now");  
  switch4=1;
}
if(train3 == "d9" && switch4==1){
  DCC_send_command(DCCaddressswitch,sw4inst,1);
  Serial.println("Switch 4 should be straight now");  
  switch4=0;
}
if(train3 == "d3" && switch2==0){
  DCC_send_command(DCCaddressswitch,sw2inst,1);
  Serial.println("Switch 2 should be in loop now");  
  switch2=1;
}
if(train3 == "d2" && switch2==1){
  DCC_send_command(DCCaddressswitch,sw2inst,1);
  DCC_send_command(DCCaddressswitch,idleinst,1);
  Serial.println("Switch 2 should be straight now");  
  switch2=0;
}
if (t1 != -1 && t3 != -1 && (abs(t1-t3) == 1 || abs(t1-t3) == numSensors - 1)){
  //Trains are adjacent to each other.
  Serial.println("Train3 at"+train3+"Train1 at "+train1+"Index train3:"+String(t3)+"Index train1:"+String(t1));
  Serial.print("Trains are next to each other");
  // Emergency stop logic along with buzzer maybe
}
if (train1 == "d4" && train3 == "d3"){
  Serial.print("collision S1");
  
}
if (train3 == "d3" && (train1 == "d21" || train1 == "d2" )){
  Serial.print("collision S2 or S3");
  //Stop train 3
}
if (train1 == "d6" && train3 == "d5"){
  Serial.print("Collision S4");
  //Stop train3
}
if (train1 == "d7" && train3 == "d5"){
  Serial.print("Collision S5");
  //Stop train3
}
if (train1 == "d8" && train3 == "d5"){
  Serial.print("Collision S6");
  //Stop train3
}
if (train1 == "d10" && train3 == "d8"){
  Serial.print("Collision S7");
  //Stop train1
}
if (train1 == "d10" && train3 == "d7"){
  Serial.print("Collision S8");
  //Stop train3
}

}

void checkTrainPos(String pos) {
  int train1Index = -1;
  int train3Index = -1;
  int posIndex = -1;


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
    train3 = pos;
    Serial.println("TRAIN 3 is at " + pos);
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
  if (train1Index>-1 && posIndex == (train1Index + 1) % numSensors) {
    train1 = pos;
    // Here, we will get the index of train1 in the outer array.    
    t1 = train1Index+1;
    Serial.println("TRAIN 1 is at " + pos);
  }
  if(pos == "d5" || pos == "d7" || pos == "d11" || pos == "d9"){
    t3=-1;
  }
}



void init_mcp() {
   Wire.begin();
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
    //printf("\n\r %llx \n\r",command);
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
