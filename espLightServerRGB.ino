#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS D4
String resetpass = "YourResetPassword"; // password to reset wifimanager when allready connected
const String thingkey = "YOURTHINGSPEKKEY"; //your thingspeak API key
String dernadd = "Fraichement boot&eacute;"; //variable for ip logging (you can put your fresh boot mesage here in text/html)
int pinRouge = D1; // pin for red leds
int pinVerte = D7; // pin for green leds
int pinBleu = D5; // pin for blue leds
const long interval = 60000; // interval in ms for sending temperature data to thingspeak
const String liens = "<a href=\"/\">Acceuil</a> - <a href=\"/temp\">Temp&eacute;rature</a> - <a href=\"/version\">Version</a> - <a href=\"/reset\">Reset</a> - <a href=\"https://thingspeak.com/channels/289148\">Temp stats</a>\n"; // html/links shown at bottom of pages
ESP8266WebServer server(80);
String derncoul;
long r;
long g;
long b;
int rouge = 0;
int vert = 0;
int bleu = 0;
unsigned long previousMillis = 0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int roulepastusuite = 0;
Ticker tiqueFade;

const String css = "<style>.button1 {\n"
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
                   "</style>\n";

void fadeRGB() {
  if (r == rouge && g == vert && b == bleu) {
    tiqueFade.detach();
//    Serial.println("Stopping");
  }
  for (int i = 0; i <= 2; i++) {
    int couleur;
    int tc;
    int pinouche;
    switch (i) {
      case 0:
        pinouche = pinRouge;
        tc = r;
        couleur = rouge;
        break;
      case 1:
        pinouche = pinVerte;
        tc = g;
        couleur = vert;
        break;
      case 2:
        pinouche = pinBleu;
        tc = b;
        couleur = bleu;
        break;
    }
    if (couleur < tc) {
      couleur++;
      analogWrite(pinouche, couleur);
    } else if (couleur > tc) {
      couleur--;
      analogWrite(pinouche, couleur);
    }
    switch (i) {
      case 0:
        rouge = couleur;
        break;
      case 1:
        vert = couleur;
        break;
      case 2:
        bleu = couleur;
        break;
    }
  }
}

void handleRoot() {
  String addy = server.client().remoteIP().toString();
  //  Serial.println("");
  //  Serial.println(addy);
  String testteu = server.arg("COULEUR");
  //  Serial.println(testteu);
  if (testteu != 0) {
    derncoul = testteu;
    long number = strtol( &testteu[1], NULL, 16);
    // Split them up into r, g, b values
    r = number >> 16;
    g = number >> 8 & 0xFF;
    b = number & 0xFF;
    if (r != rouge) {
      //      Serial.print("Rouge = ");
      //      Serial.println(r);
      tiqueFade.attach_ms(40, fadeRGB);
      dernadd = addy;
      delay(5);
    }
    if (g != vert) {
      //      Serial.print("Vert = ");
      //      Serial.println(g);
      tiqueFade.attach_ms(40, fadeRGB);
      dernadd = addy;
      delay(5);
    }
    if (b != bleu) {
      //      Serial.print("Bleu = ");
      //      Serial.println(b);
      tiqueFade.attach_ms(40, fadeRGB);
      dernadd = addy;
    }
  }
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  if (server.args()) { // recharge la page / pour permetre d'appuyer refresh sans re-poster la form
    contenu += "<meta http-equiv=\"refresh\" content=\"0;url=/\" />\n";
  }
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere soit</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n"
             "<form action=\"/\" method=\"post\">\n"
             "<h1>Change la couleur de mon bureau:</h1>"
             "<input type=\"color\" name=\"COULEUR\" value=\"" + derncoul + "\"><br>\n"
             "<input type=\"submit\" class=\"button1\" value=\"Changer\">\n"
             "</form>\n<br>Dernier IP &agrave; avoir chang&eacute; la couleur : ";
  contenu += dernadd;
  contenu += "<br>\n";
  contenu += liens;
  contenu += "</div></body></html>\n";
  server.send(200, "text/html", contenu);
}

void handleReset() {
  int ouireset = 0;
  String addy = server.client().remoteIP().toString();
  //  Serial.println("");
  //  Serial.println(addy);
  //  Serial.println("!!! RESET PAGE !!!");
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
    //    Serial.print("BAD PASSWORD : ");
    //    Serial.println(passme);
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
    //    Serial.println("Reset dans 10 secondes");
    delay(10000);
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.restart();
  }
}

void handleNotFound() {
  String addy = server.client().remoteIP().toString();
  //  Serial.println("");
  //  Serial.println(addy);
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
  //  Serial.print(message);
}

void updateTemp(String tempe) {
  if (tempe != "-127.00") {
    //    Serial.print("Sending temp : ");
    //    Serial.println(tempe);
    HTTPClient http;
    String webadd = "http://api.thingspeak.com/update?key=";
    webadd += thingkey;
    webadd += "&field1=";
    webadd += tempe;
    http.begin(webadd);
    int httpCode = http.GET();//Send the request
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      //      Serial.println(payload);                     //Print the response payload
    }
    http.end();   //Close connection
  }
}

void latemp() {
  sensors.requestTemperatures();
  float thetemp = sensors.getTempCByIndex(0);// Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  char buffer[10];
  String tempF = dtostrf(thetemp, 5, 2, buffer);
  updateTemp(tempF);
}

void handleTemp() {
  sensors.requestTemperatures();
  float latemp = sensors.getTempCByIndex(0);
  while (latemp == -127.00) {
    sensors.requestTemperatures();
    latemp = sensors.getTempCByIndex(0);
  }
  String addy = server.client().remoteIP().toString();
  //  Serial.println("");
  //  Serial.println(addy);
  //  Serial.print("Temperature : ");
  //  Serial.println(latemp);
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
  contenu += "&deg;C</h2><br>\n<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/289148/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15\"></iframe><input type=\"submit\" class=\"button1\" value=\"Go Home\">\n"
             "</form>\n"
             "<br>Dernier IP &agrave; avoir chang&eacute; la couleur :";
  contenu += dernadd;
  contenu += "\n<br>";
  contenu += liens;
  contenu += "\n<br></div></body></html>\n";
  server.send(200, "text/html", contenu);
}

void setup() {
  analogWriteRange(255);
  //  analogWriteFreq(500);
  //  Serial.begin(9600);
  pinMode(pinRouge, OUTPUT);
  pinMode(pinVerte, OUTPUT);
  pinMode(pinBleu, OUTPUT);
  digitalWrite(pinRouge, LOW);
  digitalWrite(pinBleu, LOW);
  digitalWrite(pinVerte, LOW);
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  delay(500);
  if (!MDNS.begin("esp8266")) {
    //Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  server.on("/temp", handleTemp);
  server.on("/version", []() {
    String addy = server.client().remoteIP().toString();
    //Serial.println("");
    //Serial.println(addy);
    //Serial.println("Version request");
    server.send(200, "text/html", "V1.7, Steve Olmstead sansillusion@gmail.com\n\n<br><br>"
                "Added fader function\nRemoved connection watchdog (better have good signal)\n<br>"
                "Removed mDns (did not work anyway)\n\n<br><br>"
                "Added smoother fading\n\n<br><br>"
                "Added AutoConect WifiManager !\n\n<br><br>"
                "Added basic last IP logging\n<br>"
                "Added /reset page to reset wifiManager connection settings\n\n<br><br>"
                "Added /temp (temperature sensor read of ky-001)\n\n<br><br>"
                "Added thingspeak support to send temperature each minute<br>\n\n<br>"
                "Major rewrite for RGB led trip usin mosfet<br>\n"
                "Removed serial to free resources for software pwm (still flikers but less than before)<br>\n" + liens);
  });
  server.onNotFound(handleNotFound);
  server.begin();
  //Serial.println("HTTP server started");
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
}

