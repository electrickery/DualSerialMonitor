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
 
#define USBBAUD 1000000
#define PXBAUD    38400
#define MON1    "1>"
#define MON2    "2>"

#define PORT1 1
#define PORT2 2
#define PORT3 3

uint8_t data;
uint8_t portActive = 0;

#define TIMEOUT 100L
uint32_t milliCount = 0L;

void setup() {
  Serial.begin(USBBAUD);
  Serial1.begin(PXBAUD);
  Serial2.begin(PXBAUD);
  while(!Serial);
  Serial.println("#Serial scope v0.1.0");
  Serial.print("#Monitoring Serial1 as '");
  Serial.print(MON1);
  Serial.println("'");
  Serial.print("#Monitoring Serial2 as '");
  Serial.print(MON2);
  Serial.println("'");
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
}
