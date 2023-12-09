// NanoEverySerialScope

/*
 * TXD0 USB-PB0-9  Serial
 * RXD0 USB-PB1-10 Serial 
 * TXD1 PC4-1      Serial1
 * RXD1 PC5-0      Serial1 >
 * TXD2 PA0-2      Serial2
 * RXD2 PA1-7      Serial2 <
 * TXD3 PF4-6/A13  Serial3
 * RXD3 PF5-3/A14  Serial3
 * 
 * Fix serial ports as described here:
 * https://forum.arduino.cc/t/arduino-nano-every-access-to-4-serial/614358/3
 */
 
#define SERIALBUFSIZE         50
char serialBuffer[SERIALBUFSIZE];
byte setBufPointer = 0;
#define LED 13

#define USBBAUD 1000000
#define MONBAUD    9600
#define MONCFG     "8N1"
#define MON1    "1>"
#define MON2    "2>"

#define PORT1 1
#define PORT2 2
#define PORT3 3

#define MON1INPUT 2
#define MON2INPUT 10

uint16_t monitorBAUD = MONBAUD;
String   monCFGstr = MONCFG;

uint8_t data;
uint8_t portActive = 0;

#define TIMEOUT 100L
uint32_t milliCount = 0L;

void setup() {
  uint8_t monConfig = getConfig(monCFGstr[0], monCFGstr[1], monCFGstr[2]);
  Serial.begin(USBBAUD);
  
  Serial1.begin(MONBAUD, monConfig);
  Serial2.begin(MONBAUD, monConfig);
  while(!Serial);
  Serial.println("#Serial scope + v0.1.1");
  Serial.print("#Monitoring Serial1 as '");
  Serial.print(MON1);
  Serial.println("'");
  Serial.print("#Monitoring Serial2 as '");
  Serial.print(MON2);
  Serial.println("'");
  Serial.print("Ports config: ");
  Serial.print(MONBAUD);
  Serial.print(" BAUD, ");
  Serial.print(monCFGstr);
  Serial.print(" (");
  Serial.print(monConfig, HEX);
  Serial.println(")");
}

void loop() {
  // monitor mode
  if (Serial1.available()) {
    data = Serial1.read();
    if (portActive != PORT1) {
      Serial.println();
      Serial.print("1>");
      portActive = PORT1;
      milliCount = millis();
    }
    if (data < 0x10) Serial.print("0");
    Serial.print(data, HEX);
//    Serial.print(" ");
  }
  if (Serial2.available()) {
    data = Serial2.read();
    if (portActive != PORT2) {
      Serial.println();
      Serial.print("2>");
      portActive = PORT2;
      milliCount = millis();
    }
    if (data < 0x10) Serial.print("0");
    Serial.print(data, HEX);
//    Serial.print(" ");
  }
  if (portActive && milliCount + TIMEOUT < millis()) {
    Serial.println();
    portActive = 0;
  }
  commandCollector();
}

void commandCollector() {
  if (Serial.available() > 0) {
    int inByte = Serial.read();
    switch(inByte) {
    case '.':
//    case '\r':
    case '\n':
      commandInterpreter();
      clearSerialBuffer();
      setBufPointer = 0;
      break;
    case '\r':
      break;  // ignore carriage return
    default:
      serialBuffer[setBufPointer] = inByte;
      setBufPointer++;
      if (setBufPointer >= SERIALBUFSIZE) {
        Serial.println("Serial buffer overflow. Cleanup.");
        clearSerialBuffer();
        setBufPointer = 0;
      }
    }
  }
}

void commandInterpreter() {
  byte bufByte = serialBuffer[0];
  
  switch(bufByte) {
    case 'B':
    case 'b':
      setBaud();
      break;
    case 'C':
    case 'c':
      setConfig();
      break;
    case 'H':
    case 'h':
    case '?':
      usage();
      break;
    case 'L':
    case 'l':
      viewLevels();
      break;
    default:
      Serial.print(bufByte);
      Serial.print(" ");
      Serial.println("unsupported");
      return;
  }
}

void setBaud() {
  unsigned int baud = 0;
  uint8_t digit;
  if (setBufPointer == 1) {
    Serial.print("BAUD rate: ");
    Serial.println(monitorBAUD);
  } else {
    for (uint8_t i = 1; i < setBufPointer; i++) {
      digit = serialBuffer[i];
  //    Serial.print(digit, HEX);
  //    Serial.print(" ");
      baud = baud * 10 + getDigit(digit);
    }
    Serial.print("New BAUD rate: ");
    Serial.println(baud);
    Serial1.begin(baud);
    Serial2.begin(baud);
    monitorBAUD = baud;
  }
  clearSerialBuffer();
}

void setConfig() {
  uint8_t config;
  if (setBufPointer == 1) {
    Serial.print("Config: ");
    Serial.println(monCFGstr);
  } else if (setBufPointer == 4) {
    config = getConfig(serialBuffer[1], serialBuffer[2], serialBuffer[3]);
    Serial1.begin(monitorBAUD, config);
    Serial2.begin(monitorBAUD, config);
    Serial.print("New config: ");
    monCFGstr[0] = serialBuffer[1];
    monCFGstr[1] = serialBuffer[2];
    monCFGstr[2] = serialBuffer[3];
    monCFGstr.toUpperCase();
    Serial.print(monCFGstr);
    Serial.print(" (");
    Serial.print(config, HEX);
    Serial.println(")");
  } else {
    Serial.println("unsupported");
    clearSerialBuffer();
  }
  
}

uint8_t getDigit(uint8_t d) {
    if (d < '0') return 0;
    if (d > '9') return 0;
    return d - '0';
}

uint8_t getConfig(uint8_t bitCount, uint8_t parity, uint8_t stopBitCnt) {
  if (parity > 'O') parity -= 0x20;  // assuming lower case, correcting
    if (bitCount == '7') {
        if (parity == 'O') {
            if (stopBitCnt == '2') {
                return SERIAL_7O2;
            } else { // '1'
                return SERIAL_7O1;
            }
        } else if (parity == 'E') {
            if (stopBitCnt == '2') {
                return SERIAL_7E2;
            } else { // '1'
                return SERIAL_7E1;
            }
        } else { // 'N' 
            if (stopBitCnt == '2') {
                return SERIAL_7N2;
            } else { // '1'
                return SERIAL_7N1;
            }
        }
    } else { // '8'
        if (parity == 'O') {
            if (stopBitCnt == '2') {
                return SERIAL_8O2;
            } else { // '1'
                return SERIAL_8O1;
            }
        } else if (parity == 'E') {
            if (stopBitCnt == '2') {
                return SERIAL_8E2;
            } else { // '1'
                return SERIAL_8E1;
            }
        } else { // 'N' 
            if (stopBitCnt == '2') {
                return SERIAL_8N2;
            } else { // '1'
                return SERIAL_8N1;
            }
        }
    }
    return SERIAL_8N1;
}

void usage() {
  Serial.println("Serial Scope Plus usage:");
  Serial.println(" B<baudrate>  - set BAUD rate");
  Serial.println(" C<config>    - set word size: [7|8][E|O|N]][1|2]");
  Serial.println(" L            - view Rx levels MON1 & MON2");
  Serial.println(" ?            - this help");
  clearSerialBuffer();
}

void clearSerialBuffer() {
  byte i;
  for (i = 0; i < SERIALBUFSIZE; i++) {
    serialBuffer[i] = 0;
  }
}

void viewLevels() {
    Serial.print(MON1);
    Serial.print(" ");
    Serial.println(digitalRead(MON1INPUT));
    Serial.print(MON2);
    Serial.print(" ");
    Serial.println(digitalRead(MON2INPUT));
}
