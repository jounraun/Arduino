#!/usr/bin/python

import serial
import sys
import time
from subprocess import call
from datetime import datetime

arduino = serial.Serial('COM6', 9600)
 
while 1:
 try:

  arduino.write('r')
  time.sleep(1)
  result = arduino.readline()
  result = result.strip()

  dt = datetime.now() 
  dtstr = dt.strftime('%d.%m.%Y %H:%M:%S')
   
  print dtstr, ":", result

  time.sleep(60)
  
 except serial.SerialTimeoutException:
  print('Data could not be read')
