#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/* ===== OLED ===== */
Adafruit_SSD1306 display(128, 32, &Wire, -1);

/* ===== DHT11 ===== */
#define DHTPIN 4
DHT dht(DHTPIN, DHT11);

float nhietDo = 0, doAm = 0;

void setup() {
  Wire.begin(9, 8);          // GIỮ NGUYÊN GPIO
  Wire.setClock(100000);    // I2C ổn định

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);              // OLED lỗi thì đứng
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  dht.begin();
  delay(2000);              // DHT11 cần ổn định
}

void loop() {
  static unsigned long last = 0;

  if (millis() - last > 2000) {   // DHT11 >= 2s
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      nhietDo = t - 3.0;   // hiệu chỉnh nếu cần
      doAm = h;
    }
    last = millis();
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

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
