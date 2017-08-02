#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

// sensors stuff
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ESPmDNS.h>
#define ONE_WIRE_BUS 5
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// end of sensors

// Settings you should check/change
const String resetpass = "YourWifiResetPassword"; // password to reset wifimanager when allready connected
const String thingkey = "YourThingSpeakKey"; //your thingspeak API key
const String thingchanel = "YourThingspeakChanel#"; //your thingspeak Chanel #
const long interval = 60000; // interval in ms for sending temperature data to thingspeak
const String liens = "<a href=\"/\">Acceuil</a> - <a href=\"/temp\">Temp&eacute;rature</a> - <a href=\"/version\">Version</a> - <a href=\"/reset\">Reset</a> - <a href=\"https://thingspeak.com/channels/289148\">Temp stats</a>\n"; // html/links shown at bottom of pages
String dernadd = "Fraichement boot&eacute;"; //variable for ip logging (you can put your fresh boot mesage here in text/html)
// led pins
uint8_t pinRouge = 12;
uint8_t pinVerte = 14; // internally pulled up
uint8_t pinBleu = 13;
// end of settings you should check/change

// internally used vars
WebServer server(80);
String derncoul = "#000000";
long r = 0;
long g = 0;
long b = 0;
int rouge = 0;
int vert = 0;
int bleu = 0;
unsigned long previousMillis = 0;
unsigned long previousMillisr = 0;
unsigned long previousMillisg = 0;
unsigned long previousMillisb = 0;
unsigned long previousMillisf = 0;
int attendR = 70;
int attendG = 70;
int attendB = 70;
int attendF = 700;
unsigned long flashfois = 0;
unsigned long flashrendu = 0;
int roulepastusuite = 0;
int flashoufade = 0;
String logs[51];
// end of internally used vars

// stylesheet for web pages
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
                   "div#logs {"
                   "width: 100%;"
                   "height: 450px;"
                   "font-size: 12px;"
                   "overflow: auto;"
                   "text-align:left;"
                   "}"
                   "</style>\n";
// end of stylesheet for web pages

// functions used internally

// used to flash leds
void flashfunk() {
  if (flashrendu <= flashfois) {
    flashrendu++;
    if (rouge == r) {
      ledcWrite(1, 0);
      rouge = 0;
    } else {
      ledcWrite(1, r);
      rouge = r;
    }
    if (vert == g) {
      ledcWrite(2, 0);
      vert = 0;
    } else {
      ledcWrite(2, g);
      vert = g;
    }
    if (bleu == b) {
      ledcWrite(3, 0);
      bleu = 0;
    } else {
      ledcWrite(3, b);
      bleu = b;
    }
  } else {
    r = rouge;
    b = bleu;
    g = vert;
    flashoufade = 0;
    flashrendu = 0;
    Serial.println("Flash fini");
  }
}

// used to fade red
void fadeR() {
  if (rouge < r) {
    rouge++;
    ledcWrite(1, rouge);
  } else if (rouge > r) {
    rouge--;
    ledcWrite(1, rouge);
  }
}

// used to fade green
void fadeG() {
  if (vert < g) {
    vert++;
    ledcWrite(2, vert);
  } else if (vert > g) {
    vert--;
    ledcWrite(2, vert);
  }
}

// used to fade blue
void fadeB() {
  if (bleu < b) {
    bleu++;
    ledcWrite(3, bleu);
  } else if (bleu > b) {
    bleu--;
    ledcWrite(3, bleu);
  }
}

// log rotator
void logtourne() {
  for (int i = 49; i > 0; i--) {
    int l = i;
    l = l + 1;
    logs[l] = logs[i];
  }
}

// web page to handle flashing of leds (color/delay/number of flashes)
void handleLog() {
  String addy = server.client().remoteIP().toString();
  String contenu = "";
  for (int i = 1; i < 51; i++) {
    if (logs[i] != "&hearts; vide") {
      contenu += logs[i];
      contenu += "\n<br>";
    }
  }
  server.send(200, "text/plain", contenu);
  Serial.println("");
  Serial.println(addy);
  Serial.println("Logs");
  videCoeur();
}

// web page to handle flashing of leds (color/delay/number of flashes)
void handleFlash() {
  String addy = server.client().remoteIP().toString();
  String testteu = server.arg("COULEUR");
  String attend = server.arg("DELAIS");
  attendF = attend.toInt();
  String flashnombre = server.arg("FOIS");
  flashfois = flashnombre.toInt();
  flashfois = flashfois * 2;
  Serial.println("");
  Serial.println(addy);
  Serial.println("Flash");
  Serial.println(testteu);
  Serial.print("Delais - ");
  Serial.println(attendF);
  Serial.print("Fois - ");
  Serial.println(flashfois / 2);
  if (testteu != 0) {
    derncoul = testteu;
    long number = strtol( &testteu[1], NULL, 16);
    r = number >> 16;
    g = number >> 8 & 0xFF;
    b = number & 0xFF;
    Serial.print("Rouge = ");
    Serial.println(r);
    Serial.print("Vert = ");
    Serial.println(g);
    Serial.print("Bleu = ");
    Serial.println(b);
    dernadd = addy;
    flashoufade = 1;
    flashrendu = 0;
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
    rouge = 0;
    bleu = 0;
    vert = 0;
  }
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta http-equiv=\"refresh\" content=\"0;url=/party\" />\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere soit</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "Merci</body></html>\n";
  server.send(200, "text/html", contenu);
  logtourne();
  logs[1] = addy;
  logs[1] += " : Flash : Couleur=";
  logs[1] += testteu;
  logs[1] += " : Delais=";
  logs[1] += attend;
  logs[1] += " : Fois=";
  logs[1] += flashnombre;
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
}

// web page for flash mode
void handlePitoune() {
  String addy = server.client().remoteIP().toString();
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere log</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js\"></script>\n";
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n"
             "<h1>Les visiteurs</h1>\n"
             "<div id=\"logs\"></div>\nDernier IP &agrave; avoir chang&eacute; la couleur : ";
  contenu += dernadd;
  contenu += "<br>\n";
  contenu += liens;
  contenu += "<script>\n"
             "$( \"#logs\" ).load( \"/logs\" );\n"
             "function pitoune(){\n"
             "$( \".pitoune:empty\" ).remove();\n"
             "$( \"#logs\" ).prepend($( \"<div class=\\\"pitoune\\\"></div>\" ).load( \"/logs\" ));\n"
             "}\n"
             "laLoad = setInterval(function(){ pitoune() }, 10000);\n"
             "</script>\n"
             "</div></body></html>\n";
  server.send(200, "text/html", contenu);
  Serial.println("");
  Serial.println(addy);
  Serial.println("Page logs");
  logtourne();
  logs[1] = addy;
  logs[1] += " : Page logs";
}

void handleClignote() {
  String addy = server.client().remoteIP().toString();
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere flash</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js\"></script>\n";
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n"
             "<form id=\"laforme\" action=\"/flash\" method=\"post\">\n"
             "<h1>Change la couleur de mon bureau:</h1>"
             "<input id=\"colorpad\" type=\"color\" name=\"COULEUR\" class=\"color\" value=\"" + derncoul + "\"><br>\n"
             "Fois: <input id=\"fois\" type=\"number\" name=\"FOIS\" class=\"numbers\"> "
             "Delais(ms): <input id=\"delais\" type=\"number\" name=\"DELAIS\" class=\"numbers\"><br>\n"
             "<input type=\"submit\" class=\"button1\" value=\"Changer\">\n"
             "</form>\n"
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
             "$(\"#fois\").val(Math.floor(Math.random() * 1000));\n"
             "$(\"#delais\").val(Math.floor(Math.random() * 1000));\n"
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
  Serial.println("");
  Serial.println(addy);
  Serial.println("Page flash");
  logtourne();
  logs[1] = addy;
  logs[1] += " : Page Flash";
}

// web page to handle leds fading
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
      flashoufade = 0;
    }
    if (g != vert) {
      Serial.print("Vert = ");
      Serial.println(g);
      dernadd = addy;
      flashoufade = 0;
    }
    if (b != bleu) {
      Serial.print("Bleu = ");
      Serial.println(b);
      dernadd = addy;
      flashoufade = 0;
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
  logtourne();
  logs[1] = addy;
  logs[1] += " : Fade : Couleur=";
  logs[1] += testteu;
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
}

// index page
void handleRoot() {
  String addy = server.client().remoteIP().toString();
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
             "</form>\n"
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
             " myVar = setInterval(function(){ disco() }, 18500);\n"
             "}\n"
             "</script>\n"
             "</div></body></html>\n";
  server.send(200, "text/html", contenu);
  Serial.println("");
  Serial.println(addy);
  Serial.println("Page acceuil");
  logtourne();
  logs[1] = addy;
  logs[1] += " : Acceuil";
}

//reset page
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
  logtourne();
  logs[1] = addy;
  logs[1] += " : Reset : Password=";
  logs[1] += passme;
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
  if (ouireset == 1) {
    Serial.println("Reset dans 10 secondes");
    delay(10000);
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.restart();
  }
}

// 404 not found page
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
  String uriL = server.uri();
  message += uriL;
  message += "\nMethod: ";
  String methodeL = (server.method() == HTTP_GET) ? "GET" : "POST";
  message += methodeL;
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  String argsL;
  for (uint8_t i = 0; i < server.args(); i++) {
    argsL += ": " + server.argName(i) + "=" + server.arg(i) + " ";
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  Serial.print(message);
  logtourne();
  logs[1] = addy;
  logs[1] += " : 404 : URI=";
  logs[1] += uriL;
  logs[1] += " : Methode=";
  logs[1] += methodeL;
  logs[1] += " ";
  logs[1] += argsL;
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
}

// function executed in main loop every minute to update sensors data to thingspeak.com
void latemp() {
  sensors.requestTemperatures();
  float thetemp = sensors.getTempCByIndex(0);
  char buffer[10];
  String tempC = dtostrf(thetemp, 5, 2, buffer);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (tempC != "-127.00") {
    if (tempC != "85.00") {
      Serial.print("Envoi de temperature et humidite: ");
      Serial.print(tempC);
      Serial.print(" - ");
      HTTPClient http;
      String webadd = "http://api.thingspeak.com/update?key=";
      webadd += thingkey;
      webadd += "&field1=";
      webadd += tempC;
      if (h == h) { // to prevent NaN
        webadd += "&field2=";
        webadd += h;
        Serial.print(h);
        webadd += "&field3=";
        webadd += t;
      }
      Serial.println("");
      http.begin(webadd);
      int httpCode = http.GET();//Send the request
      http.end();   //Close connection
    }
  }
}

// web page for showing sensors data
void handleTemp() {
  sensors.requestTemperatures();
  float latemp = sensors.getTempCByIndex(0);
  float h = dht.readHumidity();
  while (latemp == -127.00) {
    sensors.requestTemperatures();
    latemp = sensors.getTempCByIndex(0);
  }
  String addy = server.client().remoteIP().toString();
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
  contenu += "</h2><br>\n<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
  contenu += thingchanel;
  contenu += "/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
             "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
  contenu += thingchanel;
  contenu += "/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
             "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
  contenu += thingchanel;
  contenu += "/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
             "<input type=\"submit\" class=\"button1\" value=\"Acceuil\">\n"
             "</form>\n"
             "<br>Dernier IP &agrave; avoir chang&eacute; la couleur :";
  contenu += dernadd;
  contenu += "\n<br>";
  contenu += liens;
  contenu += "\n<br></div></body></html>\n";
  server.send(200, "text/html", contenu);
  Serial.println("");
  Serial.println(addy);
  Serial.print("Temperature : ");
  Serial.println(latemp);
  Serial.print("Humidite : ");
  Serial.println(h);
  logtourne();
  logs[1] = addy;
  logs[1] += " : Temp : Temp=";
  logs[1] += latemp;
  logs[1] += " : Humidite=";
  logs[1] += h;
}

int fadecomment(int claire) {
  int combien = 70;
  if (claire > 0)
    combien = 90;
  if (claire > 40)
    combien = 80;
  if (claire > 80)
    combien = 60;
  if (claire > 100)
    combien = 40;
  if (claire > 120)
    combien = 30;
  if (claire > 140)
    combien = 20;
  if (claire > 180)
    combien = 9;
  return combien;
}

void videCoeur(){
    for (int i = 50; i > 0; i--) {
    logs[i] = "&hearts; vide";
  }
}

void setup() {
  videCoeur();
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
  server.on("/flash", handleFlash);
  server.on("/reset", handleReset);
  server.on("/logs", handleLog);
  server.on("/log", handlePitoune);
  server.on("/temp", handleTemp);
  server.on("/party", handleClignote);
  server.on("/version", []() {
    String addy = server.client().remoteIP().toString();
    server.send(200, "text/html", "V2.2, Steve Olmstead sansillusion@gmail.com\n\n<br><br>"
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
                "        (Re-aded mDNS in 2.0 for esp32 but does not seem to work yet)\n<br>"
                "Rewrite of code (removed Ticker and changed a few libraries) for esp32 !!!\n\n<br><br>"
                "Changed fader functions for smoother fade\n<br>"
                "Cleaned code\n\n<br><br>"
                "Added Flashing function and page for smart widget usage\n<br>"
                "Added var to show your thingspeak channel in /temp iframes\n\n<br><br>"
                "Added /party page for easy control of flash mode\n\n<br><br>"
                "Added /log page to see latest ~50 visits\n\n<br><br>" + liens);
    Serial.println("");
    Serial.println(addy);
    Serial.println("Version request");
    logtourne();
    logs[1] = addy;
    logs[1] += " : Version request";
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  delay(100);
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
  if (flashoufade == 1) {
    currentMillis = millis();
    if (currentMillis - previousMillisf >= attendF) {
      flashfunk();
      previousMillisf = currentMillis;
    }
  } else {
    if (r != rouge) {
      currentMillis = millis();
      if (currentMillis - previousMillisr >= attendR) {
        fadeR();
        previousMillisr = currentMillis;
        attendR = fadecomment(rouge);
      }
    }
    if (g != vert) {
      currentMillis = millis();
      if (currentMillis - previousMillisg >= attendG) {
        fadeG();
        previousMillisg = currentMillis;
        attendG = fadecomment(vert);
      }
    }
    if (b != bleu) {
      currentMillis = millis();
      if (currentMillis - previousMillisb >= attendB) {
        fadeB();
        previousMillisb = currentMillis;
        attendB = fadecomment(bleu);
      }
    }
  }
}

