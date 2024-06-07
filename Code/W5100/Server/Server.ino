#include <SPI.h>
#include <Ethernet.h>
// #include <utility/w5100.h>
   
byte mac[] = { 0x11, 0x21, 0x32, 0x43, 0x54, 0x65 };
byte ip[] = { 192, 168, 0, 123 };
byte gateway[] = { 192, 168, 0, 254 };
byte subnet[] = { 255, 255, 255, 0 };
 
EthernetServer server(80);
 
void setup()
{  
//  pinMode(53, OUTPUT); //Uncomment this line if using a Mega
  pinMode(10, OUTPUT);
  digitalWrite(10,HIGH);

  /* Initialise the serial port */
  Serial.begin(9600);

  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
   
  Serial.println("Hello");
  
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
 
            ethernet.println("Hello World<br>");
           
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
