#include <MCP23017.h>

const int Track = 7;
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

void setup() {
 pinMode(INT0, INPUT_PULLUP);
 pinMode(INT1, INPUT_PULLUP);
 pinMode(LED_BUILTIN, OUTPUT);
pinMode(Track, OUTPUT);

 init_mcp();
 init_interrupts();


}


void loop() {
 // show the program is still running by blinking the onboard LED
//  digitalWrite(LED_BUILTIN, LOW);
//  delay(500);
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(500);
//  on_int0_change();
//  on_int1_change();

DCC_send_command(DCCtrain1,speed15,1);
DCC_send_command(DCCtrain3,DCCinst_forward,1);
//  delay(500);
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

void on_int0_change() {
   // In your code, you might want to move this logic out of the interrupt routine
   // (for instance, by setting a flag and checking this flag in your main-loop)
   // This will prevent overhead.
   delayMicroseconds(2000);
   int sensor_data = mcp.readRegister(MCP23017Register::INTCAP_A);
   // The only thing we do with the interrupt signal is printing it
  //  Serial.println("int0:");
  //  Serial.print(sensor_data, BIN);
  if (sensor_data == 0xFB) {
      // Sensor data is 11111101
      Serial.println("Sensor data is 11111101");
      DCC_send_command(DCCtrain1,0x00,1);
      DCC_send_command(DCCtrain3,0x00,1);

    // digitalWrite(Track,LOW);
    delay(1000);
      // digitalWrite(Track, LOW); // Stop the train
      // delay(1000);
      // digitalWrite(Track, HIGH); // Resume the train
   }

}

void on_int1_change() {
   // In your code, you might want to move this logic out of the interrupt routine
   // (for instance, by setting a flag and checking this flag in your main-loop)
   // This will prevent overhead.
   delayMicroseconds(2000);
   int sensor_data = mcp.readRegister(MCP23017Register::INTCAP_B);
   // The only thing we do with the interrupt signal is printing it
  //  Serial.println("int1:");
  //  Serial.print(sensor_data, BIN);
  if (sensor_data == 0xEF) {
  // // Sensor data is 11111101
    Serial.println("Sensor data is blah");
    DCC_send_command(DCCtrain1,0x00,1);
      DCC_send_command(DCCtrain3,0x00,1);
    // digitalWrite(Track,LOW);
    delay(1000);  
    // digitalWrite(Track,HIGH);
    }   

  //  Serial.println();
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
          // int sensorStatusM0 = digitalRead(HallSensorM0);
          // int sensorStatusM1 = digitalRead(HallSensorM1);
          // int sensorStatusM2 = digitalRead(HallSensorM2);
                //  Serial.println(j);
                //  Serial.println("+++");
                //  Serial.println(sensorStatusM0);
                // Serial.println(sensorStatusM1);
                // Serial.println(sensorStatusM2);
             if((temp_command&0x8000000000000000)==0) { //test packet bit
            
              // Serial.println(sensorStatusM0);
              // Serial.println(sensorStatusM1);
              // Serial.println(sensorStatusM2);
                
               digitalWrite(Track, LOW);
               delayMicroseconds(100);
               digitalWrite(Track, HIGH);
               delayMicroseconds(100);
                
                //printf("0011");
            } else {
                

                digitalWrite(Track, LOW);
                delayMicroseconds(58);
                digitalWrite(Track, HIGH);
                delayMicroseconds(58);
               // Serial.print("01");

            }
            // next bit in packet
            temp_command = temp_command<<1;
        }
        i++;
    }
}