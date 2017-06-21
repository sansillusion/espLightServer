#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>
int ledPin = D7; // GPIO13
ESP8266WebServer server(80);
int value = LOW;
int roulepastusuite = 0;
int fort = 0;
String dernadd = "Fraichement boot&eacute;";
String dernoff = "Fraichement boot&eacute;";
String resetpass = "ouresetmonwifi"; // password to reset wifimanager if allready connected
Ticker fader;

void fadein() {
  if (fort < 1023) {
    fort++;
    analogWrite(ledPin, fort);
    if (fort > 80) {
      fader.attach_ms(16, fadein);
    }
    if (fort > 160) {
      fader.attach_ms(12, fadein);
    }
    if (fort > 320) {
      fader.attach_ms(8, fadein);
    }
    if (fort > 640) {
      fader.attach_ms(5, fadein);
    }
  } else {
    fader.detach();
  }
}

void fadeout() {
  if (fort > 0) {
    fort--;
    analogWrite(ledPin, fort);
    if (fort < 80) {
      fader.attach_ms(20, fadeout);
    }
    if (fort < 160) {
      fader.attach_ms(16, fadeout);
    }
    if (fort < 320) {
      fader.attach_ms(12, fadeout);
    }
    if (fort < 640) {
      fader.attach_ms(8, fadeout);
    }
  } else {
    fader.detach();
  }
}

void handleRoot() {
  String addy = server.client().remoteIP().toString();
  Serial.println("");
  Serial.println(addy);
  String testteu = server.arg("LED");
  if (testteu == "ON") {
    dernadd = addy;
    fader.attach_ms(20, fadein);
    Serial.println("LED On");
    value = HIGH;
  } else if (testteu == "OFF") {
    dernoff = addy;
    fader.attach_ms(5, fadeout);
    Serial.println("LED Off");
    value = LOW;
  }
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  if (server.args()) { // recharge la page / pour permetre d'appuyer refresh sans re-poster la form
    contenu += "<meta http-equiv=\"refresh\" content=\"0;url=/\" />\n";
  }
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere soit</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
             "<style>.button1 {\n"
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
             "</style>\n"
             "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n"
             "<form action=\"/\" method=\"post\">\n"
             "<h1>La DEL est: ";
  if (value == HIGH) {
    contenu += "On</h1>\n"
               "<input type=\"hidden\" name=\"LED\" value=\"OFF\"><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"&Eacute;teindre\">\n";
  } else {
    contenu += "Off</h1>\n"
               "<input type=\"hidden\" name=\"LED\" value=\"ON\"><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Allumer\">\n";
  }
  contenu += "</form>\n<br>Dernier IP &agrave; avoir allum&eacute; la del :";
  contenu += dernadd;
  contenu += "\n<br>Dernier IP &agrave; avoir &eacute;teint la del :";
  contenu += dernoff;
  contenu += "\n</div></body></html>\n";
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
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
             "<style>.button1 {\n"
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
             "</style>\n"
             "</head>\n<body>\n"
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
  contenu += "<br>Dernier IP &agrave; avoir allum&eacute; la del :";
  contenu += dernadd;
  contenu += "\n<br>Dernier IP &agrave; avoir &eacute;teint la del :";
  contenu += dernoff;
  contenu += "\n</div></body></html>\n";
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
  server.send(404, "text/plain", message);
}

void setup() {
  analogWriteRange(1023);
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  server.on("/version", []() {
    String addy = server.client().remoteIP().toString();
    Serial.println("");
    Serial.println(addy);
    Serial.println("Version request");
    server.send(200, "text/plain", "V1.4, Steve Olmstead sansillusion@gmail.com\n\n"
                "Added fader function\nRemoved connection watchdog (better have good signal)\n"
                "Removed mDns (did not work anyway)\n\n"
                "Added smoother fading\n\n"
                "Added AutoConect WifiManager !\n\n"
                "Added basic last IP logging\n"
                "Added /reset page to reset wifiManager connection settings");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

