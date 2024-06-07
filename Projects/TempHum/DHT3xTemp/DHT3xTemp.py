#!/usr/bin/python

import sys
import time
import serial
from subprocess import call
from datetime import datetime

myfile = 'C:/Tmp/DHT3xTemp.log' 
arduino = serial.Serial('COM8', 9600)
n = 0
 
while 1:
 try:

  arduino.write('r')
  result = arduino.readline()
  result = result.strip()

  dt = datetime.now() 
  dtstr = dt.strftime('%d.%m.%Y %H:%M:%S')
   
  print dtstr, ":", result

  f = open(myfile, 'a')
  f.write(result)
  f.close()
  time.sleep(10)
  
 except serial.SerialTimeoutException:
  print('Data could not be read')
  time.sleep(1)
