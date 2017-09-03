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
<img src="https://lh3.googleusercontent.com/MuaKXzjvkwILHPCckufDm2GIoRXS9gnb1z3rfuf7Ac6mrBS8pdpfNPaG5s36WYZAbQsXCGjsQ2jOYtaOGI3kvbScvFTei8Opv5TLtoAhvuOFQmRxI7J-tnOVNc59OOs3NXQn3uK-O5edh_kPKsmtsmowJwMkyU-Jcbucw3KZ7nTRV7BK3XShO7IMmWCc8mbQN5EzS5D342-1_xiOjTmWVJ2xSkT4NkZOIIulH9dOqU6jp7wJz9Jb-dAnzRKlvahx5fIYwAr5IMrC6PEmLeysp8Eivk1Dc151RPnzlVHzklO8fY0_9rnKYDZeb33RwQW63NNlzS9eIgMsP0dvnbFCKgVn0rcvfG8MhqwHLKvYXiptuE3dquB3DuMzAPIRt409r2j5LASmGsWCRAMcQKjPtTVEnlk6J2qcDoF4xVO2v1f81VOcUXXUsGJ4GPRfov6wm2v5OZRF9E-7JFeFgrnQ_1lP5hy4d13eucjivKiYvYPa5rHk8C-6Wp4N_GvE-nXQ5WcJcIuh-cqlX6NkgnOgmA-9Rd9V8fC51d8gfHEQlV40MfMje9iCR-QBYOgu0lxWuvSnFEgYFWmW782n0fKCYL1nn6rjSDs6xgjeIYidBch0ND9q9XBAkHsox2IvjqReP4C3s85gIJXGZyJ1SDxnutObv_PFKEMoMRKbtKJ9cEWqn9o=w519-h923-no">
<br>
You configure the device for your home network<br>
<img src="https://lh3.googleusercontent.com/fjEJB7kQeErmpclVvl-V67bbcayMRFg_kpTJ3zDreL0y_0IfN9bPhfYuvUfBZ7Jz1ki4alkVY7wbGDi4m4fUWIRDLrejpft5Cgy6ASRCF68IA4RY36VVTEaKDwp9hs17ghRDyIxIPY1y-Z1NpbN3PNUCzHN_kAcofM9MsQ9zUaA6BzmNGOBK9SXKVa6z5gaYPaXYlGvVo7DgHXsTU00rGK6NCiKIL_cosnpjr7W6fGHwEcplGMjsJBJcE_e9tYFqT6d7j_Da_bRQ4USkWcV9lvunejRQ9i6SJ_eBasFkjkPEz2zmqkf4ZpKwdajIy5HM-Q86SvvisRFH58xVT8yRpNWqDqSpg0LJp8HMjTMAiQQ_0j4MXhznq-RVmzYdnjHN3ZVlfkk9890tycKcrzlnsZJsy4OT82yPFG5nK5HUWMi7T_-x16sDwOxBPMhNZDoHXOxJ5kuaKSawAJ34pcC3RHhYI4o52FpTSyoiYJ8WwErA1xaDHqkv_tpeOgZCRoOsSzLXyc67zxGrRXBJoMNcv9O7QyTlMenh107h4LZTRa-afoyFo2Mff72tAzzHSWk1WUjdO6vl62YpWEp9hpMs3mLBT_N8m8oSEr20doRO_3pkJjoPsCmVFSe0PCPygcIr01eo5uUkkOMvWxfu-SJeVT6q5mXlR5LxU5vTTa59-QkmQmM=w519-h923-no">
<br>
You save the settings and then the device tries to connect to your network<br>
<img src="https://lh3.googleusercontent.com/b6aU-hZ_YgNMPX_omZotEpIi54lKmH4yCjSl9ohiZh0y-PX2Jtcw-9SrggnPAytVcXv0EEDHxMOxBbzoR_f0eZkt1U-mT7L0HSzjFuBAcOIfa3zDKuoqbmoifH7Okkxa77NYBcjAk08koFr1k6whNUZdqXewTyjY_1C8pr20BCxDj-IEa5M4kFtMQ9qTAdxhuN6OcVhi_am8zaHP31G-NZtYun4SKkDz2Wx7NGlpQGDHY0dg3W9m7OuvWw9s4HZp1hsZ8DDDPJVJpNzL6mVrmVj2SOyzE9iQ_iL2SqZH4RS0IOzmrZzfCKnw0PxUZCAGe_WkaOqDyqu2WU4j-xz4ALOCLmAZlqR1XPYfy9beaoPKFaJ5ZyTjDxFDuEVa34rmN5haA2g2oQwQQxalnxqUJ7ZJal2wFRYsFEEzAC-aqo_t8eeBZSkD3bI1xvjihsWEg7lTUUinYcfp7iR0gv1-cIbuJRDMLZx8AivLTYGGnuEwzyqIdKam4g8XgNTH8t0zpA1UEphvMt14LEOQ89dEhrXIfTKhyM-87HTbLf-1S-lHgt28e4tTe7lrjHVLFW7wpWCW9KWo1R6lOPkY7wjR5iPTkCncaIe40dR3Wt-VZsZsHkrsD7Wmst71P34QLLnIOCgIc6iEsyqfIwUu58nU64uYgYGKPjmwsvu6HGdQE82O-54=w519-h923-no">
<br>
Make sure your phone/computer is re-connected to your home network<br>
<img stc="https://lh3.googleusercontent.com/Y_L9saJfba0ekVGjNwR0tlwfN9jmKp99BFe1FDp4F751GyTpI3b9V7DGGBxD7mR44gFM9FVARYkX9F5oqKYl7aOU-Z-6Lxm-mmvSFtDh6-WtJ0PfqBg_TzrgxIO4sA--W9GKqqOFjiPZRixp-T35XgMMFBusD7Vnz4Tr8BsEcginAxBdxce9ZxzM5_KMU5SYX3F0oInY9WinfgRapq58ctLtHTmjWCoo7qxobjvuAVKrb8noqEqB4a8BqpKsU3qQnv6fTIJJLR0y3YAKjybjqL7pNQvOCepSYC_7zSipsntfU7ZTF2-2tNnh5C-jRX14HqmGBGsf1nSKt8whcJrVtWkIqBhrorEfG_Nv2lucxELf4RfUT6vYkkAY4-NrieEYqmtdodHGOKRDZz9kMKsyXnLrayVcyi5KmdbWFr295WgpjELMPcActy5DkXfmu_P446Q0M86QE_h2MATm_1KRXmUHXcdgYfLxO4F3-CuHjYTwzix0fSq2clhspOKTdDsdyAp_kIQzz2udVt7_FzIHSj-W823paFMfcd14M3Qiv9hDWyoM6H0y6GeF2pAty0I51Oy6t61oVugqRoHakApudOt0WvUz05q2yGAHD4R2QfXw2l1dPtwgsJdb3VSVdz_m9_gn6aTHd9CNlhXLKnQCo2pBi17izYSwmIyZXaXKbcKQbTI=w519-h923-no">
<br>
You can then use <a href="http://lumiere.local">http://lumiere.local</a> to connect to your device<br>
<img src="https://lh3.googleusercontent.com/mDUCIm2T9uFS8md7spcts86DFRMCJ5-wwsW5VfaAD_2AL4WoCc84x0QfCWtPuyXflklKAucBzOYfVQkakXRNOUqkX6cS8KwWaOFDKflH8cr5-20MJZEDgyD7yhgI2daRHgtAgqmxwGmDrHO8F-9LaxcFXhRSYYFS8hl3BWSnBgoMhFGBeMTS4HGG_S4TEgP4dZPJ772YUnxW8WkeAFO5PtxB-3HUfjEfO1srlizJ1Nacz2NwOeNnLrRnRGzMcdluKMK1nf0c2KxBjUFAirNFNqRdhzWteor62jIi2LA9oIMOohfQ42BlbSzGQX-ET5MlUHjP9ZjmA9WrPIBvQloHRTXLaNn9Cgx4H8gRvaQzathOP95eu5NC5Ve3Vv_PW5mi3xDa_fztF-xp66DylJtbFFEbviOiFkAoG5645Qi3IXMr63w8lhV7wVGLqvw2i5V_T6vUzaJUMLshRTcywpT9xJZmeCu4lUh0IfQfl-zMhCSXnxaU-ibUQy-KeINgCKOwneKavXFiCJTQ4GH_zQSjto48dgXYQzb96lsPuWJeOfLABrGQzURKL-djaTnys3ToPhdOzfptTNEnchgbZIYY60YzJxrstSP2_7YpRzEGB_OIiO8dBgxcJZ__tD4m73JwozeKOmmNN0gYpd5OtieYX84YbV2xLHJNvfXcq0rZhDHCS4I=w519-h923-no">
Use the <a href="http://lumiere.local/setup">http://lumiere.local/setup</a> page to configure your thingspeak API key and channel #<br>
<img src="https://lh3.googleusercontent.com/vSXWduVz5fseYOBh6bzLLr5Fis8bk35eK-lpmUPMY1UABGkqy5HojzgJCjlh6P0JAAK9lFJp0UMgJqt8vvOjBjTZlsC3x3dNEdomeESUfU6Nbhxhr100pqq1Eo_4XQc3Ow8seDGfUVA9bQ7h3vlByy6Pwqg_KiPOqHnOzH8etJF9LDt7DiDezEDGR9vEFysrptxhEko9NJzXLzyAD6H3QdkSBk5MxZtMWVJoBwm17ynvURHuvMXGCAupNCDWsj3LLfNaxP0P5_mdZSHAIdTvveAFlNAO7hNpyHthHunVueFDp9HikYjIlqayDqOcR8CUOMa1wN7QtRobXTc8PC5GBdS2KQx47DHZVJCUVAofT93x_ukekNoPV_5jtDk2mvkHYa2kbPJaqhmyScqtTRsYik2ScYiXgWcAnJiXg0FXexGUGslluF81Z3YDSliv_s5OwT67E1syLFZJWpuLEiA8K6vSCj_-VcDN5TudJsHZcEz2_7wIH8qgmeAmWYtGAUNzOM6j7IPTGs_vtkn8pAR14OFl-lqS8tAdMuz9yZv4l-dHXqFX-oV7q8okgVBRQNfISOOWx-sr5yb349-6JknLRDcvLSU02aq-PaIbGX9PFZABdqn7zJDDdW8dNA8DHl9bv--8eNC1jg-1CZTGKGC1ZalreBq4sJSOeqUcVEnFxpedAgg=w519-h923-no">
<br>
From there you can also go to the reset page wich reset ALL settings like new<br>
<img src="https://lh3.googleusercontent.com/m02ltG1iWTE7PadCrQFcmHUNyA-oSfrZB8ezor2Lo7mEfWL3YW31QHig3Y22kL_ZPKkASwD0bh_i4j2RS8txYsCts_r5xNXRcF-fc3ZheWIallN_I2SUj8SwAZEw7xm4vlLVbapfLmf050AOIsOiBV76hPp0P8BGVo98FgtMXE8DoerUOe1v-_WxE9RBLGKn5fA1M-csrov1YQKpy26HiHqQ0OrcdqlV8A9RHghIDs9ZerxSV-o7VWh4l6nn6clMM5DiQ1FT91hswhya9IJWEOrMJEZf17auBZCVoxTCx_IJ55_k8Za0wa8YlFC-RWvCh1oBxwEYW23j2xmB4u1u7se8xdQA16FOyyq7daYlkHRoBB9UMPdSKqyBEXGkuw558X_aaHeM6qzdjd9FcBIuYKs5-27ij269kycWA0GQGlG4TfMNihBupsjd-vGOqdEHxbjkvUbxWqqzgx9GSKw3JO0BLbvfPpBvPbFG2K5RKzePchXBnLIpVeLsDklOVYTSGKoDheanm_j4Y13zTsIhNPDeDF2I2GRsSDEUqh56GhBcf4Rme-C_RLJkbVGW11Jfdm52OVPWtLtP__2ZsvUXvxzWhE0JD1TUVfwrNEEa41IiTxA-Ssw7ei9XX97zsye9DpXd7Bi-wouXipqFQEVkBwxpaCS3SD2Sz6SjdFGhNsV0MeU=w519-h923-no">
<br>
From the /setup page you also have a link to the /log page wich let's you see the latest 50 visitors<br>
<img src="https://lh3.googleusercontent.com/_54OKL-1G4P7zhUCY0GnbTOH8AkRRLOtorS23mxg2sK6JrYcHkKQRbK-eBXX6ruO3GV99X8NM_HKxGW6SXwD-pVXQkpWucDW_S8XqkL8vQvyBAFojIf7NA5suWcPnlMluX8CWqI-63NnrDEH6DAKFCkJsj7lZAEGVoZcxmTUD3rwYYcpFK1ACW6V3deFwevOjFgewNYExiwAy-dZVf-h8W0kbxJ8IduG5s-iakNztByxoiUNGBFlOY63Vn4YFEbVNIA_wHVUvakr93b83uB13rX3a4uCNlf56pavSkSVAl5EhoAS8J8mygt1z0r-rgKJ0STOpi9KjnGBD7o_PWx-Ax4_-YLx1v8oVMtmpirvUzIF8ibEIiMbA-h1ixtrHZwlgTlY-05OYlMRJUOQQhIppa7cO9IFcAF4y-XdJD4Ej4ehVhM1LlpTPxC4vEg2xhpWRaLBY-fyNMpIHjXARt4jMUi-PsYUDZNGkuzTeVMdS69OwN2yadDtpMzAgfZFaG1ZjtFTAH83wOMXHLvob2RfSTM_UYV87V7aDTkn5FR_0exvX3lcdjYU-yBj4ydkKn44atgliKjPSTEheWPRYtUDoNjaO9Ro9v1WLobvbMH_5HnPAgvMTaxDKcybWlJdwjKGp8wOAVLqt1JiSwm0haWnCqiJ5IDjDXSwC99YgStjtRU_IKU=w519-h923-no">
<br>
And the /party page wich lets you set flash color, duration in ms, and # of flashes<br>
<img src="https://lh3.googleusercontent.com/hVnDd39HfXmCuhzNYkuW7Q2F6IoLV1uLmFQR-KwHgu1UZrstSkCY2Cv5jhKxbnnSvAz5n9HyCK3fIVy-9-YOfFqv2fGGitvTTqnZQuzc4o8JCuAx5eqcMHx6P7S8fRbK8FVXUMsgjrjudQCMlN5hfj4ecLGdxvzSyJIpV9XlQwQnqSKfmIFXYNl8BobPkj7no5XYHK9TaBla0QyhmN2PuY4EETKeb49uRxzrcPvsd3Ih1ff1Ngzoo1DaP7vrPAcmhRHJPpOvagrZ3hgr-NKpxdBr4LBFs79Bxv05tH_gnCIPrIrkW-_u1fcIPDb0fFqmnPkPn-mWPTwbSvgtglV0_KC6KQVKtQKrx3D1rg0vVF76FYCOxNLmzjsX99B_6Z9LbevfsnPUWCtu-ZFGzy7D-xGXC09203x1wMfBzd9io_7RQ13c0fayRA5zNQBmTr7C97kC1uSJ0HpXIeRbooXBis3-J-El8ojx5NwPuzNMLjTEIbdCmnGB86PmENaw67APGyIsFz_iuqjLpVwRY9n54LtQquBh_rZTtV1c3PaDaNb_aZjWx2l6iaw-F80v7gE6-_JfdPPfVyebpplcbWMrfXX07UEuRsUzYwnYtNfcBIRTSycx0HnvqTZLXYZ3Y2k4lG7fgp8f5IUPD2OG5opxSDvLV09dGtYMwgw9onCLV_8IWYw=w519-h923-no">
<br>
