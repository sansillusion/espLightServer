#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ESPmDNS.h>

#define ONE_WIRE_BUS 15
#define DHTPIN 2
#define DHTTYPE DHT11

String resetpass = "MyResetPassword"; // password to reset wifimanager when allready connected
const String thingkey = "YOURTHINGSPEAKKEY"; //your thingspeak API key
String dernadd = "Fraichement boot&eacute;"; //variable for ip logging (you can put your fresh boot mesage here in text/html)

uint8_t pinRouge = 12;
uint8_t pinVerte = 14; // internally pulled up
uint8_t pinBleu = 13;

const long interval = 60000; // interval in ms for sending temperature data to thingspeak
const String liens = "<a href=\"/\">Acceuil</a> - <a href=\"/temp\">Temp&eacute;rature</a> - <a href=\"/version\">Version</a> - <a href=\"/reset\">Reset</a> - <a href=\"https://thingspeak.com/channels/289148\">Temp stats</a>\n"; // html/links shown at bottom of pages
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

String derncoul;
long r;
long g;
long b;
int rouge = 0;
int vert = 0;
int bleu = 0;
unsigned long previousMillis = 0;
unsigned long previousMillisr = 0;
unsigned long previousMillisg = 0;
unsigned long previousMillisb = 0;
int attendR = 70;
int attendG = 70;
int attendB = 70;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int roulepastusuite = 0;

const String css = "<style>\n"
                   ".color {\n"
                   "width:50%;\n"
                   "height:80px;\n"
                   "padding:5px;\n"
                   "margin:10px;\n}\n"
                   ".button1 {\n"
                   "background-color: #4CAF50;\n"
                   "border: none;\n"
                   "color: white;\n"
                   "padding: 15px 32px;\n"
                   "text-align: center;\n"
                   "text-decoration: none;\n"
                   "display: inline-block;\n"
                   "font-size: 24px;\n"
                   "margin: 4px 2px;\n"
                   "cursor: pointer; -webkit-transition-duration: 0.4s;\n"
                   "transition-duration: 0.4s; width: 80%;\n}\n"
                   ".button1:hover {\n"
                   "box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);\n}\n"
                   "button {\n"
                   "background-color: #AC0050;\n"
                   "border: none;\n"
                   "color: white;\n"
                   "padding: 15px 32px;\n"
                   "text-align: center;\n"
                   "text-decoration: none;\n"
                   "display: inline-block;\n"
                   "font-size: 24px;\n"
                   "margin: 4px 2px;\n"
                   "cursor: pointer; -webkit-transition-duration: 0.4s;\n"
                   "transition-duration: 0.4s; width: 80%;\n}\n"
                   "button:hover {\n"
                   "box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);\n}\n"
                   "</style>\n";

void fadeR() {
  if (rouge < r) {
    rouge++;
    ledcWrite(1, rouge);
  } else if (rouge > r) {
    rouge--;
    ledcWrite(1, rouge);
  }
}

void fadeG() {
  if (vert < g) {
    vert++;
    ledcWrite(2, vert);
  } else if (vert > g) {
    vert--;
    ledcWrite(2, vert);
  }
}

void fadeB() {
  if (bleu < b) {
    bleu++;
    ledcWrite(3, bleu);
  } else if (bleu > b) {
    bleu--;
    ledcWrite(3, bleu);
  }
}

void handleLeds() {
  String addy = server.client().remoteIP().toString();
  Serial.println("");
  Serial.println(addy);
  String testteu = server.arg("COULEUR");
  Serial.println(testteu);
  if (testteu != 0) {
    derncoul = testteu;
    long number = strtol( &testteu[1], NULL, 16);
    r = number >> 16;
    g = number >> 8 & 0xFF;
    b = number & 0xFF;
    if (r != rouge) {
      Serial.print("Rouge = ");
      Serial.println(r);
      dernadd = addy;
    }
    if (g != vert) {
      Serial.print("Vert = ");
      Serial.println(g);
      dernadd = addy;
    }
    if (b != bleu) {
      Serial.print("Bleu = ");
      Serial.println(b);
      dernadd = addy;
    }
  }
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta http-equiv=\"refresh\" content=\"0;url=/\" />\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere soit</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "Merci</body></html>\n";
  server.send(200, "text/html", contenu);
}

void handleRoot() {
  String addy = server.client().remoteIP().toString();
  Serial.println("");
  Serial.println(addy);
  Serial.println("Page acceuil");
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere soit</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js\"></script>\n";
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n"
             "<form id=\"laforme\" action=\"/leds\" method=\"post\">\n"
             "<h1>Change la couleur de mon bureau:</h1>"
             "<input id=\"colorpad\" type=\"color\" name=\"COULEUR\" class=\"color\" value=\"" + derncoul + "\"><br>\n"
             "<input type=\"submit\" class=\"button1\" value=\"Changer\">\n"
             "</form>\n<br>"
             "<button onclick=\"disco()\">Hazard</button><br>\n"
             "<button onclick=\"startDisco()\">Auto Disco</button><br>\n"
             "Dernier IP &agrave; avoir chang&eacute; la couleur : ";
  contenu += dernadd;
  contenu += "<br>\n";
  contenu += liens;
  contenu += "<script>\n"
             "function getRandomColor() {\n"
             "var letters = '0000F0F1205F9B0F';\n"
             "var color = '#';\n"
             "for (var i = 0; i < 6; i++ ) {\n"
             "    color += letters[Math.floor(Math.random() * 16)];\n"
             "}\n"
             "return color;\n"
             "}\n"
             "function setRandomColor(){\n"
             "$(\"#colorpad\").val(getRandomColor());\n"
             "}\n"
             "$(\"#laforme\").submit(function( event ) {\n"
             "  event.preventDefault();\n"
             "$.ajax({\n"
             "url : $(this).attr('action') || window.location.pathname,\n"
             "type: \"POST\",\n"
             "data: $(this).serialize(),\n"
             "});\n"
             "});\n"
             "function disco(){\n"
             "setRandomColor();\n"
             "$(\"#laforme\").submit();\n"
             "}\n"
             "var myVar = \" \";\n"
             "function startDisco(){\n"
             " disco();"
             " myVar = setInterval(function(){ disco() }, 13500);\n"
             "}\n"
             "</script>\n"
             "</div></body></html>\n";
  server.send(200, "text/html", contenu);
}

void handleReset() {
  int ouireset = 0;
  String addy = server.client().remoteIP().toString();
  Serial.println("");
  Serial.println(addy);
  Serial.println("!!! RESET PAGE !!!");
  String testteu = server.arg("RESET");
  String passme = server.arg("PASSME");
  if (testteu == "OUI") {
    if (passme == resetpass) {
      ouireset = 1;
    } else {
      ouireset = 2;
    }
  }
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere se RESET</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n";
  if (!ouireset) {
    contenu += "<form action=\"/reset\" method=\"post\">\n"
               "<h1>Tu veux resetter le wifi network pour reconfigurer ?</h1>"
               "<input type=\"hidden\" name=\"RESET\" value=\"OUI\"><br>\n"
               "<input type=\"password\" name=\"PASSME\" value=\"password\"><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Reset Wifi Network\">\n"
               "</form>\n";
  } else if (ouireset != 1) {
    contenu += "<form action=\"/reset\" method=\"post\">\n"
               "<h1>Tu veux resetter le wifi network pour reconfigurer ?</h1>"
               "<input type=\"hidden\" name=\"RESET\" value=\"OUI\"><br>\n"
               "<input type=\"password\" name=\"PASSME\" value=\"password\"><br>\n"
               "<h2>!!! MAUVAIS PASSWORD !!!</h2><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Reset Wifi Network\">\n"
               "</form>\n";
    Serial.print("BAD PASSWORD : ");
    Serial.println(passme);
  } else if (ouireset == 1) {
    contenu += "<h1>Reset Dans 10 Secondes</h1>\n";
  }
  contenu += "<br>Dernier IP &agrave; avoir chang&eacute; la couleur :";
  contenu += dernadd;
  contenu += "\n<br>";
  contenu += liens;
  contenu += "\n<br></div></body></html>\n";
  server.send(200, "text/html", contenu);
  if (ouireset == 1) {
    Serial.println("Reset dans 10 secondes");
    delay(10000);
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.restart();
  }
}

void handleNotFound() {
  String addy = server.client().remoteIP().toString();
  Serial.println("");
  Serial.println(addy);
  String htmlmessage = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  htmlmessage += "<meta charset=\"UTF-8\" />\n<title>404 Not found</title>\n"
                 "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  htmlmessage += css;
  htmlmessage += "</head>\n<body>\n"
                 "<div style=\"text-align:center;width:100%;\">\n";
  htmlmessage += "<h1>404 File Not Found</h1><br>\n";
  htmlmessage += "URI: ";
  htmlmessage += server.uri();
  htmlmessage += "<br>\nMethod: ";
  htmlmessage += (server.method() == HTTP_GET) ? "GET" : "POST";
  htmlmessage += "<br>\nArguments: ";
  htmlmessage += server.args();
  htmlmessage += "<br>\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    htmlmessage += " " + server.argName(i) + ": " + server.arg(i) + "<br>\n";
  }
  htmlmessage += "<br>\n";
  htmlmessage += liens;
  htmlmessage += "<br>\n</div></body></html>\n";
  server.send(404, "text/html", htmlmessage);
  String message = "404 File Not Found\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  Serial.print(message);
}

void updateTemp(String tempe, float h, float t) {
  if (tempe != "-127.00") {
    Serial.print("Envoi de temperature et humidite: ");
    Serial.print(tempe);
    Serial.print(" - ");
    HTTPClient http;
    String webadd = "http://api.thingspeak.com/update?key=";
    webadd += thingkey;
    webadd += "&field1=";
    webadd += tempe;
    if (h == h) { // to prevent NaN
      webadd += "&field2=";
      webadd += h;
      Serial.println(h);
      webadd += "&field3=";
      webadd += t;
    }
    http.begin(webadd);
    int httpCode = http.GET();//Send the request
    http.end();   //Close connection
  }
}

void latemp() {
  sensors.requestTemperatures();
  float thetemp = sensors.getTempCByIndex(0);
  char buffer[10];
  String tempC = dtostrf(thetemp, 5, 2, buffer);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  updateTemp(tempC, h, t);
}

void handleTemp() {
  sensors.requestTemperatures();
  float latemp = sensors.getTempCByIndex(0);
  float h = dht.readHumidity();
  while (latemp == -127.00) {
    sensors.requestTemperatures();
    latemp = sensors.getTempCByIndex(0);
  }
  String addy = server.client().remoteIP().toString();
  Serial.println("");
  Serial.println(addy);
  Serial.print("Temperature : ");
  Serial.println(latemp);
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Quel chaleur</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n";
  contenu += "<form action=\"/\" method=\"get\">\n"
             "<h1>Voici la temperature</h1>"
             "<h2>";
  contenu += latemp;
  contenu += "&deg;C Humidit&eacute; : ";
  contenu += h;
  contenu += "</h2><br>\n<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/289148/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
             "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/289148/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
             "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/289148/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
             "<input type=\"submit\" class=\"button1\" value=\"Go Home\">\n"
             "</form>\n"
             "<br>Dernier IP &agrave; avoir chang&eacute; la couleur :";
  contenu += dernadd;
  contenu += "\n<br>";
  contenu += liens;
  contenu += "\n<br></div></body></html>\n";
  server.send(200, "text/html", contenu);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  ledcAttachPin(pinRouge, 1); // assign RGB led pins to channels
  ledcAttachPin(pinVerte, 2);
  ledcAttachPin(pinBleu, 3);
  pinMode(pinRouge, OUTPUT);
  pinMode(pinVerte, OUTPUT);
  pinMode(pinBleu, OUTPUT);
  digitalWrite(pinRouge, LOW);
  digitalWrite(pinBleu, LOW);
  digitalWrite(pinVerte, LOW);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  MDNS.begin("lumiere");
  server.on("/", handleRoot);
  server.on("/leds", handleLeds);
  server.on("/reset", handleReset);
  server.on("/temp", handleTemp);
  server.on("/version", []() {
    String addy = server.client().remoteIP().toString();
    Serial.println("");
    Serial.println(addy);
    Serial.println("Version request");
    server.send(200, "text/html", "V1.9, Steve Olmstead sansillusion@gmail.com\n\n<br><br>"
                "Added fader function\nRemoved connection watchdog (better have good signal)\n<br>"
                "Removed mDns (did not work anyway)\n\n<br><br>"
                "Added smoother fading\n\n<br><br>"
                "Added AutoConect WifiManager !\n\n<br><br>"
                "Added basic last IP logging\n<br>"
                "Added /reset page to reset wifiManager connection settings\n\n<br><br>"
                "Added /temp (temperature sensor read of ky-001)\n\n<br><br>"
                "Added thingspeak support to send temperature each minute<br>\n\n<br>"
                "Major rewrite for RGB led trip usin mosfet<br>\n"
                "Removed serial to free resources for software pwm (still flikers but less than before)<br>\n\n<br>"
                "Seperated LEDs handling page from index\n<br>"
                "Added jQuery to load settings without reloading page\n<br>"
                "Added random color selection button\n\n<br><br>"
                "Added disco mode script and buttons\n\n<br><br>"
                "Removed mDNS (does not work with esp32)\n<br>"
                "Rewrite of code (removed Ticker and changed a few libraries) for esp32 !!!\n\n<br><br>"
                "Changed fader functions for smoother fade\n<br>"
                "Cleaned code\n\n<br><br>" + liens);
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  MDNS.addService("http", "tcp", 80);
  sensors.begin();
}

void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    latemp();
  }
  if (r != rouge) {
    currentMillis = millis();
    if (currentMillis - previousMillisr >= attendR) {
      fadeR();
      previousMillisr = currentMillis;
      if (rouge > 0)
        attendR = 70;
      if (rouge > 40)
        attendR = 60;
      if (rouge > 80)
        attendR = 50;
      if (rouge > 100)
        attendR = 40;
      if (rouge > 120)
        attendR = 30;
      if (rouge > 140)
        attendR = 20;
      if (rouge > 180)
        attendR = 10;
    }
  }
  if (g != vert) {
    currentMillis = millis();
    if (currentMillis - previousMillisg >= attendG) {
      fadeG();
      previousMillisg = currentMillis;
      if (vert > 0)
        attendG = 70;
      if (vert > 40)
        attendG = 60;
      if (vert > 80)
        attendG = 50;
      if (vert > 100)
        attendG = 40;
      if (vert > 120)
        attendG = 30;
      if (vert > 140)
        attendG = 20;
      if (vert > 180)
        attendG = 10;
    }
  }
  if (b != bleu) {
    currentMillis = millis();
    if (currentMillis - previousMillisb >= attendB) {
      fadeB();
      previousMillisb = currentMillis;
      if (bleu > 0)
        attendB = 70;
      if (bleu > 40)
        attendB = 60;
      if (bleu > 80)
        attendB = 50;
      if (bleu > 100)
        attendB = 40;
      if (bleu > 120)
        attendB = 30;
      if (bleu > 140)
        attendB = 20;
      if (bleu > 180)
        attendB = 10;
    }
  }
}

