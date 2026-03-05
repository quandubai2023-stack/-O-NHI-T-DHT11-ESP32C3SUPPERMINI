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

/* ===== BUTTON ===== */
#define BTN_PIN 5

/* ===== WIFI AP ===== */
const char* AP_SSID = "DO_NHIET_DO";
const char* AP_PASS = "12345678";

WebServer server(80);

/* ===== DATA ===== */
float nhietDo = 0, doAm = 0;
float tMin = 100, tMax = -100;
float hMin = 100, hMax = 0;
bool showMinMax = false;

/* ===== WEB ===== */
String webPage() {
  String html;
  html += "<meta name='viewport' content='width=device-width'>";
  html += "<meta http-equiv='refresh' content='2'>";
  html += "<body style='background:#111;color:#0f0;text-align:center;font-family:Arial'>";
  html += "<h2>NHIET DO</h2><h1>" + String(nhietDo) + " °C</h1>";
  html += "<h2>DO AM</h2><h1>" + String(doAm) + " %</h1>";
  html += "<hr>";
  html += "<p>T MIN: " + String(tMin) + " | T MAX: " + String(tMax) + "</p>";
  html += "<p>H MIN: " + String(hMin) + " | H MAX: " + String(hMax) + "</p>";
  html += "</body>";
  return html;
}

/* ===== INTRO ===== */
void intro() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 4);
  display.print("DANG KHOI DONG ...");
  display.setCursor(18, 18);

  
  display.print("XIN CHAO !");
  display.display();
  delay(2000);
}

void setup() {
  /* I2C OLED */
  Wire.begin(9, 8);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  intro();

  /* Button */
  pinMode(BTN_PIN, INPUT_PULLUP);

  /* DHT */
  dht.begin();

  /* WiFi AP */
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  /* Web */
  server.on("/", []() {
    server.send(200, "text/html", webPage());
  });
  server.begin();
}

void loop() {
  server.handleClient();

  /* Read sensor mỗi 1s */
  static unsigned long lastRead = 0;
  if (millis() - lastRead > 1000) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      nhietDo = t;
      doAm = h;

      tMin = min(tMin, nhietDo);
      tMax = max(tMax, nhietDo);
      hMin = min(hMin, doAm);
      hMax = max(hMax, doAm);
    }
    lastRead = millis();
  }

  /* Button */
  showMinMax = (digitalRead(BTN_PIN) == LOW);

  /* OLED */
  display.clearDisplay();
  display.setTextSize(1);

  if (!showMinMax) {
    display.setCursor(0, 0);
    display.print("Nhiet do: ");
    display.print(nhietDo);
    display.print(" C");

    display.setCursor(0, 16);
    display.print("Do am: ");
    display.print(doAm);
    display.print(" %");
  } else {
    display.setCursor(0, 0);
    display.print("T ");
    display.print(tMin);
    display.print("/");
    display.print(tMax);

    display.setCursor(0, 16);
    display.print("H ");
    display.print(hMin);
    display.print("/");
    display.print(hMax);
  }

  display.display();
}
