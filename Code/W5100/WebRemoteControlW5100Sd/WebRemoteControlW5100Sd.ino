//Web Remote control with Arduino board and ethernet card Wiznet w5100 with SD card
// January 20, 2012 with Arduino software 0022.

//***************This code created with Arduino software 0022**************
//***************Include inside folder libraries the below libraries*************
//***************I verify , compile , load and work with arduino 0022***********
//***************Maybe you have errors with arduino 1.0.1***************
 
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <PulsesIR_Toyotomi.h>//My library for Toyotomi IR signals

PulsesIR_Toyotomi pulsesIR_Toyotomi(2);// 2 is pin for IR led 

File myFile;

#define maxLength 40

String inString = String(maxLength);       // allocate a new String


//const byte IR_PIN=2;//Ir led pin

const byte LEDpin=3;//Indicator

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192,168,2,15};//Setup your local ip
const byte Port = 80;

Server server(Port);

int counter=0;
char c;
word xClicksProcessed=0;
char codeOK='0';//start Code is blank....

void setup()
{
   // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.begin(9600);//Restore, if you want to monitor program's
       //operation via serial monitor
 
   Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
   pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  //pinMode(IR_PIN,OUTPUT);
  //digitalWrite(IR_PIN,LOW);
  pinMode(LEDpin,OUTPUT);
  LEDOff();
 
}

void loop()
{
 
  Client client = server.available();
    
     if (client) {//2
    boolean currentLineIsBlank = true;//3
    counter=0;
    inString="";//format inString
     while (client.connected()) {//4
      if (client.available()) {//45
         c = client.read();//6
         
          
         if ((c!='\n') && (c!='\r')){
         if (inString.length() < maxLength) {
              inString+=c;
         }        
         }
         // Serial.print(c);
         // Serial.print(inString);
         if (c == '\n' && currentLineIsBlank ) {
           delay(300);
           xClicksProcessed++;
           
           
           client.println("HTTP/1.1 200 OK");//8
            client.println("Content-Type: text/html");//9
            client.println();//10

             myFile = SD.open("index.htm");
  if (myFile) {
    Serial.println("index.htm:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
        client.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
      // if the file didn't open, print an error:
    Serial.println("error opening index.htm");
  }

            client.print("<br>Analog input A3 (Temperature) reads: <b>");//Temperature sensor Lm35
            client.print(analogRead(3)*0.488);
            client.println("</b>");

            client.print("<p>Clicks since server booted: ");
            client.print(xClicksProcessed);
            client.println("</p>");
            
            //This is code inside index.htm into sd card without client.println
            //****************Put this code inside SD card into a file with name index.htm**************************///
            //**************** ..................................................................................... ************************///
/*
          <html>
            <head>
             <title>Web Remote Control</title>
              </head>
              </head>
            
            <body>

            <center>
            <h2>Web Remote Control A/C with Arduino and W5100 ver.22/12/2011
            
            </h2>

            
            <p>----

            <!--br>Analog input A3 (Temperature) reads: <b>
            <var>analogRead(3)*0.488</var>
            </b-->


          
             
              <form METHOD=get action="">
              <input type=password name="code" size="4" maxlength="4">
              <input type=submit name="Submit" value="Code">
              </form>
              
              
              
              <form METHOD=get action="">
              <input type=hidden name="cmd" value="AC_ON">
              <input type=submit value="AC On">
              </form>
              

              <form METHOD=get action="">
              <input type=hidden name="cmd" value="AC_OFF">
              <input type=submit value="AC Off">
              </form>
              
              
              <form METHOD=get action="">
              <select name="cmd">
              <option value="COOL" selected="selected" >Cool</option>
              <option value="HEAT">Heat</option>
              <option value="DRY">Dry</option>
              <option value="FAN">Fan</option>
              <option value="AUTO">Auto</option>
              </select>
              <input type="submit" name="Submit" value="Mode">
              </form>
              
            
              
              <form METHOD=get action="">
              <select name="cmd">
              <option value="temp30" selected="selected" >Temp 30</option>
              <option value="temp29">Temp 29</option>
              <option value="temp28">Temp 28</option>
              <option value="temp27">Temp 27</option>
              <option value="temp26">Temp 26</option>
              <option value="temp25">Temp 25</option>
              <option value="temp24">Temp 24</option>
              <option value="temp23">Temp 23</option>
              <option value="temp22">Temp 22</option>
              <option value="temp21">Temp 21</option>
              <option value="temp20">Temp 20</option>
              <option value="temp19">Temp 19</option>
              <option value="temp18">Temp 18</option>
              <option value="temp17">Temp 17</option>
              <option value="temp16">Temp 16</option>
              </select>
              
              <input type="submit" name="Submit" value="Temperature">
              </form>
              
              
              <form METHOD=get action="">
              <select name="cmd">
              <option value="Fan_Low" selected="selected" >Fan low</option>
              <option value="Fan_Medium">Fan medium</option>
              <option value="Fan_High">Fan high</option>    
              <option value="Fan_Auto">Fan Auto</option>
              </select>
              
              <input type="submit" name="Submit" value="Fan speed">
              </form>
              
             
     

              <!--p>Clicks since server booted:
              <var>xClicksProcessed</var>
              </p-->

          <br>

          
          <center>         

           */
//********************End of html inside SD card********************************/

          break;//15
         }
         
         if (c == '\n') {//17
          currentLineIsBlank = true;//18
          
          Serial.print(inString);
          
          if (inString.startsWith("GET")>0) {//19
            
            if  (inString.startsWith("GET /?code=xxxx")>0) {//Set the code
                     codeOK='1';
                     Serial.println("codeOK=");
                     Serial.print(codeOK);
            }
            
            if ((codeOK=='1') && (counter <1)){counter++;
              if (inString.startsWith("GET /?cmd=AC_ON")>0 ){
                   LEDOn();
                   pulsesIR_Toyotomi.SendACCodeOn();
                  
                   //Serial.println("codeOK=");
                   //Serial.print(codeOK);
                   //Serial.print(inString);
                   
              }
              if (inString.startsWith("GET /?cmd=AC_OFF")>0){
                   LEDOff();
                   pulsesIR_Toyotomi.SendACCodeOff();
              }
              
              if (inString.startsWith("GET /?cmd=COOL")>0){
               
                 pulsesIR_Toyotomi.SendACCode_modeCool();                       
              }
              if (inString.startsWith("GET /?cmd=HEAT")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_modeHeat();
              }
              if (inString.startsWith("GET /?cmd=DRY")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_modeDry();
              }
              if (inString.startsWith("GET /?cmd=AUTO")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_modeAuto();
              }
              if (inString.startsWith("GET /?cmd=FAN")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_modeFan();
              }
               if (inString.startsWith("GET /?cmd=Fan_Low")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_modeFanspeedLow();
              }
              if (inString.startsWith("GET /?cmd=Fan_Medium")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_modeFanspeedMedium();
              }
              if (inString.startsWith("GET /?cmd=Fan_High")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_modeFanspeedHigh();
              }
              if (inString.startsWith("GET /?cmd=Fan_Auto")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_modeFanspeedAuto();
              }
              if (inString.startsWith("GET /?cmd=temp30")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_30();
              }
              if (inString.indexOf("cmd=temp29")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_29();
              }
              if (inString.indexOf("cmd=temp28")>0){
                   
                 //pulsesIR_Toyotomi.SendACCode_Temp_28();
              }
             if (inString.indexOf("cmd=temp27")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_27();
              }
              if (inString.indexOf("cmd=temp26")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_26();
              }
              if (inString.indexOf("cmd=temp25")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_25();
              }
              if (inString.indexOf("cmd=temp24")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_24();
             }
              if (inString.startsWith("cmd=temp23")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_23();
              }
              if (inString.indexOf("cmd=temp22")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_22();
              }
              if (inString.indexOf("cmd=temp21")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_21();
              }
              if (inString.indexOf("cmd=temp20")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_20();
              }
              if (inString.indexOf("cmd=temp19")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_19();
              }
              if (inString.indexOf("cmd=temp18")>0){
                   
                 //pulsesIR_Toyotomi.SendACCode_Temp_18();
              }
              if (inString.indexOf("cmd=temp17")>0){
                   
                 //pulsesIR_Toyotomi.SendACCode_Temp_17();
              }
              if (inString.indexOf("cmd=temp16")>0){
                   
                 pulsesIR_Toyotomi.SendACCode_Temp_16();
              }
              
            }//codeok
          }//get
         }//current line
                    
               else if (c != '\r') {        //20
          currentLineIsBlank = false;//21
               }
         }//client avalaible
      }
      delay(1);//25
    client.stop();//26
  }//27

}//28

void LEDOn(){
digitalWrite(LEDpin,HIGH);
}

void LEDOff(){
digitalWrite(LEDpin,LOW);
}
