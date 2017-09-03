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


<h2>Screenshots :</h2>

At first run you connect to the device AP<br>
<img src="https://lh3.googleusercontent.com/9esbqNh3DMHfB7AhGj44R5IvAgRvviSabrC_yWQwcvkpc_WvY00G9T9wp_g3euGfw8EhRS55d2lof6VjoOdB9hh63joVqkgyE7CQvtgtHQNwjWA-Uw2mPPwM0drnLL4kkIZ2aHCJonyZl81evWFuE3_yleSjr8AK6bmjGXZCcaMIX-TMrSkQ9PtiO0OpNcooEfVugCi_5cXB7S-NQs4qGLlzGPSW-fbDX_ez2zm1NiROf-WMvIweOhnOjMgffkV8byCdGNeQN6t_oiAfrdLRsmWoxXuyref4Zz6OHNouso-kIbsc09HvIZZfDWKMzgSEZTuvKUy9qB-WKzDAC-4syEmbx6UwMuMK_tN5Pt8RPq2xRFGFb-xGmsZ4G1U2ra0MeZ8xpLP6ZzpPtELjEGEnKVS2kIGrqBLk5o3Gm8nuWiEkF5O9_572gze0JOCnLetmzwnxJ40p0EqBJlOSuzsWwxg4l-KG85h6FbahQMHLWpQNeligYzPgvLRxapcwesP3p2WscNdQJ51tWyb3KjQv-uiHpR33ri6aOXygmUw3k_Bkk6ILExCArVhiE26mhKKWdbRXhma7gvUM2ZshHT0ouTSfOViX4lpsGIvHtP7o_rSGPWakOykdzn3RyMACBuRLe2fC0dVy2sIpIEGiphzqJY2qqAIaQfdzIm7lX2ndPc8LtxI=w519-h923-no">
<br>
and get this:
<br>
<img src="https://photos.app.goo.gl/2BSxn5Z2Vq8vsMmz1">
<br>
You configure the device for your home network<br>
<img src="https://photos.app.goo.gl/4z9ecZBzFJIWrg4v2">
<br>
You save the settings and then the device tries to connect to your network<br>
<img src="https://photos.app.goo.gl/LGaA3rqBsiweauk73">
<br>
Make sure your phone/computer is re-connected to your home network<br>
<img stc="https://photos.app.goo.gl/MzvfvUlv9sLXgkI32">
<br>
You can then use <a href="http://lumiere.local">http://lumiere.local</a> to connect to your device<br>
<img src="https://photos.app.goo.gl/bvC9mtouJJyl4nyz2">
Use the <a href="http://lumiere.local/setup">http://lumiere.local/setup</a> page to configure your thingspeak API key and channel #<br>
<img src="https://photos.app.goo.gl/nSFJ4bynAaK1DyBl2">
<br>
From there you can also go to the reset page wich reset ALL settings like new<br>
<img src="https://photos.app.goo.gl/aEOdo0Jo2KZVg2jE3">
<br>
From the /setup page you also have a link to the /log page wich let's you see the latest 50 visitors<br>
<img src="https://photos.app.goo.gl/wRcfQcYr6XSPXHqg2">
<br>
And the /party page wich lets you set flash color, duration in ms, and # of flashes<br>
<img src="https://photos.app.goo.gl/OgPpP4BYzDmBD4gA2">
<br>
