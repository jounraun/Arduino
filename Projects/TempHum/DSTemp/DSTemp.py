#!/usr/bin/python

import sys
import time
import serial
from subprocess import call
from datetime import datetime

myfile = 'C:/Tmp/dstemp.txt' 
arduino = serial.Serial('COM10', 9600)
n = 0

result = arduino.readline()
result = result.strip()
print result
result = arduino.readline()
result = result.strip()
print result
result = arduino.readline()
result = result.strip()
print result
result = arduino.readline()
result = result.strip()
print result
result = arduino.readline()
result = result.strip()
print result
 
while 1:
 try:

  arduino.write('r')
  result = arduino.readline()
  result = result.strip()

  dt = datetime.now() 
  dtstr = dt.strftime('%d.%m.%Y %H:%M:%S')
   
  if n > 0 :
    print dtstr, ":", result, "C"
    f = open(myfile, 'w')
    f.write(result)
    f.close()
    time.sleep(600)

  n = n + 1
  
#  sys.exit()

 except serial.SerialTimeoutException:
  print('Data could not be read')
  time.sleep(1)
