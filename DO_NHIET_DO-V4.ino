#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

/* ===== PHIEN BAN ===== */
#define FW_VERSION "4.0"

/* ===== OLED ===== */
Adafruit_SSD1306 display(128, 32, &Wire, -1);

/* ===== DHT ===== */
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* ===== WIFI ===== */
const char* AP_SSID = "DO NHIET DO";
const char* AP_PASS = "12345678";
WebServer server(80);

/* ===== BIEN ===== */
float nhietDo = 0, doAm = 0;
float nhietMax = -100, nhietMin = 100;
float tongNhiet = 0, nhietTB = 0;
unsigned long dem = 0;

/* ===== INTRO ===== */
void intro() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(18, 2);
  display.print("DANG KHOI DONG");

  display.setCursor(44, 12);
  display.print("FW ");
  display.print(FW_VERSION);

  display.drawRect(14, 22, 100, 6, SSD1306_WHITE);
  display.display();

  for (int i = 0; i <= 100; i++) {
    display.fillRect(16, 24, map(i, 0, 100, 0, 96), 2, SSD1306_WHITE);
    display.display();
    delay(40);
  }

  display.clearDisplay();
  display.setCursor(40, 12);
  display.print("XIN CHAO");
  display.display();
  delay(1500);
}

/* ===== WEB TRANG CHINH ===== */
String webTrangChinh() {
  String s;
  s += "<!DOCTYPE html><html><head>";
  s += "<meta charset='UTF-8'>";
  s += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  s += "<style>";
  s += "body{background:#000;color:#0f0;font-family:Arial;text-align:center}";
  s += ".box{font-size:22px;margin:10px}";
  s += "button{font-size:18px;padding:10px 18px}";
  s += "</style>";
  s += "<script>";
  s += "async function capNhat(){";
  s += "let r=await fetch('/data'); let d=await r.json();";
  s += "t.innerHTML=d.t+' C';";
  s += "h.innerHTML=d.h+' %';";
  s += "max.innerHTML=d.max+' C';";
  s += "min.innerHTML=d.min+' C';";
  s += "tb.innerHTML=d.tb+' C';}";
  s += "setInterval(capNhat,2000);";
  s += "</script></head>";
  s += "<body onload='capNhat()'>";
  s += "<h2>HE THONG DO NHIET DO</h2>";
  s += "<div class='box'>NHIET DO: <b id='t'></b></div>";
  s += "<div class='box'>DO AM: <b id='h'></b></div>";
  s += "<div class='box'>CAO NHAT: <b id='max'></b></div>";
  s += "<div class='box'>THAP NHAT: <b id='min'></b></div>";
  s += "<div class='box'>TRUNG BINH: <b id='tb'></b></div>";
  s += "<a href='/chart'><button>BIEU DO</button></a>";
  s += "</body></html>";
  return s;
}

/* ===== JSON ===== */
String duLieuJson() {
  return "{"
    "\"t\":" + String(nhietDo,1) + ","
    "\"h\":" + String(doAm,0) + ","
    "\"max\":" + String(nhietMax,1) + ","
    "\"min\":" + String(nhietMin,1) + ","
    "\"tb\":" + String(nhietTB,1) +
  "}";
}

/* ===== WEB BIEU DO ===== */
String webBieuDo() {
  String s;
  s += "<!DOCTYPE html><html><head>";
  s += "<meta charset='UTF-8'>";
  s += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  s += "<style>";
  s += "html,body{margin:0;background:#000;height:100%;overflow:hidden}";
  s += "canvas{width:100%;height:100%}";
  s += "</style></head><body>";
  s += "<canvas id='c'></canvas>";
  s += "<script>";
  s += "let c=document.getElementById('c'),x=c.getContext('2d');";
  s += "function rs(){c.width=innerWidth;c.height=innerHeight}";
  s += "async function ve(){";
  s += "let r=await fetch('/data'); let d=await r.json();";
  s += "x.clearRect(0,0,c.width,c.height);";
  s += "for(let t=0;t<=50;t+=5){";
  s += "let y=c.height-(t*c.height/50);";
  s += "x.strokeStyle='#333';x.beginPath();x.moveTo(0,y);x.lineTo(c.width,y);x.stroke();";
  s += "x.fillStyle='#888';x.fillText(t+'C',5,y-2);}";
  s += "let y=c.height-(d.t*c.height/50);";
  s += "x.strokeStyle=d.t>35?'red':'lime';";
  s += "x.lineWidth=3;x.beginPath();x.moveTo(0,y);x.lineTo(c.width,y);x.stroke();}";
  s += "setInterval(ve,2000);addEventListener('resize',rs);rs();ve();";
  s += "</script></body></html>";
  return s;
}

/* ===== SETUP ===== */
void setup() {
  Wire.begin(9, 8);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  intro();
  dht.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/", [](){ server.send(200,"text/html",webTrangChinh()); });
  server.on("/data", [](){ server.send(200,"application/json",duLieuJson()); });
  server.on("/chart", [](){ server.send(200,"text/html",webBieuDo()); });

  server.begin();
}

/* ===== LOOP ===== */
void loop() {
  server.handleClient();

  static unsigned long last = 0;
  if (millis() - last > 2000) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      nhietDo = t - 3.0;
      doAm = h;

      if (nhietDo > nhietMax) nhietMax = nhietDo;
      if (nhietDo < nhietMin) nhietMin = nhietDo;

      tongNhiet += nhietDo;
      dem++;
      nhietTB = tongNhiet / dem;
    }
    last = millis();
  }

  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0,0);
  display.print("NHIET DO:");
  display.print(nhietDo,1);
  display.print("C");

  display.setCursor(0,8);
  display.print("CAO:");
  display.print(nhietMax,1);
  display.print(" THAP:");
  display.print(nhietMin,1);

  display.setCursor(0,16);
  display.print("TRUNG BINH:");
  display.print(nhietTB,1);

  display.setCursor(0,24);
  display.print("DO AM:");
  display.print(doAm,0);
  display.print("%");

  display.display();
}
