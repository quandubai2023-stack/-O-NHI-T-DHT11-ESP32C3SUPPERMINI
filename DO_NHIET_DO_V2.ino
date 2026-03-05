#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/* ===== OLED ===== */
Adafruit_SSD1306 display(128, 32, &Wire, -1);

/* ===== DHT11 ===== */
#define DHTPIN 4
DHT dht(DHTPIN, DHT11);

/* ===== WIFI AP ===== */
const char* AP_SSID = "DO NHIET DO";
const char* AP_PASS = "12345678";

WebServer server(80);
float nhietDo = 0, doAm = 0;

/* ===== WEB ===== */
String web() {
  return "<meta name='viewport' content='width=device-width'>"
         "<meta http-equiv='refresh' content='1'>"
         "<body style='background:#111;color:#0f0;text-align:center;font-family:Arial'>"
         "<h2>Nhiet do</h2><h1>" + String(nhietDo) + " °C</h1>"
         "<h2>Do am</h2><h1>" + String(doAm) + " %</h1>";
}

/* ===== INTRO 4s ===== */
void intro() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(10, 8);
  display.print("DANG KHOI DONG...");
 
  display.setCursor(38, 20);
  display.print("XIN CHAO");
 
  display.display();
  delay(5000);   //
}

void setup() {
  Wire.begin(9, 8);   // SDA=9, SCL=8

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  intro();

  dht.begin();
  delay(1500);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/", []() {
    server.send(200, "text/html", web());
  });
  server.begin();
}

void loop() {
  server.handleClient();

  static unsigned long last = 0;
  if (millis() - last > 1000) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      nhietDo = t - 3.0;
      doAm = h;
    }
    last = millis();
  }

  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print("Nhiet do: ");
  display.print(nhietDo);
  display.print(" C");

  display.setCursor(0, 16);
  display.print("Do am: ");
  display.print(doAm);
  display.print(" %");

  display.display();
}
