#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>
//dht DHT;                // Creates a DHT object
DHT dht(7, DHT11);
int m = 1;

RF24 radio(9, 10);
const byte address[6] = "00001";

#define cs 10   //đặt tên cho chân Chip Select (CS) của module LoRa SX1278
#define rst 9   //đặt tên cho chân Reset (RST) của module LoRa SX1278
#define dio0 2  //đặt tên cho chân DIO0 (Interrupt) của module LoRa SX1278.
//#define outPin A3
#define rain_sensor A1      // Chân A1 (15) analog kết nối với cảm biến mưa
#define light_sensor A2  // chân A2 (16) analog kết nối cảm biến ánh sáng
#define soil_moisture_sensor A0  //chân A0 (14) analog kết nối cảm biến độ ẩm đất

#define led 33                // chân số 33 kết nối vs đèn
#define pump 35                // chân 35 kết nối với bơm
#define fan 31               //chân 31 kết nối với quạt

#define open_door 5
#define close_door 6
#define open_roof 4
#define close_roof 3

#define limit_switch_open_door 23        // Chân số 23 kết nối với công tắt hành trình 1
#define limit_switch_close_door 25      // Chân số 25 kết nối với công tắt hành trình 2
#define limit_switch_open_roof 29    // Chân số 18 kết nối với công tắt hành trình 4
#define limit_switch_close_roof 27  // Chân số 17 kết nối với công tắt hành trình 3

int temperature;
int humidity;

void setup() {
  Serial.begin(9600);   // Khởi tạo kết nối Serial với baud rate 9600
  Serial3.begin(9600);  // Khởi động giao tiếp serial với module RFID
  dht.begin();
  radio.begin();

  //set the address
  radio.openWritingPipe(address);
  //Set module as transmitter
  radio.stopListening();

  pinMode(limit_switch_open_door, INPUT_PULLUP);
  pinMode(limit_switch_close_door, INPUT_PULLUP);
  pinMode(limit_switch_open_roof, INPUT_PULLUP);
  pinMode(limit_switch_close_roof, INPUT_PULLUP);

  // Set all the motor control pins to outputs
  pinMode(open_door, OUTPUT);
  pinMode(close_door, OUTPUT);
  pinMode(open_roof, OUTPUT);
  pinMode(close_roof, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(open_door, LOW);
  digitalWrite(close_door, LOW);
  digitalWrite(open_roof, LOW);
  digitalWrite(close_roof, LOW);

  pinMode(led, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(fan, OUTPUT);
  //rfidSerial.begin(9600); // Khởi động giao tiếp serial với module RFID

  // while (!Serial);
  //  Serial.println("LoRa Sender");
  //  LoRa.setPins(ss, rst, dio0);
  //  delay(50);
  //  if (!LoRa.begin(433E6)) {
  //    Serial.println("Starting LoRa failed!");
  //    while (1)
  //      ;
  //  }
  //  //  LoRa.setTxPower(10);
  //  delay(1000);
}

void loop() {

  //////////////////////////////////Data transfer//////////////////
  String data_transfer = "a";
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  data_transfer += temperature;
  data_transfer += 'b';
  data_transfer += humidity;
  data_transfer += 'c';

  if (temperature > 29) {
    digitalWrite(fan, HIGH);
    data_transfer += '2';
  } else {
    digitalWrite(fan, LOW);
    data_transfer += '1';
  }
  data_transfer += 'd';

  int humidity_value = map(analogRead(soil_moisture_sensor), 0, 1023, 100, 1);
  data_transfer += humidity_value;
  data_transfer += 'e';
  //
  if (humidity_value < 50) {
    digitalWrite(pump, HIGH);
    data_transfer += '2';
  } else if (humidity_value > 51) {
    digitalWrite(pump, LOW);
    data_transfer += '1';
  }
  data_transfer += 'f';
  //
  bool rain_value = (analogRead(rain_sensor) > 510) ? false : true;
  data_transfer += (rain_value == false) ? '1' : '2';
  data_transfer += 'g';
  Serial.println(analogRead(rain_sensor));
  //
  int light_value = map(analogRead(light_sensor), 0, 1023, 100, 1);
  data_transfer += (String)((light_value < 20) ? '1' : '2');  //1: troi toi , 2: troi sang
  data_transfer += 'h';

  if (light_value <= 60 && digitalRead(limit_switch_open_roof) == 1) {
    analogWrite(open_roof, 50);
    analogWrite(close_roof, 0);
    data_transfer += '2';
  }
  if (light_value <= 60 && digitalRead(limit_switch_open_roof) == 0) {
    analogWrite(open_roof, 0);
    analogWrite(close_roof, 0);
    data_transfer += '1';
  }
  if (light_value > 60 && digitalRead(limit_switch_close_roof) == 1) {
    analogWrite(open_roof, 0);
    analogWrite(close_roof, 50);
    data_transfer += '2';
  }
  if (light_value > 60 && digitalRead(limit_switch_close_roof) == 0) {
    analogWrite(open_roof, 0);
    analogWrite(close_roof, 0);
    data_transfer += '1';
  }
  data_transfer += 'k';

  if (light_value <= 10) {
    digitalWrite(led, HIGH);
    data_transfer += '2';
  }
  if (light_value > 11) {
    digitalWrite(led, LOW);
    data_transfer += '1';
  }
  data_transfer += 'l';

  while (Serial3.available()) {
    if (Serial3.find("B7940D")) {
      if (m == 0 && digitalRead(limit_switch_open_door) == 0) {
        m = 1;
      } else if (m == 1 && digitalRead(limit_switch_close_door) == 0) {
        m = 0;
      }
    }
  }
  if (m == 0) {
    data_transfer += '2';
    if (digitalRead(limit_switch_open_door) == 1) {
      analogWrite(open_door, 40);
      analogWrite(close_door, 0);
    } else {
      analogWrite(open_door, 0);
      analogWrite(close_door, 0);
    }
  } else {
    data_transfer += '1';
    if (digitalRead(limit_switch_close_door) == 1) {
      analogWrite(open_door, 0);
      analogWrite(close_door, 40);
    } else {
      analogWrite(open_door, 0);
      analogWrite(close_door, 0);
    }
  }
  data_transfer += 'm';
  data_transfer += '1';
  data_transfer += 'w';

  Serial.println(data_transfer); 
  
  const char text[100];
  for (byte len = 1; len <= data_transfer.length() + 1; len++) {
    data_transfer.toCharArray(text, len);
  }
  radio.write(&text, sizeof(text));

  //  LoRa.beginPacket();
  //  LoRa.print(data_transfer);
  //  LoRa.endPacket();
  delay(500);
}
