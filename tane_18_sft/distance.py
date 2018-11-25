# coding: utf-8

#緯度線方向の距離の計算
def distance_lat(ido):
    return abs(ido - dest_lat)*1849.1794

#経度線方向の距離の計算
def distance_lng(keido):
    return abs(keido - dest_lng)*1511.1323
