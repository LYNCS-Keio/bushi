import RPi.GPIO as GPIO
import time

#GPIO numbering
GPIO.setmode(GPIO.BCM)


inputA_1 = 23
inputA_2 = 24
inputB_1 = 20
inputB_2 = 21

motorA_1 = 17
motorA_2 = 27
enableA = 22
motorB_1 = 9
motorB_2 = 10
enableB = 11


GPIO.setup(inputA_1, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(inputA_2, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(inputB_1, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(inputB_2, GPIO.IN, pull_up_down = GPIO.PUD_UP)

GPIO.setup(motorA_1, GPIO.OUT)
GPIO.setup(motorA_2, GPIO.OUT)
GPIO.setup(enableA, GPIO.OUT)
GPIO.setup(motorB_1, GPIO.OUT)
GPIO.setup(motorB_2, GPIO.OUT)
GPIO.setup(enableB, GPIO.OUT)

oldA_1 = True
oldA_2 = True

oldB_1 = True
oldB_2 = True

def front(kyori):
    GPIO.output(enableA, GPIO.HIGH)
    GPIO.output(enableB, GPIO.HIGH)

    GPIO.output(motorA_1, GPIO.HIGH)
    GPIO.output(motorA_2, GPIO.LOW)

    GPIO.output(motorB_1, GPIO.HIGH)
    GPIO.output(motorB_2, GPIO.LOW)
    kyori = 1.25*kyori
    time.sleep(kyori)
    #deal_encoder(dist*643.772804)

    stop()

    time.sleep(0.3)

    GPIO.output(enableA, GPIO.HIGH)
    GPIO.output(enableB, GPIO.HIGH)

    GPIO.output(motorA_1, GPIO.HIGH)
    GPIO.output(motorA_2, GPIO.LOW)

    GPIO.output(motorB_1, GPIO.HIGH)
    GPIO.output(motorB_2, GPIO.LOW)

    time.sleep(0.2)

    stop()

    print("front" + str(kyori) + "meter\n")
    with open("logfile.txt", "a") as logfileobj:
        logfileobj.write('front:'+str(kyori)+'meter\n')

def turn_left(kakudo):
    GPIO.output(enableA, GPIO.HIGH)
    GPIO.output(enableB, GPIO.HIGH)

    GPIO.output(motorA_1, GPIO.HIGH)
    GPIO.output(motorA_2, GPIO.LOW)

    GPIO.output(motorB_1, GPIO.LOW)
    GPIO.output(motorB_2, GPIO.HIGH)

    kakudo = kakudo*1.25/360
    time.sleep(kakudo)
    #deal_encoder(deg)

    stop()

    print("turn_left" + str(kakudo) + "deg\n")
    with open("logfile.txt", "a") as logfileobj:
        logfileobj.write('turn_left'+str(kakudo)+'deg\n')

def turn_right(kakudo):
    GPIO.output(enableA, GPIO.HIGH)
    GPIO.output(enableB, GPIO.HIGH)

    GPIO.output(motorA_1, GPIO.LOW)
    GPIO.output(motorA_2, GPIO.HIGH)

    GPIO.output(motorB_1, GPIO.HIGH)
    GPIO.output(motorB_2, GPIO.LOW)

    kakudo = kakudo*1.25/360
    time.sleep(kakudo)
    #deal_encoder(deg)

    stop()

    print("turn_right" + str(kakudo) + "deg\n")
    with open("logfile.txt", "a") as logfileobj:
        logfileobj.write('turn_right'+str(kakudo)+'deg\n')

def stop():
    GPIO.output(enableA, GPIO.LOW)
    GPIO.output(enableB, GPIO.LOW)

    GPIO.output(enableA, GPIO.HIGH)
    GPIO.output(enableB, GPIO.HIGH)

    GPIO.output(motorA_1, GPIO.LOW)
    GPIO.output(motorA_2, GPIO.LOW)

    GPIO.output(motorB_1, GPIO.LOW)
    GPIO.output(motorB_2, GPIO.LOW)

    GPIO.output(enableA, GPIO.LOW)
    GPIO.output(enableB, GPIO.LOW)

    print("stop\n")
    with open("logfile.txt", "a") as logfileobj:
        logfileobj.write("stop\n")

def get_encoderA():
    global oldA_1, oldA_2
    i = 0
    newA_1 = GPIO.input(inputA_1)
    newA_2 = GPIO.input(inputA_2)

    if newA_1 != oldA_1 or newA_2 != oldA_2:
        i += 1
    oldA_1 ,oldA_2 = newA_1 ,newA_2

#    time.sleep(0.001)

    return i

def get_encoderB():
    global oldB_1, oldB_2
    j = 0
    newB_1 = GPIO.input(inputB_1)
    newB_2 = GPIO.input(inputB_2)

    if newB_1 != oldB_1:
        j += 1
    oldB_1 ,oldB_2 = newB_1 ,newB_2

#    time.sleep(0.001)

    return  j

def deal_encoder(angle):
    x = 0
    y = 0
    angle = int(angle*1.15*2)
    while True:
        changeA = get_encoderA()
        changeB = get_encoderB()
#        print(str(x) +'  '+ str(y))
        if changeA != 0 or changeB != 0:
            x += changeA
            y += changeB
            if x >= angle or y >= angle:
                stop()
                break
