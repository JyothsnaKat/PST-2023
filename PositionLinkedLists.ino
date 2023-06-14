#include <MCP23017.h>
// #include "Adafruit_LiquidCrystal.h"

// Connect via i2c, default address #0 (A0-A2 not jumpered)
// Adafruit_LiquidCrystal lcd(0);
//intcapA
int trainpos = 0;
String train1 = "d12";
String train3 = "d4";
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

struct Node {
    String position;
    Node* next;
};

Node* head = nullptr;
Node* tail = nullptr;


void setup() {
 pinMode(INT0, INPUT_PULLUP);
 pinMode(INT1, INPUT_PULLUP);
 pinMode(LED_BUILTIN, OUTPUT);
 pinMode(Track, OUTPUT);
 init_mcp();
 init_interrupts();
 pinMode(buzzerPin, OUTPUT);
  String sensorIDs[] = {"d21", "d2", "d22", "d1", "d0", "d13", "d12", "d10", "d8", "d7", "d4"};
  int numSensors = sizeof(sensorIDs) / sizeof(sensorIDs[0]);

  for (int i = 0; i < numSensors; i++) {
      Node* newNode = new Node;
      newNode->position = sensorIDs[i];
      newNode->next = nullptr;

      if (head == nullptr) {
          head = newNode;
          tail = newNode;
      } else {
          tail->next = newNode;
          tail = newNode;
      }
  }
  tail->next = head;

}


void loop() {
 // show the program is still running by blinking the onboard LED
//  digitalWrite(LED_BUILTIN, LOW);
//  delay(500);
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(500);
//  on_int0_change();
//  on_int1_change();
// trainpos = 1;
DCC_send_command(DCCtrain1,step11,1);
//lcd.setCursor(0, 1);
// trainpos = 3;

DCC_send_command(DCCtrain3,step13,1);

//  delay(500);
}

// int calculatePositionDifference(int pos1, int pos2) {
//     struct Node {
//         int position;
//         Node* next;
//     };

//     int diff = 0;

//     Node* head = nullptr;
//     Node* tail = nullptr;

//     // Initialize the linked list with the values
//     int positions[] = {21, 2, 22, 1, 0, 13, 12, 10, 8, 7, 6, 4};
//     int numPositions = sizeof(positions) / sizeof(positions[0]);

//     for (int i = 0; i < numPositions; i++) {
//         Node* newNode = new Node;
//         newNode->position = positions[i];
//         newNode->next = nullptr;

//         if (head == nullptr) {
//             head = newNode;
//             tail = newNode;
//         } else {
//             tail->next = newNode;
//             tail = newNode;
//         }
//     }

//     Node* currentNode = head;
//     bool foundPos1 = false;
//     bool foundPos2 = false;

//     while (currentNode != nullptr) {
//         if (currentNode->position == pos1) {
//             foundPos1 = true;
//         }

//         if (currentNode->position == pos2) {
//             foundPos2 = true;
//         }

//         if (foundPos1 && foundPos2) {
//             break;
//         }

//         currentNode = currentNode->next;
//         diff++;
//     }

//     if (foundPos1 && foundPos2) {
//         return abs(diff);
//     } else {
//         return -1;
//     }
// }


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
  delayMicroseconds(2000);
  int sensor_data = mcp.readRegister(MCP23017Register::INTCAP_A);
  
  Node* currentNode = head;
  Node* foundNode = nullptr;
  String pos= findMatchingVariable(sensor_data,'b');
  // Serial.print("INTA:"+pos+"\n");

  while (currentNode != nullptr) {
    if (currentNode->position == train1 && currentNode->next->position == pos) {
      train1 = pos;
      Serial.print("TRAIN 1 in INT0 is changed to " + pos + "\n");
      break;
    }
    if (currentNode->position == train3 && currentNode->next->position == pos) {
      train3 = pos;
      Serial.print("TRAIN 3 in INT0 is changed to " + pos + "\n");
      break;
    }
    currentNode = currentNode->next;
  }

  // if (foundNode != nullptr && foundNode->next != nullptr) {
  //   if (train1 == foundNode->position) {
  //     train1 = foundNode->next->position;
  //     // Print the train and sensor information
  //     Serial.println("Train 1 detected by " + String(sensor_data));
  //     Serial.println("Train 1 position updated to " + String(foundNode->next->position));
  //   } else if (train3 == foundNode->position) {
  //     train3 = foundNode->next->position;
  //     // Print the train and sensor information
  //     Serial.println("Train 3 detected by " + String(sensor_data));
  //     Serial.println("Train 3 position updated to " + String(foundNode->next->position));
  //   }
  // } else {
  //   Serial.println("DOnt know why we are here");
  // }

  // Clean up the linked list
  // currentNode = head;
  // while (currentNode != nullptr) {
  //   Node* temp = currentNode;
  //   currentNode = currentNode->next;
  //   delete temp;
  // }

  // unsigned int sensorIDs[] = {d21, d2, d22, d1, d0, d13, d12, d10, d8, d7, d6, d4};
  // int numSensors = sizeof(sensorIDs) / sizeof(sensorIDs[0]);

  // String train;
  // if (trainpos == 1) {
  //   train = "Train 1";
  // } else {
  //   train = "Train 3";
  // }

  // if (sensor_data == d7) {
  //   if (trainpos == 1) {
  //     train1 = 7;
  //   } else {
  //     train3 = 7;
  //   }
  //   // Serial.println(train + " detected by D7");
  // } else if (sensor_data == d6) {
  //   if (trainpos == 1) {
  //     train1 = 6;
  //   } else {
  //     train3 = 6;
  //   }
  //   // Serial.println(train + " detected by D6");
  // } else if (sensor_data == d5) {
  //   if (trainpos == 1) {
  //     train1 = 5;
  //   } else {
  //     train3 = 5;
  //   }
  //   // Serial.println(train + " detected by D5");
  // } else if (sensor_data == d4) {
  //   if (trainpos == 1) {
  //     train1 = 4;
  //   } else {
  //     train3 = 4;
  //   }
  //   // Serial.println(train + " detected by D4");
  // } else if (sensor_data == d3) {
  //   if (trainpos == 1) {
  //     train1 = 3;
  //   } else {
  //     train3 = 3;
  //   }
  //   // Serial.println(train + " detected by D3");
  // } else if (sensor_data == d2) {
  //   if (trainpos == 1) {
  //     train1 = 2;
  //   } else {
  //     train3 = 2;
  //   }
  //   // Serial.println(train + " detected by D2");
  // } else if (sensor_data == d1) {
  //   if (trainpos == 1) {
  //     train1 = 1;
  //   } else {
  //     train3 = 1;
  //   }
  //   // Serial.println(train + " detected by D1");
  // } else if (sensor_data == d0) {
  //   if (trainpos == 1) {
  //     train1 = 0;
  //   } else {
  //     train3 = 0;
  //   }
  //   // Serial.println(train + " detected by D0");
  // } else {
  //   // Handle the case when none of the sensors are triggered
  // }
  // Serial.println("TRAIN 3: D" + String(train3));
  // Serial.println("TRAIN 1: D" + String(train1));
}


void on_int1_change() {
  delayMicroseconds(2000);
  int sensor_data = mcp.readRegister(MCP23017Register::INTCAP_B);

  Node* currentNode = head;
  Node* foundNode = nullptr;
  String pos= findMatchingVariable(sensor_data,'a');
  Serial.print("INTB:"+pos+ "\n");
  // Node* one = nullptr;
  while (currentNode != nullptr) {
    if (currentNode->position == train1 && currentNode->next->position == pos) {
      train1 = pos;
      Serial.print("TRAIN 1 in INT1 is changed to " + pos + "\n");
      break;
    }
    if (currentNode->position == train3 && currentNode->next->position == pos) {
      train3 = pos;
      Serial.print("TRAIN 3 in INT1 is changed to " + pos + "\n");
      break;
    }
    currentNode = currentNode->next;
  }

  // if (foundNode != nullptr && foundNode->next != nullptr) {
  //   if (train1 == foundNode->position) {
  //     train1 = foundNode->next->position;
  //     // Print the train and sensor information
  //     Serial.println("Train 1 detected by int1:" + String(sensor_data));
  //     Serial.println("Train 1 position updated to int1:" + String(train1));
  //   } else if (train3 == foundNode->position) {
  //     train3 = foundNode->next->position;
  //     // Print the train and sensor information
  //     Serial.println("Train 3 detected by int1:" + String(sensor_data));
  //     Serial.println("Train 3 position updated to int1:" + String(train3));
  //   }
  // } else {
  //   Serial.println("DOnt know why we are here int1");
  // }
  // if (trainpos == 1) {
  //   train = "Train 1";
  // } else {
  //   train = "Train 3";
  // }

  // if (sensor_data == d22)
  // {
  //   if (trainpos == 1) {
  //     train1 = 22;
  //   } else {
  //     train3 = 22;
  //   }
  //   // Serial.println(train + " detected by D22");
  // }
  // else if (sensor_data == d21)
  // {
  //   if (trainpos == 1) {
  //     train1 = 21;
  //   } else {
  //     train3 = 21;
  //   }
  //   // Serial.println(train + " detected by D21");
  // }
  // else if (sensor_data == d13)
  // {
  //   if (trainpos == 1) {
  //     train1 = 13;
  //   } else {
  //     train3 = 13;
  //   }
  //   // Serial.println(train + " detected by D13");
  // }
  // else if (sensor_data == d12)
  // {
  //   if (trainpos == 1) {
  //     train1 = 12;
  //   } else {
  //     train3 = 12;
  //   }
  //   // Serial.println(train + " detected by D12");
  // }
  // else if (sensor_data == d11)
  // {
  //   if (trainpos == 1) {
  //     train1 = 11;
  //   } else {
  //     train3 = 11;
  //   }
  //   // Serial.println(train + " detected by D11");
  // }
  // else if (sensor_data == d10)
  // {
  //   if (trainpos == 1) {
  //     train1 = 10;
  //   } else {
  //     train3 = 10;
  //   }
  //   // Serial.println(train + " detected by D10");
  // }
  // else if (sensor_data == d9)
  // {
  //   if (trainpos == 1) {
  //     train1 = 9;
  //   } else {
  //     train3 = 9;
  //   }
  //   // Serial.println(train + " detected by D9");
  // }
  // else if (sensor_data == d8)
  // {
  //   if (trainpos == 1) {
  //     train1 = 8;
  //   } else {
  //     train3 = 8;
  //     // Serial.println(String(train3)+"HELLLOOOOO");
  //   }
  //   // Serial.println(train + " detected by D8");
  // }
  // else
  // {
  //   // Handle the case when none of the sensors are triggered
  // }
  
  // // Print the current positions of train1 and train3
  // // Serial.println("TRAIN 1: D" + String(train1));
  // Serial.println("TRAIN 3: D" + String(train3));
  // Serial.println("TRAIN 1: D" + String(train1));
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
