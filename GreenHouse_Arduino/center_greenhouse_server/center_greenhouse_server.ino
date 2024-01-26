#include <ESP8266WiFi.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266HTTPClient.h>

char ssid[] = "ThienNhan";
char pass[] = "20102001";

RF24 radio(2, 0);  // CE, CSN
const byte address[6] = "00001";

LiquidCrystal_I2C lcd1(0x26, 20, 4);
LiquidCrystal_I2C lcd2(0x27, 20, 4);

const char* serverAddress = "greenhouse-server-silk.vercel.app";
const int serverPort = 443;

int temperature = 0;
int humidity = 0;
int soil_moisture = 0;
int status_rain = 0;
int status_light_sensor = 0;
int status_led = 0;
int status_fan = 0;
int status_pump = 0;
int status_door = 0;
int roof = 0;

WiFiClientSecure client;

int currentHouse = 1;

bool checkServerConnection() {
  if (!client.connect(serverAddress, serverPort)) {
    Serial.println("Failed to connect to server.");
    return false;
  }
  Serial.println("Connected to server.");
  return true;
}

void sendDataToServerIfNeeded(int greenhouseNumber, int temperature, int humidity, int soil_moisture, int status_rain, int status_light_sensor, int status_led, int status_fan, int status_pump, int status_door, int roof) {
  if (!checkServerConnection()) {
    Serial.println("Cannot send data to server, check connection.");
    return;
  }

  HTTPClient http;

  String path = "/api/data/greenhouse-" + String(greenhouseNumber);
  String url = "https://" + String(serverAddress) + path;

  if (http.begin(client, url)) {
    http.addHeader("Content-Type", "application/json");

    String postData = "{\"temperature\": \"";
    postData.concat(String(temperature));
    postData.concat("\",\"humidity\":\"");
    postData.concat(String(humidity));
    postData.concat("\",\"soil_moisture\":\"");
    postData.concat(String(soil_moisture));
    postData.concat("\",\"status_light_sensor\":\"");
    postData.concat(String(status_light_sensor));
    postData.concat("\",\"status_rain\":\"");
    postData.concat(String(status_rain));
    postData.concat("\",\"status_led\":\"");
    postData.concat(String(status_led));
    postData.concat("\",\"status_fan\":\"");
    postData.concat(String(status_fan));
    postData.concat("\",\"status_pump\":\"");
    postData.concat(String(status_pump));
    postData.concat("\",\"status_door\":\"");
    postData.concat(String(status_door));
    postData.concat("\",\"roof\":\"");
    postData.concat(String(roof));
    postData.concat("\"}");

    int httpCode = http.POST(postData);

    if (httpCode > 0) {
      Serial.print("HTTP code: ");
      Serial.println(httpCode);

      if (httpCode == 201) {
        Serial.println("Data sent successfully!");
      } else {
        Serial.println("Failed to send data. Check server response.");
      }
    } else {
      Serial.println("HTTP request failed.");
    }

    http.end();
  } else {
    Serial.println("Unable to begin HTTP client.");
  }
}

void processRadioData(String data) {
  int a = data.indexOf("a");
  int b = data.indexOf("b");
  int c = data.indexOf("c");
  int d = data.indexOf("d");
  int e = data.indexOf("e");
  int f = data.indexOf("f");
  int g = data.indexOf("g");
  int h = data.indexOf("h");
  int k = data.indexOf("k");
  int l = data.indexOf("l");
  int m = data.indexOf("m");
  int w = data.indexOf("w");

  if (a >= 0 && b >= 0 && c >= 0 && d >= 0 && e >= 0 && f >= 0 && g >= 0 && h >= 0 && k >= 0 && l >= 0 && m >= 0 && w >= 0) {
    temperature = (data.substring(a + 1, b)).toInt();
    humidity = (data.substring(b + 1, c)).toInt();
    soil_moisture = (data.substring(d + 1, e)).toInt();
    status_rain = (data.substring(f + 1, g)).toInt();
    status_light_sensor = (data.substring(g + 1, h)).toInt();
    status_led = (data.substring(k + 1, l)).toInt();
    status_fan = (data.substring(c + 1, d)).toInt();
    status_pump = (data.substring(e + 1, f)).toInt();
    status_door = (data.substring(l + 1, m)).toInt();
    roof = (data.substring(h + 1, k)).toInt();
    int houseNumber = (data.substring(m + 1, w)).toInt();

    if (houseNumber == currentHouse) {
      if (currentHouse == 1) {
        lcd1.setCursor(4, 1);
        lcd1.print(temperature);
        lcd1.print((char)223);
        lcd1.print("C ");

        lcd1.setCursor(15, 1);
        lcd1.print(humidity);
        lcd1.print("% ");

        lcd1.setCursor(8, 2);
        lcd1.print(soil_moisture);
        lcd1.print("% ");

        lcd1.setCursor(13, 2);
        lcd1.print((status_rain == 1) ? "KO MUA" : "MUA   ");

        lcd1.setCursor(2, 3);
        lcd1.print((status_light_sensor == 1) ? "TROI TOI " : "TROI SANG");

        Serial.println("home 1"); 
        sendDataToServerIfNeeded(1, temperature, humidity, soil_moisture, status_rain, status_light_sensor, status_led, status_fan, status_pump, status_door, roof);

        currentHouse = 2;
      } else if (currentHouse == 2) {
        lcd2.setCursor(4, 1);
        lcd2.print(temperature);
        lcd2.print((char)223);
        lcd2.print("C ");

        lcd2.setCursor(15, 1);
        lcd2.print(humidity);
        lcd2.print("% ");

        lcd2.setCursor(8, 2);
        lcd2.print(soil_moisture);
        lcd2.print("% ");

        lcd2.setCursor(13, 2);
        lcd2.print((status_rain == 1) ? "KO MUA" : "MUA   ");

        lcd2.setCursor(2, 3);
        lcd2.print((status_light_sensor == 1) ? "TROI TOI " : "TROI SANG");

        Serial.println("home 2");
        sendDataToServerIfNeeded(2, temperature, humidity, soil_moisture, status_rain, status_light_sensor, status_led, status_fan, status_pump, status_door , roof);

        currentHouse = 1;
      }
    }
  } else {
    Serial.println("Invalid data format from radio.");
  }
}

void setup() {
  lcd1.init();
  lcd1.clear();
  lcd1.backlight();
  lcd2.init();
  lcd2.clear();
  lcd2.backlight();

  Serial.begin(9600);

  // Connect WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  client.setInsecure();

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.startListening();

  lcd1.setCursor(5, 0);
  lcd1.print("GREEN HOUSE 1");
  lcd1.setCursor(0, 1);
  lcd1.print("T : ");
  lcd1.setCursor(11, 1);
  lcd1.print("H : ");
  lcd1.setCursor(0, 2);
  lcd1.print("H_Dat : ");

  lcd2.setCursor(5, 0);
  lcd2.print("GREEN HOUSE 2");
  lcd2.setCursor(0, 1);
  lcd2.print("T : ");
  lcd2.setCursor(11, 1);
  lcd2.print("H : ");
  lcd2.setCursor(0, 2);
  lcd2.print("H_Dat : ");
}

void loop() {
  String data_ = "";
  if (radio.available()) {
    char text[100] = { 0 };
    radio.read(&text, sizeof(text));
    data_ = (String)text;
    Serial.println(data_);
  }
  if (data_ != "") {
    processRadioData(data_);
    delay(3000);
  }
}
