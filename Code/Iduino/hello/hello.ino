#include <Console.h>
//use Console class for Arduino IDE debug over Wifi, similar to Serial class,

String name;

void setup()
{
  // Initialize Console and wait for port to open:
  Bridge.begin();
  Console.begin();
  // Wait for Console port to connect
  while (!Console);
  Console.println("Hi, what's your name?");//Data flow: Arduino --> Yun Shield --> Arduino IDE
}

void loop()
{
  if (Console.available() > 0) {
    char c = Console.read(); //read the next char received, data flow: IDE --> Yun Shield--> Arduino
    // look for the newline character, this is the last character in the string
    if (c == '\n') {
      //print text with the name received
      Console.print("Hi ");
      Console.print(name);
      Console.println("! Nice to meet you!");
      Console.println();
      // Ask again for name and clear the old name
      Console.println("Hi, what's your name?");
      name = "";  // clear the name string
    } else {  // if the buffer is empty Cosole.read() returns -1
      name += c; // append the read char from Console to the name string
    }
  }
}

