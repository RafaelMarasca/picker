import cv2
import numpy as np
import serial
import calendar
import time

HEIGHT = 144#36
WIDTH = 176#36

while 1:
    list =[]

    with serial.Serial('COM5', 115200) as ser:
        ser.reset_input_buffer()
        while(len(list)<HEIGHT*WIDTH):
            list =ser.read(size = HEIGHT*WIDTH)

    y = []

    for index,element in enumerate(list):
        y.append(element)
    y = np.array(y,dtype = np.uint8)
    y = np.uint8(y.reshape((HEIGHT,WIDTH), order = 'C'))


    im = cv2.merge([y,y,y])
    cv2.namedWindow('dysplay', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('dysplay', 500,500)
    cv2.imshow('dysplay', im)
    ch = cv2.waitKey(0)
    if ch == ord('s'):
        cv2.imwrite("{}.png".format(calendar.timegm(time.gmtime())), im)
    cv2.destroyAllWindows()
