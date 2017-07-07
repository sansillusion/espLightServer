# esp32LightServerRGB
A simple esp32 LED strip, wifi enabled, controler with temperature sensor and thingspeak

uses thingspeak for temperature data logging from a (ky-001) sensor and a ky-015

This code requires the following dependencies :
WifiManager by tzapu
all other dependencies are built in the arduino ide
or may have been manualy added will need to clean-up when esp32 official support is out

At first run it goes in wifi server mode, you use your phone or pc to connect to it's wifi
and the set it up for your home wifi configuration. then you connect to it's ip to 
change the led strip color (pin 12 (Red), 13 (Blue), 14 (green) on Wemos Lolin)

it has Six basic pages:
"/" for controling the LED strip color
"/temp" for temperature information
"/version" for version information
"/reset" to remove your wifi credetials from flash memory and reboot in wifi server mode
"/leds" Used internally to set leds color (can also be used to create widget on phones and pc)
"/anythingelse" a basic 404 not found page

Any input are welcome !
