#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoOTA.h>
#include "BluetoothSerial.h"
#include <rom/rtc.h>

#define HTTP_MAX_CLOSE_WAIT 10
Preferences preferences;
BluetoothSerial SerialBT;

// led pins
const uint8_t pinRouge = 14;
const uint8_t pinVerte = 13;
const uint8_t pinBleu = 12;
//end of led pins

// sensors stuff
#include <Wire.h>
#include <DallasTemperature.h>
#include <SimpleDHT.h>
int pinDHT22 = 5;
SimpleDHT22 dht22;
#define ONE_WIRE_BUS 15 //dallas sensor pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
// end of sensors

// internally used vars
// Some used vars, some of these are set and loaded in setup() using esp32 Preferences
const long interval = 60000; // interval in ms for sending temperature data to thingspeak
const char* www_username = "admin";
String dernadd = "Fraichement boot&eacute;"; //variable for ip logging (you can put your fresh boot mesage here in text/html)
String resetpass = "admin";
String thingkey = "";
String thingchanel = "";
String liens = "";
String derncoul = "";
WebServer server(80);
int r = 0;
int g = 0;
int b = 0;
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
float dernh = 0;
float dernt = 0;
String dernd = "0";
int usesenseurdallas = 0;
int usesenseurdht = 0;
int errdallas = 0;
int errdht = 0;
String btreadString = "";
String bootString = "";
String bootStringa = "";
String bootStringb = "";
String bootip = "";

// end of internally used vars



void reboot_ip(String leip, String ledoing){
  bootip = leip;
  bootip += " fesait : <br>";
  bootip = ledoing;
  preferences.putString("bootip", bootip);
}

void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : bootString = "POWERON_RESET";break;          /**<1, Vbat power on reset*/
    case 3 : bootString = "SW_RESET";break;               /**<3, Software reset digital core*/
    case 4 : bootString = "OWDT_RESET";break;             /**<4, Legacy watch dog reset digital core*/
    case 5 : bootString = "DEEPSLEEP_RESET";break;        /**<5, Deep Sleep reset digital core*/
    case 6 : bootString = "SDIO_RESET";break;             /**<6, Reset by SLC module, reset digital core*/
    case 7 : bootString = "TG0WDT_SYS_RESET";break;       /**<7, Timer Group0 Watch dog reset digital core*/
    case 8 : bootString = "TG1WDT_SYS_RESET";break;       /**<8, Timer Group1 Watch dog reset digital core*/
    case 9 : bootString = "RTCWDT_SYS_RESET";break;       /**<9, RTC Watch dog Reset digital core*/
    case 10 : bootString = "INTRUSION_RESET";break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : bootString = "TGWDT_CPU_RESET";break;       /**<11, Time Group reset CPU*/
    case 12 : bootString = "SW_CPU_RESET";break;          /**<12, Software reset CPU*/
    case 13 : bootString = "RTCWDT_CPU_RESET";break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : bootString = "EXT_CPU_RESET";break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : bootString = "RTCWDT_BROWN_OUT_RESET";break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : bootString = "RTCWDT_RTC_RESET";break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : bootString = "NO_MEAN";
  }
}








// stylesheet for web pages
const String css = "<style>\n"
                   ".color, button, .button1{\n"
                   "border: none;\n"
                   "text-decoration: none;\n"
                   "display: inline-block;\n"
                   "cursor: pointer; -webkit-transition-duration: 0.4s;\n"
                   "transition-duration: 0.4s\n"
                   "text-align: center;\n"
                   "font-size: 24px;\n"
                   "width:80%;\n"
                   "}\n"
                   ".color {\n"
                   "height:64px;\n"
                   "padding:0px;\n"
                   "margin:0px;\n"
                   "}\n"
                   ".button1 {\n"
                   "background-color: #4CAF50;\n"
                   "color: white;\n"
                   "padding: 15px 32px;\n"
                   "margin: 4px 2px;\n"
                   "}\n"
                   "button {\n"
                   "background-color: #AC0050;\n"
                   "color: white;\n"
                   "padding: 15px 32px;\n"
                   "font-size: 24px;\n"
                   "margin: 4px 2px;\n"
                   "}\n"
                   "button:hover,.color:hover, .button1:hover {\n"
                   "box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);\n}\n"
                   "div#logs {\n"
                   "width: 100%;\n"
                   "height: 450px;\n"
                   "font-size: 12px;\n"
                   "overflow: auto;\n"
                   "text-align:left;\n"
                   "}\n"
                   "</style>\n";
// end of stylesheet for web pages

// functions used internally

//used to set the links at the bottom of the pages (ran at setup() and on /setup changes)
void lesliens() {
  int usesenseur = 0;
  if (usesenseurdallas == 1) {
    usesenseur = 1;
  }
  if (usesenseurdht == 1) {
    usesenseur = 1;
  }
  liens = "<a href=\"/\">Acceuil</a>";
  if (usesenseur == 1) {
    liens += " - <a href=\"/temp\">Temp&eacute;rature</a>";
  }
  liens += " - <a href=\"/version\">Version</a> - <a href=\"/setup\">Setup</a>";
  if (usesenseur == 1) {
    if (thingchanel != "none") {
      liens += " - <a href=\"https://thingspeak.com/channels/";
      liens += thingchanel;
      liens += "\">Temp stats</a>\n";
    }
  }
}


//Serial output (BT+SERIAL)
void logeur(String mess) {
  SerialBT.print(mess);
  delay(20);
  Serial.print(mess);
}

void logeurln(String mess) {
  SerialBT.println(mess);
  delay(20);
  Serial.println(mess);
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
    logeur("Flash finished");
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

// web page to handle logs ouput used by javascript on log page
void handleLog() {
  if (!server.authenticate(www_username, string2char(resetpass))) {
    logeurln("log no/bad password");
    return server.requestAuthentication();
  }
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "Logs");
  String contenu = "";
  for (int i = 1; i < 51; i++) {
    if (logs[i] != "&hearts; vide") {
      contenu += logs[i];
      contenu += "\n<br>";
    }
  }
  server.send(200, "text/plain", contenu);
  logeur(".");
  videCoeur();
}

// web page to handle flashing of leds (color/delay/number of flashes)
void handleFlash() {
  String sortie = "";
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "Flash");
  String testteu = server.arg("COULEUR");
  String attend = server.arg("DELAIS");
  attendF = attend.toInt();
  String flashnombre = server.arg("FOIS");
  flashfois = flashnombre.toInt();
  flashfois = flashfois * 2;
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  sortie += "Flash\n";
  sortie += testteu;
  sortie += "\n";
  sortie += "Delay - ";
  sortie += attend;
  sortie += "\n";
  sortie += "Times - ";
  sortie += flashnombre;
  sortie += "\n";
  if (attendF == 0) {
    flashfois = 0;
  }
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
    sortie += "Red = ";
    sortie += r;
    sortie += "\n";
    sortie += "Green = ";
    sortie += g;
    sortie += "\n";
    sortie += "Blue = ";
    sortie += b;
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
  logeurln(sortie);
}

// web page for flash mode
void handlePitoune() {
  String sortie = "";
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "PageLog");
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  sortie += "Logs page";
  logtourne();
  logs[1] = addy;
  logs[1] += " : Page logs";
  if (!server.authenticate(www_username, string2char(resetpass))) {
    logeurln("no/bad password");
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
  logeurln(sortie);
}

void handleClignote() {
  String sortie = "";
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "PageFlash");
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
             " myVar = setInterval(function(){ disco() }, 150000);\n"
             "}\n"
             "</script>\n"
             "</div></body></html>\n";
  server.send(200, "text/html", contenu);
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  sortie += "Flashing page";
  logtourne();
  logs[1] = addy;
  logs[1] += " : Page Flash";
  logeurln(sortie);
}

// web page to see # of sensor errors
void handleDebug() {
  String sortie = "";
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "Debug");
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  sortie += "Debug";
  sortie += "\n";
  sortie += "#ErrDallas : ";
  sortie += errdallas;
  sortie += " - #ErrDht : ";
  sortie += errdht;
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Que la lumiere Debug</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n"
             "<h1>Debug des senseurs</h1>\n"
             "Nombres d'erreurs du senseur Dallas : ";
  contenu += errdallas;
  contenu += "<br>\n"
             "Nombres d'erreurs du senseur DHT : ";
  contenu += errdht;
  contenu += "<br><br>\n";
  contenu += liens;
  contenu += "</div>\n"
             "</body></html>\n";
  server.send(200, "text/html", contenu);
  logtourne();
  logs[1] = addy;
  logs[1] += " : ErrDalas=";
  logs[1] += errdallas;
  logs[1] += " : ErrDHT=";
  logs[1] += errdht;
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
  logeurln(sortie);
}

// web page to handle leds fading
void handleLeds() {
  String sortie = "";
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "Couleur");
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  String testteu = server.arg("COULEUR");
  sortie += testteu;
  sortie += "\n";
  if (testteu != 0) {
    derncoul = testteu;
    long number = strtol( &testteu[1], NULL, 16);
    r = number >> 16;
    g = number >> 8 & 0xFF;
    b = number & 0xFF;
    preferences.putString("derncoul", derncoul);
    if (r != rouge) {
      sortie += "Red = ";
      sortie += r;
      sortie += "\n";
      dernadd = addy;
      flashoufade = 0;
      preferences.putUInt("r", r);
    }
    if (g != vert) {
      sortie += "Green = ";
      sortie += g;
      sortie += "\n";
      dernadd = addy;
      flashoufade = 0;
      preferences.putUInt("g", g);
    }
    if (b != bleu) {
      sortie += "Blue = ";
      sortie += b;
      sortie += "\n";
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
  logeur(sortie);
}

// index page
void handleRoot() {
  String sortie = "";
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "Home");
  String header;
  String referer;
  if (server.hasHeader("Referer")) {
    referer = server.header("Referer");
  } else {
    referer = "Direct ?";
  }
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
             " myVar = setInterval(function(){ disco() }, 90000);\n"
             "}\n"
             "</script>\n"
             "</div></body></html>\n";
  server.send(200, "text/html", contenu);
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  sortie += referer;
  sortie += "\n";
  sortie += "Home Page";
  logeurln(sortie);
  logtourne();
  logs[1] = addy;
  logs[1] += " : Acceuil";
  logs[1] += " : referer :";
  logs[1] += referer;
}

//reset page
void handleReset() {
  String sortie = "";
  int ouireset = 0;
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "Reset");
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  sortie += "!!! RESET PAGE !!!";
  sortie += "\n";
  String testteu = server.arg("RESET");
  String passme = server.arg("PASSMOI");
  logtourne();
  logs[1] = addy;
  logs[1] += " : Reset : Password=";
  logs[1] += passme;
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
  if (!server.authenticate(www_username, string2char(resetpass))) {
    sortie += "no/bad password";
    sortie += "\n";
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
               "<input type=\"password\" name=\"PASSMOI\" value=\"\"><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Reset Wifi Network\">\n"
               "</form>\n";
  } else if (ouireset != 1) {
    contenu += "<form action=\"/reset\" method=\"post\">\n"
               "<h2>Tu veux VRAIEMENT resetter TOUT les r&eacute;glages (incluent les param&egrave;tres wifi)</h2>"
               "<input type=\"hidden\" name=\"RESET\" value=\"OUI\"><br>\n"
               "<input type=\"password\" name=\"PASSMOI\" value=\"\"><br>\n"
               "<h2>!!! MAUVAIS PASSWORD !!!</h2><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Reset Wifi Network\">\n"
               "</form>\n";
    sortie += "BAD PASSWORD : ";
    sortie += passme;
    sortie += "\n";
  } else if (ouireset == 1) {
    contenu += "<h1>Reset Dans 10 Secondes</h1>\n";
  }
  contenu += "<br>Dernier IP &agrave; avoir chang&eacute; la couleur :";
  contenu += dernadd;
  contenu += "\n<br>";
  contenu += liens;
  contenu += "\n<br></div></body></html>\n";
  server.send(200, "text/html", contenu);
  logeur(sortie);
  if (ouireset == 1) {
    logeurln("Reset in 10 seconds");
    delay(10000);
    preferences.clear();
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    logeurln("Bip Bip... Memory erased, what is my purpose ?");
    ESP.restart();
  }
}

//setup page
void handleSetup() {
  String sortie = "";
  String addy = server.client().remoteIP().toString();
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  sortie += "!!! SETUP PAGE !!!";
  sortie += "\n";
  logtourne();
  logs[1] = addy;
  logs[1] += " : Settings !";
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
  if (!server.authenticate(www_username, string2char(resetpass))) {
    sortie += "no/bad password";
    sortie += "\n";
    logs[1] += " Not logged in !";
    return server.requestAuthentication();
  }
  reboot_ip(addy, "Setup");
  int ouireset = 0;
  String testteu = server.arg("RESET");
  String noupass = server.arg("LAPASSE");
  String lakey = server.arg("APIKEY");
  String lechan = server.arg("CHANEL");
  String thedallas = server.arg("LEDALLAS");
  String thedht = server.arg("LEDHT");
  int leusedallas = 0;
  int leusedht = 0;
  if (thedallas == "usedallas") {
    leusedallas = 1;
  }
  if (thedht == "usedht") {
    leusedht = 1;
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
  if (ouireset == 1) {
    sortie += "Settings changed";
    sortie += "\n";
    if (noupass != resetpass) {
      preferences.putString("resetpass", noupass);
      resetpass = noupass;
      sortie += "NEW Password : ";
      sortie += resetpass;
      sortie += "\n";
    }
    if (lakey != thingkey) {
      preferences.putString("thingkey", lakey);
      thingkey = lakey;
      sortie += "NEW API Key : ";
      sortie += thingkey;
      sortie += "\n";
    }
    if (leusedallas != usesenseurdallas) {
      preferences.putUInt("usesenseurdal", leusedallas);
      usesenseurdallas = leusedallas;
      lesliens();
      sortie += "NEW dallas setting : ";
      sortie += usesenseurdallas;
      sortie += "\n";
    }
    if (leusedht != usesenseurdht) {
      preferences.putUInt("usesenseurdht", leusedht);
      usesenseurdht = leusedht;
      lesliens();
      sortie += "NEW dht setting : ";
      sortie += usesenseurdht;
      sortie += "\n";
    }
    if (lechan != thingchanel) {
      preferences.putString("thingchanel", lechan);
      thingchanel = lechan;
      lesliens();
      sortie += "NEW channel : ";
      sortie += thingchanel;
      sortie += "\n";
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
               "<br>\n<input type=\"checkbox\" name=\"LEDALLAS\" value=\"usedallas\" ";
    if (usesenseurdallas == 1) {
      contenu += "checked";
    }
    contenu += ">Utilise un senseur Dallas (field1 sur Thingspeak)"
               "<br>\n<input type=\"checkbox\" name=\"LEDHT\" value=\"usedht\" ";
    if (usesenseurdht == 1) {
      contenu += "checked";
    }
    contenu += ">Utilise un senseur DHT22 (field2 et 3 sur Thingspeak)<br>\n"
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
    contenu += "Password :<br>\n<input type=\"password\" name=\"LAPASSE\" value=\"";
    contenu += resetpass;
    contenu += "\"><br>\n"
               "<br>\n<input type=\"checkbox\" name=\"LEDALLAS\" value=\"usedallas\" ";
    if (usesenseurdallas == 1) {
      contenu += "checked";
    }
    contenu += ">Utilise un senseur Dallas (field1 sur Thingspeak)"
               "<br>\n<input type=\"checkbox\" name=\"LEDHT\" value=\"usedht\" ";
    if (usesenseurdht == 1) {
      contenu += "checked";
    }
    contenu += ">Utilise un senseur DHT22 (field2 et 3 sur Thingspeak)<br>\n"
               "ThingSpeak API Key :<br>\n<input type=\"text\" name=\"APIKEY\" value=\"";
    contenu += thingkey;
    contenu += "\"><br>\n"
               "ThingSpeak Channel # :<br>\n<input type=\"text\" name=\"CHANEL\" value=\"";
    contenu += thingchanel;
    contenu += "\"><br>\n"
               "<input type=\"submit\" class=\"button1\" value=\"Changer\">\n"
               "</form>\n";
    if (ouireset == 3) {
      sortie += "EMPTY FIELD";
      sortie += "\n";
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
  logeur(sortie);
}

// convert string 2 char*
char* string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char*>(command.c_str());
    return p;
  }else{
    return 0;
  }
}

// 404 not found page
void handleNotFound() {
  String sortie = "";
  String header;
  String addy = server.client().remoteIP().toString();
  sortie += "\n";
  String userAgent;
  sortie += addy;
  sortie += "\n";
  if (server.hasHeader("User-Agent")) {
    userAgent = server.header("User-Agent");
  } else {
    userAgent = "Pas de userAgent ?";
  }
  sortie += userAgent;
  sortie += "\n";
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
  reboot_ip(addy, uriL);
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
  logtourne();
  logs[1] = addy;
  logs[1] += " : 404 : URI=";
  logs[1] += uriL;
  logs[1] += " : UserAgent=";
  logs[1] += userAgent;
  logs[1] += " : Methode=";
  logs[1] += methodeL;
  logs[1] += " ";
  logs[1] += argsL;
  logs[1].replace("<", "</"); //prevent html tags in logs so hackers cant code in log page lol
  sortie += message;
  logeur(sortie);
}

// function executed in main loop every minute to update sensors data to thingspeak.com
void latemp() {
  reboot_ip("Internal", "TempSend");
  String sortie = "";
  if (thingkey != "none") {
    HTTPClient http;
    String webadd = "http://api.thingspeak.com/update?key=";
    webadd += thingkey;
    sortie += "\n";
    sortie += "Sending temp and humidity readings: ";
    int enweille = 0;
    if (usesenseurdallas == 1) {
      //dallas
      sensors.requestTemperatures();
      delay(10);
      sensors.requestTemperatures();
      float thetemp = sensors.getTempCByIndex(0);
      char buffer[10];
      String tempC = dtostrf(thetemp, 5, 2, buffer);
      //      delay(400);
      if (tempC != "-127.00") {
        if (tempC != "85.00") {
          sortie += tempC;
          if (usesenseurdht == 1) {
            sortie += " - ";
          }
          webadd += "&field1=";
          webadd += tempC;
          enweille = 1;
          dernd = tempC;
        } else {
          sortie += "Dallas sensor error 85 !";
          sortie += "\n";
          errdallas++;
          if (dernd != "0") {
            tempC = dernd;
            webadd += "&field1=";
            webadd += tempC;
            enweille = 1;
            sortie += tempC;
            if (usesenseurdht == 1) {
              sortie += " - ";
            }
          } else {
            enweille = 0;
          }
        }
      } else {
        sortie += "Dallas sensor error -127 !";
        sortie += "\n";
        errdallas++;
        if (dernd != "0") {
          tempC = dernd;
          webadd += "&field1=";
          webadd += tempC;
          enweille = 1;
          sortie += tempC;
          if (usesenseurdht == 1) {
            sortie += " - ";
          }
        } else {
          enweille = 0;
        }
      }
    }
    if (usesenseurdht == 1) {
      //dht
      float t = 0;
      float h = 0;
      int err = SimpleDHTErrSuccess;
      if ((err = dht22.read2(pinDHT22, &t, &h, NULL)) != SimpleDHTErrSuccess) {
        errdht++;
        sortie += "\n";
        sortie += "DHT sensor error, err=";
        sortie += err;
        sortie += "\n";
        if (dernh != 0) {
          h = dernh;
          t = dernt;
          webadd += "&field2=";
          webadd += h;
          sortie += h;
          webadd += "&field3=";
          webadd += t;
          sortie += " - ";
          sortie += t;
          enweille = 1;
          dernt = t;
          dernh = h;
        }
      } else {
        webadd += "&field2=";
        webadd += h;
        sortie += h;
        webadd += "&field3=";
        webadd += t;
        sortie += " - ";
        sortie += t;
        enweille = 1;
        dernt = t;
        dernh = h;
      }
    }
    if (enweille == 1) {
      sortie += "\n";
      http.begin(webadd);
      int httpCode = http.GET();
      if (httpCode > 0) {
        sortie += "Sending successfull !";
        sortie += "\n";
        tstrouteur = 0;
      } else {
        if (tstrouteur == 10) {
          logeurln("10 errors while sending ! REBOOT !");
          ESP.restart();
        } else {
          sortie += "!!! Error while sending !!!";
          sortie += "\n";
          tstrouteur++;
        }
      }
      http.end();   //Close connection
    } else {
      sortie += "Not sending due to sensors error !";
      sortie += "\n";
    }
  } else {
    sortie += "Not sending since there is no API key set !";
    sortie += "\n";
  }
  logeur(sortie);
}

// web page for showing sensors data
void handleTemp() {
  String sortie = "";
  String addy = server.client().remoteIP().toString();
  reboot_ip(addy, "Temp");
  sortie += "\n";
  sortie += addy;
  sortie += "\n";
  sortie += "Temperature page";
  sortie += "\n";
  float latemp = 0;
  float t = 0;
  float h = 0;
  String tempC;
  if (usesenseurdallas == 1) {
    //sensors.requestTemperatures();
    //delay(10);
    sensors.requestTemperatures();
    latemp = sensors.getTempCByIndex(0);
    char buffer[10];
    tempC = dtostrf(latemp, 5, 2, buffer);
    if (tempC != "-127.00") {
      if (tempC != "85.00") {
        dernd = tempC;
      } else {
        errdallas++;
        sortie += "Dallas sensor error 85 !";
        sortie += "\n";
        if (dernd != "0") {
          tempC = dernd;
        }
      }
    } else {
      sortie += "Dallas sensor error -127 !";
      sortie += "\n";
      errdallas++;
      if (dernd != "0") {
        tempC = dernd;
      }
    }
  }
  if (usesenseurdht == 1) {
    t = 0;
    h = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht22.read2(pinDHT22, &t, &h, NULL)) != SimpleDHTErrSuccess) {
      errdht++;
      sortie += "DHT sensor error, err=";
      sortie += err;
      sortie += "\n";
      t = dernt;
      h = dernh;
    }
    dernt = t;
    dernh = h;
  }
  String contenu = "<!DOCTYPE html>\n<html lang=\"en\" dir=\"ltr\" class=\"client-nojs\">\n<head>\n";
  contenu += "<meta charset=\"UTF-8\" />\n<title>Quel chaleur</title>\n"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  contenu += css;
  contenu += "</head>\n<body>\n"
             "<div style=\"text-align:center;width:100%;\">\n";
  contenu += "<form action=\"/\" method=\"get\">\n"
             "<h1>Voici la temp&eacute;rature</h1>"
             "<h2>";
  if (usesenseurdallas == 1) {
    contenu += tempC;
    contenu += "&deg;C";
  }
  if (usesenseurdht == 1) {
    if (usesenseurdallas == 1) {
      contenu += " - ";
    }
    contenu += t;
    contenu += "&deg;C - Humidit&eacute; : ";
    contenu += h;
    contenu += "&#37;";
  }
  contenu += "</h2><br>\n";
  if (thingchanel != "none") {
    if (usesenseurdallas == 1) {
      contenu += "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
      contenu += thingchanel;
      contenu += "/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>";
    }
    if (usesenseurdht == 1) {
      contenu += "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
      contenu += thingchanel;
      contenu += "/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>"
                 "<iframe width=\"450\" height=\"260\" style=\"border: 1px solid #cccccc;\" src=\"https://thingspeak.com/channels/";
      contenu += thingchanel;
      contenu += "/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=30\"></iframe>";
    }
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
  if (usesenseurdallas == 1) {
    sortie += "Temp : ";
    sortie += latemp;
    sortie += "C";
    sortie += "\n";
  }
  if (usesenseurdht == 1) {
    sortie += "Temp2 : ";
    sortie += t;
    sortie += "C";
    sortie += "\n";
    sortie += "Humidity : ";
    sortie += h;
    sortie += "%";
    sortie += "\n";
  }
  logtourne();
  logs[1] = addy;
  if (usesenseurdallas == 1) {
    logs[1] += " : Temp : Temp=";
    logs[1] += latemp;
  }
  if (usesenseurdht == 1) {
    logs[1] += " : Humidite=";
    logs[1] += h;
    logs[1] += " : Temp2=";
    logs[1] += t;
  }
  logeur(sortie);
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

void lumiereloop() {
  unsigned long currentMillis = millis();
  if (flashoufade == 1) {
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

void loop1(void *pvParameters) {
  while (1) {
    if ( r != rouge || g != vert || b != bleu || flashoufade != 0 ) {
      lumiereloop();
      vTaskDelay( 10 / portTICK_PERIOD_MS ); // wait / yield time to other tasks
    } else {
      vTaskDelay( 100 / portTICK_PERIOD_MS ); // wait / yield time to other tasks
    }
  }
}

void ladentbleu() {
  while (SerialBT.available()) {
    vTaskDelay( 60 / portTICK_PERIOD_MS ); // wait / yield time to other tasks
    if (SerialBT.available() > 0) {
      char c = SerialBT.read();
      btreadString += c;
    }
  }
}

void setup() {
  //Serial.begin(115200);
  SerialBT.begin("lumiere"); //Bluetooth device name
  print_reset_reason(rtc_get_reset_reason(0));
  bootStringa = bootString;
  print_reset_reason(rtc_get_reset_reason(1));
  bootStringb = bootString;
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
  usesenseurdallas = preferences.getUInt("usesenseurdal", 0);
  usesenseurdht = preferences.getUInt("usesenseurdht", 0);
  r = preferences.getUInt("r", 0);
  g = preferences.getUInt("g", 0);
  b = preferences.getUInt("b", 0);
  resetpass = preferences.getString("resetpass", "admin");
  thingkey = preferences.getString("thingkey", "none");
  thingchanel = preferences.getString("thingchanel", "none");
  xTaskCreatePinnedToCore(loop1, "loop1", 1024, NULL, 0, NULL, 0);
  lesliens();
  delay(500);
  WiFiManager wifiManager;
  wifiManager.setTimeout(240);
  //WiFi.disconnect(); // pour prevenir de bugs de power et autres
  delay(1000);
  if (!wifiManager.autoConnect()) {
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  delay(500);
  if (!sensors.getAddress(insideThermometer, 0)) {
    int ereur = 0;
    int marchetu = 1;
    do { // 5 retry
      ereur++;
      Serial.print("Sensor error try #");
      Serial.println(ereur);
      delay(200);
      if (sensors.getAddress(insideThermometer, 0)) {
        ereur = 6;
        marchetu = 0;
      }
    } while (ereur < 5);
    if (marchetu != 0) {
      dernadd = "Erreur de senseur !";
      Serial.println("Error ! Sensor not working !");
    }
  }
  sensors.setResolution(insideThermometer, 12);// 9 ou 12 9 plus rapide mais moin prÃ©Ã§is
  MDNS.begin("lumiere");
  ArduinoOTA.setHostname("lumiere");
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  server.on("/", handleRoot);
  server.on("/setup", handleSetup);
  server.on("/leds", handleLeds);
  server.on("/flash", handleFlash);
  server.on("/reset", handleReset);
  server.on("/logs", handleLog);
  server.on("/log", handlePitoune);
  server.on("/temp", handleTemp);
  server.on("/debug", handleDebug);
  server.on("/party", handleClignote);
  server.on("/version", []() {
    String sortie = "";
    String addy = server.client().remoteIP().toString();
    reboot_ip(addy, "Version");
    server.send(200, "text/html", "V3.2, Steve Olmstead sansillusion@gmail.com\n\n<br><br>"
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
                "Changed from Adafruit DHT library to simpleDHT for better reliability\n\n<br><br>"
                "Changed /setup (removed use sensors and added use dht and use dallas checkboxes)\n<br>"
                "Revamped /temp and lesliens() to accomodate new changes in setup\n\n<br><br>"
                "Added User-Agernt information in logging and serial output of / and 404 not found page\n<br>"
                "Fixed Bonjour service support you can now use ( <a href=\"http://lumiere.local\">http://lumiere.local</a> ) if you have Bonjour V3 installed\n<br><br>"
                "Moved fading to it's own core and loop<br><br>\n"
                "Cleaned CSS and changed color selector to match buttons<br><br>\n\n"
                "Added ArduinoOTA support for easy updates<br><br>\n\n"
                "Added Bluetooth Serial logging and color control<br>\n\n"
                "<br>\n" + liens);
    sortie += "\n";
    sortie += addy;
    sortie += "\n";
    sortie += "Version page";
    sortie += "\n";
    logtourne();
    logs[1] = addy;
    logs[1] += " : Page de version";
    logeur(sortie);
  });
  server.onNotFound(handleNotFound);
  const char * headerkeys[] = {"User-Agent", "Referer"} ;//peut ajouter autres si besoin eg {"User-Agent", "Cookie"}
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
  Serial.println("Web server started visit http://lumiere.local");
  MDNS.addService("_http", "_tcp", 80);
  ArduinoOTA.begin();
  delay(100);
  bootip = preferences.getString("bootip", "");
  dernadd = bootStringa;
  dernadd += " - ";
  dernadd += bootStringb;
  dernadd += "<br>";
  dernadd += bootip;
}

void loop() {
  if (SerialBT.available()) {
    String sortie = "";
    btreadString = "";
    ladentbleu();
    String lacmd = btreadString.substring(0, 1);
    if (lacmd == "#") {
      String execca = btreadString.substring(0, 7);
      if (execca != 0) {
        derncoul = execca;
        long number = strtol( &execca[1], NULL, 16);
        r = number >> 16;
        g = number >> 8 & 0xFF;
        b = number & 0xFF;
        preferences.putString("derncoul", derncoul);
        if (r != rouge) {
          sortie += "Red = ";
          sortie += r;
          sortie += "\n";
          dernadd = "BT";
          flashoufade = 0;
          preferences.putUInt("r", r);
        }
        if (g != vert) {
          sortie += "Green = ";
          sortie += g;
          sortie += "\n";
          dernadd = "BT";
          flashoufade = 0;
          preferences.putUInt("g", g);
        }
        if (b != bleu) {
          sortie += "Blue = ";
          sortie += b;
          sortie += "\n";
          dernadd = "BT";
          flashoufade = 0;
          preferences.putUInt("b", b);
        }
      }
    }
    logeur(sortie);
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int usesenseur = 0;
    if (usesenseurdallas == 1) {
      usesenseur = 1;
    }
    if (usesenseurdht == 1) {
      usesenseur = 1;
    }
    if (usesenseur == 1) {
      latemp();
    }
  }
  server.handleClient();
  vTaskDelay( 60 / portTICK_PERIOD_MS ); // wait / yield time to other tasks
  ArduinoOTA.handle();
}
