#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ESPmDNS.h>

Preferences preferences;

// sensors stuff
#include <Wire.h>
#include <DallasTemperature.h>
#include <SimpleDHT.h>
int pinDHT22 = 5;
SimpleDHT22 dht22;
#define ONE_WIRE_BUS 15 //dallas sensor pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// end of sensors

// led pins
uint8_t pinRouge = 12;
uint8_t pinVerte = 14;
uint8_t pinBleu = 13;
//end of led pins

// internally used vars
// Some used vars, some of these are set and loaded in setup() using esp32 Preferences
const long interval = 60000; // interval in ms for sending temperature data to thingspeak
const char* www_username = "admin";
String dernadd = "Fraichement boot&eacute;"; //variable for ip logging (you can put your fresh boot mesage here in text/html)
String resetpass = "";
String thingkey = "";
String thingchanel = "";
String liens = "";
String derncoul = "";
WebServer server(80);
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
int tstrouteur = 0;
String logs[51];
int usesenseur = 0;
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

//used to set the links at the bottom of the pages (ran at setup and on /setup changes)
void lesliens() {
  liens = "<a href=\"/\">Acceuil</a>";
  if (usesenseur == 1) {
    liens += " - <a href=\"/temp\">Temp&eacute;rature</a>";
  }
  liens += " - <a href=\"/version\">Version</a> - <a href=\"/setup\">Setup</a>";
  if (usesenseur == 1) {
    liens += " - <a href=\"https://thingspeak.com/channels/";
    liens += thingchanel;
    liens += "\">Temp stats</a>\n";
  }
}

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
    int l = i + 1;
    logs[l] = logs[i];
  }
}

// web page to handle flashing of leds (color/delay/number of flashes)
void handleLog() {
  if (!server.authenticate(www_username, string2char(resetpass))) {
    Serial.println("pas/mauvais password");
    return server.requestAuthentication();
  }
  String addy = server.client().remoteIP().toString();
  String contenu = "";
  for (int i = 1; i < 51; i++) {
    if (logs[i] != "&hearts; vide") {
      contenu += logs[i];
      contenu += "\n<br>";
    }
  }
  server.send(200, "text/plain", contenu);
  //Serial.println("");
  //Serial.println(addy);
  Serial.print(".");
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
    preferences.putString("derncoul", derncoul);
    preferences.putUInt("r", r);
    preferences.putUInt("g", g);
    preferences.putUInt("b", b);
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
  Serial.println("");
  Serial.println(addy);
  Serial.println("Page de logs");
  logtourne();
  logs[1] = addy;
  logs[1] += " : Page logs";
  if (!server.authenticate(www_username, string2char(resetpass))) {
    Serial.println("pas/mauvais password");
    return server.requestAuthentication();
  }
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
    preferences.putString("derncoul", derncoul);
    if (r != rouge) {
      Serial.print("Rouge = ");
      Serial.println(r);
      dernadd = addy;
      flashoufade = 0;
      preferences.putUInt("r", r);
    }
    if (g != vert) {
      Serial.print("Vert = ");
      Serial.println(g);
      dernadd = addy;
      flashoufade = 0;
      preferences.putUInt("g", g);
    }
    if (b != bleu) {
      Serial.print("Bleu = ");
      Serial.println(b);
      dernadd = addy;
      flashoufade = 0;
      preferences.putUInt("b", b);
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
  Serial.println("!!! PAGE DE RESET !!!");
  String testteu = server.arg("RESET");
  String passme = server.arg("PASSME");
  logtourne();
  logs[1] = addy;
  logs[1] += " : Reset : Password=";
  logs[1] += passme;
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
  if (!server.authenticate(www_username, string2char(resetpass))) {
    Serial.println("pas/mauvais password");
    return server.requestAuthentication();
  }
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
               "<h2>Tu veux VRAIEMENT resetter TOUT les r&eacute;glages (incluent les param&egrave;tres wifi)</h2>"
               "<input type=\"hidden\" name=\"RESET\" value=\"OUI\"><br>\n"
               "<input type=\"password\" name=\"PASSME\" value=\"password\"><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Reset Wifi Network\">\n"
               "</form>\n";
  } else if (ouireset != 1) {
    contenu += "<form action=\"/reset\" method=\"post\">\n"
               "<h2>Tu veux VRAIEMENT resetter TOUT les r&eacute;glages (incluent les param&egrave;tres wifi)</h2>"
               "<input type=\"hidden\" name=\"RESET\" value=\"OUI\"><br>\n"
               "<input type=\"password\" name=\"PASSME\" value=\"password\"><br>\n"
               "<h2>!!! MAUVAIS PASSWORD !!!</h2><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Reset Wifi Network\">\n"
               "</form>\n";
    Serial.print("MAUVAIS PASSWORD : ");
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
    preferences.clear();
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.restart();
  }
}

//setup page
void handleSetup() {
  String addy = server.client().remoteIP().toString();
  Serial.println("");
  Serial.println(addy);
  Serial.println("!!! PAGE DE SETUP !!!");
  int ouireset = 0;
  String testteu = server.arg("RESET");
  //  String passme = server.arg("PASSME");
  String noupass = server.arg("LAPASSE");
  String lakey = server.arg("APIKEY");
  String lechan = server.arg("CHANEL");
  String theuse = server.arg("LEUSE");
  int leuse = 0;
  if (theuse == "usemoi") {
    leuse = 1;
  } else {
    leuse = 0;
  }
  logtourne();
  logs[1] = addy;
  logs[1] += " : Settings !";
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
  if (!server.authenticate(www_username, string2char(resetpass))) {
    Serial.println("pas/mauvais password");
    logs[1] += " Not logged in !";
    return server.requestAuthentication();
  }
  if (testteu == "OUI") {
    ouireset = 1;
    if (noupass == "") {
      ouireset = 3;
    }
    if (lakey == "") {
      ouireset = 3;
    }
    if (lechan == "") {
      ouireset = 3;
    }
  }
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere se SETUP</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n";
  if (!ouireset) {
    contenu += "<form action=\"/setup\" method=\"post\">\n"
               "<h1>Changes les r&eacute;glages ici !</h1>"
               "<input type=\"hidden\" name=\"RESET\" value=\"OUI\"><br>\n"
               "Password :<br>\n<input type=\"password\" name=\"LAPASSE\" value=\"";
    contenu += resetpass;
    contenu += "\"><br>\n"
               "<br>\n<input type=\"checkbox\" name=\"LEUSE\" value=\"usemoi\" ";
    if (usesenseur == 1) {
      contenu += "checked";
    }
    contenu += ">Utilise des senseurs<br>\n"
               "ThingSpeak API Key :<br>\n<input type=\"text\" name=\"APIKEY\" value=\"";
    contenu += thingkey;
    contenu += "\"><br>\n"
               "ThingSpeak Channel # :<br>\n<input type=\"text\" name=\"CHANEL\" value=\"";
    contenu += thingchanel;
    contenu += "\"><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Changer\">\n"
               "</form>\n";
  } else if (ouireset != 1) {
    contenu += "<form action=\"/setup\" method=\"post\">\n"
               "<h1>Changes les r&eacute;glages ici !</h1>"
               "<input type=\"hidden\" name=\"RESET\" value=\"OUI\"><br>\n";
    if (ouireset == 3) {
      contenu += "<h2>!!! LES CHAMPS NE PEUVENT PAS &Ecirc;TRE VIDE !!!</h2><br>\n";
    }
    "Password :<br>\n<input type=\"password\" name=\"LAPASSE\" value=\"";
    contenu += resetpass;
    contenu += "\"><br>\n"
               "<br>\n<input type=\"checkbox\" name=\"LEUSE\" value=\"usemoi\" ";
    if (usesenseur == 1) {
      contenu += "checked";
    }
    contenu += ">Utilise des senseurs<br>\n"
               "ThingSpeak API Key :<br>\n<input type=\"text\" name=\"APIKEY\" value=\"";
    contenu += thingkey;
    contenu += "\"><br>\n"
               "ThingSpeak Channel # :<br>\n<input type=\"text\" name=\"CHANEL\" value=\"";
    contenu += thingchanel;
    contenu += "\"><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Changer\">\n"
               "</form>\n";
    if (ouireset == 3) {
      Serial.println("CHAMP VIDE");
    }
  } else if (ouireset == 1) {
    contenu += "<h1>R&eacute;glages chang&eacute;s !</h1>\n";
    lesliens();
  }
  contenu += "<br>Dernier IP &agrave; avoir chang&eacute; la couleur :";
  contenu += dernadd;
  contenu += "\n<br>";
  contenu += "\n<h3><a href=\"/reset\">Remet les r&eacute;glages &agrave; z&eacute;ro !</a></h3>";
  contenu += "\n<a href=\"/log\">Voir les derniers visiteurs</a> - <a href=\"/party\">Mode Party/Flash</a>";
  contenu += "\n<br>";
  contenu += liens;
  contenu += "\n<br></div></body></html>\n";
  server.send(200, "text/html", contenu);
  if (ouireset == 1) {
    Serial.println("Réglages changé");
    if (noupass != resetpass) {
      preferences.putString("resetpass", noupass);
      resetpass = noupass;
      Serial.print("NOUVEAU Password : ");
      Serial.println(resetpass);
    }
    if (lakey != thingkey) {
      preferences.putString("thingkey", lakey);
      thingkey = lakey;
      Serial.print("NOUVEL API KEY : ");
      Serial.println(thingkey);
    }
    if (leuse != usesenseur) {
      preferences.putUInt("usesenseur", leuse);
      usesenseur = leuse;
      lesliens();
      Serial.print("NOUVEAU use les senseurs : ");
      Serial.println(usesenseur);
    }
    if (lechan != thingchanel) {
      preferences.putString("thingchanel", lechan);
      thingchanel = lechan;
      lesliens();
      Serial.print("NOUVEAU channel : ");
      Serial.println(thingchanel);
    }
  }
}

// convert string 2 char*
char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
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
  if (thingkey != "none") {
    sensors.requestTemperatures();
    float thetemp = sensors.getTempCByIndex(0);
    char buffer[10];
    String tempC = dtostrf(thetemp, 5, 2, buffer);
    delay(400);
    float t = 0;
    float h = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht22.read2(pinDHT22, &t, &h, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT22 failed, err=");
      Serial.println(err);
      h = 0;
    }
    if (tempC != "-127.00") {
      if (tempC != "85.00") {
        Serial.print("Envoi de température et humidité: ");
        Serial.print(tempC);
        Serial.print(" - ");
        HTTPClient http;
        String webadd = "http://api.thingspeak.com/update?key=";
        webadd += thingkey;
        webadd += "&field1=";
        webadd += tempC;
        if (h != 0) {
          webadd += "&field2=";
          webadd += h;
          Serial.print(h);
          webadd += "&field3=";
          webadd += t;
        }
        Serial.println("");
        http.begin(webadd);
        int httpCode = http.GET();//Send the request
        if (httpCode > 0) {
          Serial.println("Envoit réussit !");
          tstrouteur = 0;
        } else {
          if (tstrouteur == 10) {
            Serial.println("10 Erreur envoit ! REBOOT !");
            ESP.restart();
          } else {
            Serial.println("!!! Erreur envoit !!!");
            tstrouteur++;
          }
        }
        http.end();   //Close connection
      }
    } else {
      Serial.println("Dallas error !");
    }
  } else {
    Serial.println("Pas de API KEY donc pas d'envoi");
  }
}

// web page for showing sensors data
void handleTemp() {
  sensors.requestTemperatures();
  float latemp = sensors.getTempCByIndex(0);
  delay(250);
  float t = 0;
  float h = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(pinDHT22, &t, &h, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err=");
    Serial.println(err);
  }
  String addy = server.client().remoteIP().toString();
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Quel chaleur</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n";
  contenu += "<form action=\"/\" method=\"get\">\n"
             "<h1>Voici la temp&eacute;rature</h1>"
             "<h2>";
  contenu += latemp;
  contenu += "&deg;C Humidit&eacute; : ";
  contenu += h;
  contenu += "</h2><br>\n";
  if (thingchanel != "none") {
    contenu += "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
    contenu += thingchanel;
    contenu += "/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
               "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
    contenu += thingchanel;
    contenu += "/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
               "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
    contenu += thingchanel;
    contenu += "/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>";
  } else {
    contenu += "<h2>Vas dans /setup pour les r&eacute;glages thingspeak !</h2>";
  }
  contenu += "<input type=\"submit\" class=\"button1\" value=\"Acceuil\">\n"
             "</form>\n"
             "<br>Dernier IP &agrave; avoir chang&eacute; la couleur :";
  contenu += dernadd;
  contenu += "\n<br>";
  contenu += liens;
  contenu += "\n<br></div></body></html>\n";
  server.send(200, "text/html", contenu);
  Serial.println("");
  Serial.println(addy);
  Serial.print("Température : ");
  Serial.println(latemp);
  Serial.print("Humidité : ");
  Serial.println(h);
  logtourne();
  logs[1] = addy;
  logs[1] += " : Temp : Temp=";
  logs[1] += latemp;
  logs[1] += " : Humidite=";
  logs[1] += h;
}

int fadecomment(int claire) {
  int combien = 77;
  if (claire > 0)
    combien = 77;
  if (claire > 40)
    combien = 60;
  if (claire > 80)
    combien = 40;
  if (claire > 100)
    combien = 30;
  if (claire > 120)
    combien = 20;
  if (claire > 140)
    combien = 15;
  if (claire > 180)
    combien = 9;
  return combien;
}

void videCoeur() {
  for (int i = 50; i > 0; i--) {
    logs[i] = "&hearts; vide";
  }
}

void setup() {
  Serial.begin(115200);
  preferences.begin("lumiere", false);
  videCoeur();
  ledcAttachPin(pinRouge, 1);
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
  derncoul = preferences.getString("derncoul", "#000000");
  usesenseur =  preferences.getUInt("usesenseur", 0);
  r = preferences.getUInt("r", 0);
  g = preferences.getUInt("g", 0);
  b = preferences.getUInt("b", 0);
  resetpass = preferences.getString("resetpass", "admin");
  thingkey = preferences.getString("thingkey", "none");
  thingchanel = preferences.getString("thingchanel", "none");
  lesliens();
  WiFiManager wifiManager;
  wifiManager.setTimeout(240);
  if (!wifiManager.autoConnect()) {
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  MDNS.begin("lumiere");
  server.on("/", handleRoot);
  server.on("/setup", handleSetup);
  server.on("/leds", handleLeds);
  server.on("/flash", handleFlash);
  server.on("/reset", handleReset);
  server.on("/logs", handleLog);
  server.on("/log", handlePitoune);
  server.on("/temp", handleTemp);
  server.on("/party", handleClignote);
  server.on("/version", []() {
    String addy = server.client().remoteIP().toString();
    server.send(200, "text/html", "V2.6, Steve Olmstead sansillusion@gmail.com\n\n<br><br>"
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
                "Added /log page to see latest ~50 visits\n\n<br><br>"
                "Added reboot if unable to connect to internet more than 10 times to send wheather data.\n\n<br><br>"
                "Added color persistance trough reboot using esp32 Preferences\n\n<br><br>"
                "Added /setup page to set admin password and thingspeak api key and channel\n\n<br><br>"
                "Added option in /setup tu use or not temperature sensors\n<br>"
                "Changed from Adafruit DHT library to simpleDHT for better reliability\n\n<br><br>"+ liens);
    Serial.println("");
    Serial.println(addy);
    Serial.println("Page de Version");
    logtourne();
    logs[1] = addy;
    logs[1] += " : Page de version";
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Serveur web démaré");
  delay(100);
  MDNS.addService("http", "tcp", 80);
  sensors.begin();
}

void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();
  if (usesenseur == 1) {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      latemp();
    }
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
