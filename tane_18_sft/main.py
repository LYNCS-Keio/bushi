# coding: utf-8
#必要なファイルをインポート
#import camera
import serial_log
import motor
import distance
import time
import RPi.GPIO as GPIO
from math import sqrt

file = "logfile.txt"

'''
#flightpinとservoが作動するまで待機
serial_log.flightpin()
serial_log.servo()
'''
if __name__ == '__main__':
    while True :
        while True :
            gps_control()

        break

        '''
        gazo()

        judge = GPS_judge()
        if judge == True :
            print("end")
            GPIO.cleanup()

        if judge == False:
            continue
        '''

def gps_control():
    #目的地座標を代入
    dest_lat = 3533.0000
    dest_lng = 13939.0000

    print(dest_lat,dest_lng)
    #進行角を算出するため前進
    motor.front(5)
    time.sleep(5)
    serial_log.get_gps()

    #緯度方向の距離が3m以下になるとループを抜けて次のループに入る
    while distance.distance_lat(serial_log.c_lat) > 3 or distance.distance_lng(c_ lng) > 3:
        #北にいるとき
        if serial_log.c_lat > dest_lat:
            #南を向く
            if serial_log.c_dire < 180:
                motor.turn_right(180 - serial_log.c_dire)
            else:
                motor.turn_left(serial_log.c_dire - 180)

        #南にいるとき
        elif serial_log.c_lat <= dest_lat:
            #北を向く
            if serial_log.c_dire < 180:
                motor.turn_left(serial_log.c_dire)
            else:
                motor.turn_right(360 - serial_log.c_dire)

        #前進
        dist = distance_lat(serial_log.c_lat)
        if dist > 10:
            motor.front(10)
        else:
            motor.front(dist)

        time.sleep(5)

        serial_log.get_gps()



        #東にいるとき
        if serial_log.serial_log.c_lng > dest_lng:
            #西を向く
            if serial_log.c_dire < 90:
                motor.turn_left(90 + serial_log.c_dire)
            elif serial_log.c_dire > 270:
                motor.turn_left(360 - serial_log.c_dire)
            else:
                motor.turn_right(270 - serial_log.c_dire)


        #西にいるとき
        elif serial_log.serial_log.c_lng <= dest_lng:
            #東を向く
            if serial_log.c_dire < 90:
                motor.turn_right(90 - serial_log.c_dire)
            elif serial_log.c_dire > 270:
                motor.turn_right(450 - serial_log.c_dire)
            else:
                motor.turn_left(serial_log.c_dire - 90)

        #前進
        dist = distance_lng(serial_log.serial_log.c_lng)
        if dist > 10:
            motor.front(10)
        else:
            motor.front(dist)

        time.sleep(5)

        serial_log.get_gps()

    print('GPS:end')
    break

'''
def gazo():
    red_percent = camera.analize_photo()
    count += 0#スタック判定用
    while True :
        #画像中の赤色の占有率から進む距離を決定。十分近いときブレーク
        if red_percent <= それなりに遠いとき :
            motor.front(1)
            front_dist = 1
        elif 比較的近いとき <= red_percent <= 比較的近いとき :
            motor.front(0.1)
            front_dist = 0.2
        elif red_percent >= どちゃくそ近いとき:
            motor.front(0.05)
            front_dist = 0.05
            break

        #赤の占有率を移動前と移動後で分類
        last_red_percent = red_percent
        red_percent = camera.analize_photo()
        #近づいていたらこれを続ける
        if red_percent > last_red_percent :
            time.sleep(0.01)
            continue
        #通り過ぎていた場合、一旦戻ってから回転して最適な向きを調べる
        elif red_percent  < last_red_percent :
            back(front_dist)
            #回転角は180°
            for i in range(1,13):
                if i%2 == 1 :
                    turn_right(15*i)
                    last_red_percent = red_percent
                    red_percent = camera.analize_photo()
                    if red_percent > last_red_percent :
                        time.sleep(0.01)
                        continue
                elif i%2 == 0 :
                    turn_left(15*i)
                    last_red_percent = red_percent
                    red_percent = camera.analize_photo()
                    if red_percent > last_red_percent :
                        time.sleep(0.01)
                        continue
'''
