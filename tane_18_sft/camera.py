#必要なファイルをインポート
import cv2
import numpy as np
import picamera
import time

def analize_photo() :
    #解像度の指定
    resolution_x = 320
    resolution_y = 240

    savefolder = ""#ここは後で指定(保存先のフォルダ)

    #カメラの初期化
    cam = picamera.PiCamera()
    cam.resolution(resolution_x, resolution_y)

    #保存ファイルの指定
    filename = time.strftime("%Y%m%d%H%M%S")+".jpg"
    savefile = savefolder + filename

    #撮影し保存
    cam.capture(savefile)

    #撮った写真を読み込み
    img = cv2.imread(savefile)
    #hsvに変換。(ファイルは最初BGRで読み込まれる)
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    #マスクの最小値と最大値を指定
    lower_red = np.array([160, 50, 50])
    upper_red = np.array([180, 255, 255])
    #マスクをかける
    mask = cv2.inRange(hsv, lower_red, upper_red)
    #画像数の読み込み
    height, width= mask.shape

    #マスクした画像(1bite) から赤色(値が200以上)のみを抽出しカウント
    color_list = mask[mask >= 200]
    color_num = len(mask2.tolist())
    #赤色の画素/全画素数を計算
    red_percent = (color_num/(height*width))*100

    #割合を保存
    file = "logfile.txt"
    with open(file, "a") as logfileobj :
        logfileobj.write(f"red_percent(time = {filename}): {red_percent}")

    #赤の割合を返す。
    return red_percent
