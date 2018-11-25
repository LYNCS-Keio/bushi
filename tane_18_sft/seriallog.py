import serial


def flightpin():
    while True:
        ser = serial.Serial('/dev/ttyS0', 9600)
        ser.flushInput()
        para_open = ser.readline()
        ser.close()
        if para_open == 'Flight Pin Open!':
            break

def servo():
    while True:
        ser = serial.Serial('/dev/ttyS0', 9600)
        ser.flushInput()
        servo_open = ser.readline()
        ser.close()
        if servo_open = 'Second Servo open!':
            break


def get_gps():
    while True:
        global c_lat, c_lng, c_dire
        ser = serial.Serial('/dev/ttyS0', 9600)
        ser.flushInput()

        #Arduinoに'1'を書き込む
        ser.write('1')
        #Arduinoからの入力を一行読み取る
        gps_data = ser.readline()
        ser.close()
        #データを3分割
        gps_data = gps_data.split(',')



        try:
            #3分割したデータをc_lat,c_lng,c_direに代入
            c_lat = float(gps_data[0])
            c_lng = float(gps_data[1])
            c_dire = float(gps_data[2])
            print("lat:lng:dire=" + str(c_lat) + " " + str(c_lng) + " " + str(c_dire) )
            with open(file,"a") as logfileobj:
                logfileobj.write('pressure:' + str(gps_data)+'\n')
                logfileobj.write('\n')
            break

        #gps_dataが文字列である場合のエラーを回避する
        except ValueError:
            pass



def get_ap():
    global ap,temp,height

    ser.write('2')
    ap_data = ser.readline()
    ap_data.split(',')

    pressure = float(gps_data[0])
    temp = float(gps_data[1])
    height = float(gps_data[2])

    with open(file,"a") as logfileobj:
        logfileobj.write('pressure:' + str(pressure)+'\n')
        logfileobj.write('temp:'+str(temp)+'\n')
        logfileobj.write('height:'+str(height)+'\n')
        logfileobj.write('\n')
