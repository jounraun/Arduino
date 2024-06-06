============
Installation
============

FullIP for Arduino
 
FullIP is a suite of ip protocols implementation for Arduino.

 
- Download the library from github : https://github.com/bibi21000/arduino-fullip.

- Install it in your libraries. Rename the directory to FullIP.

- If you want to use FullIP with enc28j60, install the UIPEthernet library : https://github.com/ntruchsess/arduino_uip.

- Restart Arduino IDE.

- Update FullIP.h according to your hardware :

  - comment #define FULLIP_UIP to build with official shield (w5100) or
    uncomment it to build with enc28j60 and UIPEthernet.
    
  - comment #define FULLIP_SD to disable SD or
    uncomment it to build ftpclient, httpserver, ...
    
- Import it in your sketch :

- import the Ethernet library or the UIPEthernet and the SD if needed :

.. code-block:: c

 // For W5100
 //You must also update FullIP/FullIP.h
 #include <SPI.h>
 #include <Ethernet.h> 
 //For enc28j60
 //You must also update FullIP/FullIP.h
 //#include <UIPEthernet.h>
 #include <SD.h>
 #include <FullIP.h>

- The pin CS are hardcoded in UIPEthernet and Ethernet libraires. Update the
  followings files if needed :
  
  - UIPEthernet/Utility/Enc28J60Network.h : 
  
  - arduino-1.x/libraries/Ethernet/Utility/w5100.h (to do ...)
