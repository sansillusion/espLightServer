# esp32LightServerRGB

A simple to use esp32 LED strip, wifi enabled, controler with option for temperature sensor and thingspeak
<br>
if enabled in /setup page<br>
It uses thingspeak for temperature data logging from a (ky-001 "dallas" on pin 15) sensor and a (DHT22 on pin 5)
<br>

Default settings:<br>
Username: admin<br>
Password: admin<br>
<br>

This code requires the following dependencies :<br>
WifiManager by tzapu ( currently using bbx10 fork https://github.com/bbx10/WiFiManager/tree/esp32)<br>
WebServer by tzapu (currently using bbx10 fork https://github.com/bbx10/WebServer_tng)<br>
simpleDHT by Winlin (builtin arduino library manager)<br>
all other dependencies are built in the arduino ide
or may have been manualy added will need to clean-up when esp32 official support is out
<br>

At first run it goes in wifi server mode, you use your phone or pc to connect to it's wifi
and then set it up for your home wifi configuration. then you connect to it's ip to 
change the password (default "admin"), sensors and thingspeak settings.
<br>
Use it to set the led strip color (pin 12 (Red), 13 (Blue), 14 (green) on Wemos Lolin32 <br><img src="https://img1.banggood.com/thumb/view/oaupload/banggood/images/2A/7E/7c9a8c11-4420-4946-a25b-bf6994016020.jpg" />
<br>also tested on nodeMCU-32S
<br>
<br>
<br>
it has these basic pages:
<br>
"/" for controling the LED strip color

"/temp" for temperature information

"/version" for version information

"/reset" to remove your wifi credetials from flash memory, reset preferences and reboot in wifi server mode

"/leds" Used internally to set leds color (can also be used to create widget on phones and pc)

"/anythingelse" a basic 404 not found page

"/flash" to flash leds (POST or GET COULEUR=#FFFFFF FOIS=10 DELAIS=500)
  where COULEUR is html color code, FOIS is the number of flashes, DELAIS is the delay in ms between flash.
  
"/party" for a page with post form to flash leds
 
"/log" to see latest visitors

"/setup" to set your password, thingspeak API KEY and channel #.


Any input are welcome !<br>
You can see most of it in action here : <a href="http://008.ddns.net:8080">http://008.ddns.net:8080</a>
(/setup and /log are password locked)
