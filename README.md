# espLightServerRGB
A simple esp8266 LED strip wifi enabled controler with temperature sensor and thingspeak

uses thingspeak for temperature data logging from a (ky-001) sensor

This code requires the following dependencies :
WifiManager by tzapu
all other dependencies are built in the arduino ide

At first run it goes in wifi server mode, you use your phone or pc to connect to it's wifi
and the set it up for your home wifi configuration. then you connect to it's ip to turn on or off the LED (pin D7 on nodemcu v1)

it has four basic pages:
"/" for controling the LED strip color
"/temp" for temperature information
"/version" for version information
"/reset" to remove your wifi credetials from flash memory and reboot in wifi server mode
"/anythingelse" a basic 404 not found page

Any input are welcome !
