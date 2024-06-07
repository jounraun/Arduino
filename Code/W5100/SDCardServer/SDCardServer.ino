/* Include the serial library for communicating with the COM port */
#include <SPI.h>
 
/* Include the standard Ethernet library */
#include <Ethernet.h>
 
/* Include the standard SD card library */
#include <SD.h>
 
/* DIO pin used to control the modules CS pin */
#define SD_CARD_CD_DIO 4 
 
File SDFileData;
 
 
/* MAC address of the Ethernet shield. If you are using this on your 
own network, then the MAC address below will be fine, but remember if 
you use more than one shield on your network they will need to be assigned
unique MAC addresses */
byte mac[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
 
/* The IP address of the shield. Make sure this matches the IP 
   address range of your network and is not in use by any other 
   device on it */    
IPAddress ip(192, 168, 1, 123 );
 
/* The port number the shield will respond to. Use port 80 
   for standard HTTP requests */
EthernetServer server(80);
 
 
 
void setup()
{  
//  pinMode(53, OUTPUT); //Uncomment this line if using a Mega
  /* Start the Ethernet interface */
  Ethernet.begin(mac, ip);
  server.begin();
 
  /* DIO pin used for the CS function. Note that even if you are not driving this
     function from your Arduino board, you must still configure this as an output 
     otherwise the SD library functions will not work. */
  pinMode(10, OUTPUT);
 
  
  /* Initialise the serial port */
  Serial.begin(9600);
 
  /* Initialise the SD card */
  if (!SD.begin(SD_CARD_CD_DIO)) 
  {
    /* If there was an error output this to the serial port and go no further */
    Serial.println("ERROR: SD card failed to initialise");
    while(1);
  }else
  {
    Serial.println("SD Card OK");
  }
 
}
 
 
/* MAIN PROGRAM LOOP */
void loop()
{
  /* All client requests are terminated with a blank line. This flag will 
    signify if the current line received from this client is a blank line */
  boolean bBlankLineFlag = true;
  
  /* Used to hold the current byre received from the client */
  char cCurrentByte;
  
  /* Used to hold the current byte read from the test.txt file located on the SD card */
  char cCurrentSDByte;
    
  
  /* Wait for a request from a client */
  EthernetClient ethernet = server.available();
  
  if (ethernet) 
  {
    /* Continue to read data from the client one byte at a time until 
       there is no more data */ 
    while (ethernet.connected()) 
    {
      /* Is there still data available to be read? ethernet class 
         ethernet.connected() returns the number of bytes available */
      if (ethernet.available()) 
      {
        /* If data is available read the next byte */ 
        cCurrentByte = ethernet.read();
 
        /* Is the next byte read is a new line termination ? */      
        if (cCurrentByte == '\n')
        { 
          /* If so was it a blank line? */
          if (bBlankLineFlag) 
          {
            
            /* If it was then we can now send a response to the client’s http request... */
            ethernet.println("HTTP/1.1 200 OK");
            ethernet.println("Content-Type: text/html");
            ethernet.println();
 
            ethernet.println("<body>");
            ethernet.println("<big><span style=\"font-weight: bold;\">www.hobbycomponents.com Ethernet Shield Example</span></big><br>");
            ethernet.println("****************************************************<br>");
 
            /* ...and the append the contents of the text.txt file */
  
            SDFileData = SD.open("test.txt");
    
 
            /* Sequentially read the data from the file until there is no more data available */
            while (SDFileData.available())
            {
              cCurrentSDByte = SDFileData.read();
             
              /* Check if the current byte is a line break ASCII and if so send an HTML line break */
              if (cCurrentSDByte == '\n')
              {
                ethernet.println("<br>");
              }else
              {
                /* If not then just send it to the client */
                ethernet.print((char)cCurrentSDByte);
              }
            }
     
            /* Close the file */
            SDFileData.close();  
           
            ethernet.println("</body>");
 
            /* Disconnect from the client */
            ethernet.stop();
          }
        
          /* The last byte received from the client was the start of a 
             new line so flag as no data received for this line yet */
          bBlankLineFlag = true;
        
        /* If the last byte received wasn't a new line then it must be data... */  
        } else if (cCurrentByte != '\r')
        {  
          /* ...and so flag this as not a blank line. */
          bBlankLineFlag = false;
        }
      }
    }
  }
}
