#include <SoftwareSerial.h>
SoftwareSerial mySerial(15, 16);
//以下goal座標
long latupgo = 4008;
long latungo = 54320;
long lngupgo = 13959;
long lngungo = 23785;
//緯度と経度の整数部、小数点以下部分を順番に
int digitalPin = 3;  //フライトピン
int val = 0;         //同上
long latup2;
long latun2;
long lngup2;
long lngun2;
long latup1;
long latun1;
long lngup1;
long lngun1;
long latup;
long latun;
long lngup;
long lngun;
long latups;
long latuns;
long lngups;
long lnguns;
long lats;
long lngs;
int i = 1;
int motorApin2 = A2;
int motorBpin2 = A3;
int motorvref2 = 9;//for motor2
int motorApin3 = A4;
int motorBpin3 = A5;
int motorvref3 = 10;//for motor3
int ch = 0;//motor 場合分け
int goal = 0;
void motorStop();
void setup() {
	mySerial.begin(9600);
	Serial1.begin(9600);
	delay(5000);
	while (!mySerial && !Serial1) { ; }
	pinMode(motorApin2, OUTPUT);
	pinMode(motorBpin2, OUTPUT);
	pinMode(motorvref2, OUTPUT);
	pinMode(motorApin3, OUTPUT);
	pinMode(motorBpin3, OUTPUT);
	pinMode(motorvref3, OUTPUT);
	digitalWrite(motorvref2, HIGH);
	digitalWrite(motorvref3, HIGH);
	digitalWrite(motorApin2, HIGH);//左,逆
	digitalWrite(motorBpin2, HIGH);
	digitalWrite(motorApin3, HIGH);//右,正
	digitalWrite(motorBpin3, HIGH);
	//ブレーキ
	flightpin();
	Serial1.println("Start GNSS Logging...");
	while (goal<1) {
		gnss();
		motor();
		thinkingface();
	}
	Serial1.println("GOAL!!!!");
}
void loop() {}

void flightpin() {
	while (1) {
		val = digitalRead(digitalPin);
		if (val == HIGH) {
			delay(36000);
			digitalWrite(motorApin2, LOW);//左,逆
			digitalWrite(motorBpin2, HIGH);
			digitalWrite(motorApin3, HIGH);//右,正
			digitalWrite(motorBpin3, LOW);
			delay(20000);
			//front
			motorStop();
			//stop
			//motor front 20sec
			break;
		}
		else {
			;
		}
	}
}

void gnss() {
	String buf;
	if (mySerial.available()) {
		buf = mySerial.readStringUntil('\n');
		if (buf != "") {
			int i, index = 0, len = buf.length();
			String str = "";
			String list[30];
			for (i = 0; i < 30; i++) {
				list[i] = "";
			}
			for (i = 0; i < len; i++) {
				if (buf[i] == ',') {
					list[index++] = str;
					str = "";
					continue;
				}
				str += buf[i];
			}
			if (list[0] == "$GNGGA") {
				list[2] = list[2] + ".";
				list[3] = list[3] + ".";
				//小数点以下と整数部に分けてそれぞれをlongに変換
				index = 0;
				str = "";
				int len1 = list[2].length();
				String lati[30];
				for (i = 0; i < 30; i++) {
					lati[i] = "";
				}
				for (i = 0; i < len1; i++) {
					if (list[2][i] == '.') {
						lati[index++] = str;
						str = "";
						continue;
					}
					str += list[2][i];
				}//整数部と小数部に分ける。
				index = 0;
				str = "";
				int len2 = list[3].length();
				String lngi[30];
				for (i = 0; i < 30; i++) {
					lngi[i] = "";
				}
				for (i = 0; i < len2; i++) {
					if (list[3][i] == '.') {
						lngi[index++] = str;
						str = "";
						continue;
					}
					str += list[3][i];
				}
				latup = lati[0].toInt();
				latun = lati[1].toInt();
				//この時、下1桁の0だったりは消えてしまうので注意
				lngup = lngi[0].toInt();
				lngun = lngi[1].toInt();
				//この時、下1桁の0だったりは消えてしまうので注意
												 //139XXは通らない
				if (3900 < lngup && lngup <13900) {//39XX,40XXを処理
					lngup = 10000 + lngup;
				}
				if (900 < lngup && lngup <960) {//9XXを処理
					lngup = 13000 + lngup;
				}
				if (lngup < 60) {//XXを処理
					if (30<lngup) {
						lngup = 13900 + lngup;
					}
					else {
						lngup = 14000 + lngup;
					}
				}
				if (i == 1) {
					latup2 = latup;
					latun2 = latun;
					lngup2 = lngup;
					lngun2 = lngun;
				}
				else {
					latup1 = latup2;//~~1...previous
					latun1 = latun2;
					lngup1 = lngup2;
					lngun1 = lngun2;

					latup2 = latup;//~~2...now
					latun2 = latun;
					lngup2 = lngup;
					lngun2 = lngun;

					latups = latup2 - latup1;//差
					latuns = latun2 - latun1;//差
					lngups = lngup2 - lngup1;//差
					lnguns = lngun2 - lngun1;//差

					if (latups>100) {
						latups = latups - 100 + 60;
					}
					if (lngups>100) {
						lngups = lngups - 100 + 60;
					}
					lats = latups * 100000 + latuns;//分
					lngs = lngups * 100000 + lnguns;//分   

					if ((latup2<latupgo || latun2<latungo) && (lngup2>lngupgo || lngun2>lngungo)) {
					//右下、下,右の順
						if ((lats>0) && (lngs>0)) {//右上進行、上,右の順
							ch = 2;
							//turnleft
						}
						if ((lats<0) && (lngs>0)) {//右下進行、下,右の順
							if (ch == 3) {
								ch = 4;
							}
							else {
								ch = 3;
							}
							//oppositeturn
						}
						if ((lats<0) && (lngs<0)) {//左下進行、下,左の順
							ch = 1;
							//turnright
						}
						if ((lats>0) && (lngs<0)) {//左上進行、上,左の順
							ch = 0;
						}
					}
					if ((latup2<latupgo || latun2<latungo) && (lngup2<lngupgo || lngun2<lngungo)) {
					//左下、下,左の順
						if ((lats>0) && (lngs>0)) {//右上進行、上,右の順
							ch = 0;
						}
						if ((lats<0) && (lngs>0)) {//右下進行、下,右の順
							ch = 2;
							//turnleft
						}
						if ((lats<0) && (lngs<0)) {//左下進行、下,左の順
							if (ch == 3) {
								ch = 4;
							}
							else {
								ch = 3;
							}
							//turnoppsite
						}
						if ((lats>0) && (lngs<0)) {//左上進行、上,左の順
							ch = 1;
							//turnright
						}
					}
					if ((latup2>latupgo || latun2>latungo) && (lngup2<lngupgo || lngun2<lngungo)) {
					//左上、上,左の順
						if ((lats>0) && (lngs>0)) {//右上進行、上,右の順
							ch = 1;
							//turnright
						}
						if ((lats<0) && (lngs>0)) {//右下進行、下,右の順
							ch = 0;
						}
						if ((lats<0) && (lngs<0)) {//左下進行、下,左の順
							ch = 2;
							//turnleft
						}
						if ((lats>0) && (lngs<0)) {//左上進行、上,左の順
							if (ch == 3) {
								ch = 4;
							}
							else {
								ch = 3;
							}
							//turnopposite
						}
					}
					if ((latup2>latupgo || latun2>latungo) && (lngup2>lngupgo || lngun2>lngungo)) {
					//右上、上,右の順
						if ((lats>0) && (lngs>0)) {//右上進行、上,右の順
							if (ch == 3) {
								ch = 4;
							}
							else {
								ch = 3;
							}
							//turnopposite
						}
						if ((lats<0) && (lngs>0)) {//右下進行、下,右の順
							ch = 1;
							//turnright
						}
						if ((lats<0) && (lngs<0)) {//左下進行、下,左の順
							ch = 0;
							//
						}
						if ((lats>0) && (lngs<0)) {//左上進行、上,左の順
							ch = 2;
							//turnleft
						}
					}
				}
				Serial1.print(latup, 5);
				Serial1.print(",");
				Serial1.print(latun, 5);
				Serial1.print(",");
				Serial1.print(lngup, 5);
				Serial1.print(",");
				Serial1.print(lngun, 5);
				Serial1.println();
				i++;
			}
			else { ; }
		}
	}
	else {
		delay(1000);
	}
}

void motorStop(){
	digitalWrite(motorApin2, LOW);
	digitalWrite(motorBpin2, LOW);
	digitalWrite(motorApin3, LOW);
	digitalWrite(motorBpin3, LOW);
}

void motor() {
	digitalWrite(motorvref2, HIGH);
	digitalWrite(motorvref3, HIGH);
	//2_front,3_back = cansat_back
	//2_front,3_front = cansat_turning_left
	//2_back,3_front = cansat_front
	//2_back,3_back = cansat_turning_right
	//2 = left & awkward , 3 = right & correct
	if (ch == 0) {
		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, HIGH);
		digitalWrite(motorApin3, HIGH);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(10000);
		//front
		motorStop();
		//stop
		//front
	}
	else if (ch == 1) {
		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, HIGH);
		digitalWrite(motorApin3, LOW);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(1000);
		//2_back,3_back = cansat_turning_right

		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, HIGH);
		digitalWrite(motorApin3, HIGH);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(10000);
		//front
		motorStop();
		//stop
		//turnright & front
	}
	else if (ch == 2) {
		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, LOW);
		digitalWrite(motorApin3, HIGH);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(1000);
		//2_front,3_front = cansat_turning_left

		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, HIGH);
		digitalWrite(motorApin3, HIGH);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(10000);
		//front
		motorStop();
		//stop
		//turnleft & front
	}
	else if (ch == 3) {
		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, HIGH);
		digitalWrite(motorApin3, LOW);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(2000);
		//2_back,3_back = cansat_turning_right
		//秒数2倍で180度

		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, HIGH);
		digitalWrite(motorApin3, HIGH);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(10000);
		//front
		motorStop();
		//stop
		//turnopposite & front
	}
	else if (ch == 4) {
		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, HIGH);
		digitalWrite(motorApin3, LOW);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(2000);
		//2_back,3_back = cansat_turning_right
		//秒数2倍

		digitalWrite(motorApin2, LOW);//左,逆
		digitalWrite(motorBpin2, HIGH);
		digitalWrite(motorApin3, HIGH);//右,正
		digitalWrite(motorBpin3, LOW);
		delay(5000);
		//front
		motorStop();
		//stop
		//turnopposite & front
		//for only half
	}
}

void thinkingface() {
	int t = 0;
	int f = 0;
	if (latupgo == latup && lngupgo == lngup) {
		if (0 <= latungo - latun <= 5) {
			t++;
		}
		if (0<latun - latungo <= 5) {
			t++;
		}
		if (0 <= lngungo - lngun <= 5) {
			t++;
		}
		if (0<lngun - lngungo <= 5) {
			t++;
		}
		if (t >= 2) {
			f++;
			delay(2000);
			latup = 0;
		}
		else { ; }
		//チェックアゲイン
		if (f >= 1) {
			while (latup <= 0) {
				String buf;
				if (mySerial.available()) {
					buf = mySerial.readStringUntil('\n');
					if (buf != "") {
						int i, index = 0, len = buf.length();
						String str = "";
						String list[30];
						for (i = 0; i < 30; i++) {
							list[i] = "";
						}
						for (i = 0; i < len; i++) {
							if (buf[i] == ',') {
								list[index++] = str;
								str = "";
								continue;
							}
							str += buf[i];
						}
						if (list[0] == "$GNGGA") {
							list[2] = list[2] + ".";
							list[3] = list[3] + ".";
							//小数点以下と整数部に分けてそれぞれをlongに変換
							index = 0;
							str = "";
							int len1 = list[2].length();
							String lati[30];
							for (i = 0; i < 30; i++) {
								lati[i] = "";
							}
							for (i = 0; i < len1; i++) {
								if (list[2][i] == '.') {
									lati[index++] = str;
									str = "";
									continue;
								}
								str += list[2][i];
							}//整数部と小数部に分ける。
							index = 0;
							str = "";
							int len2 = list[3].length();
							String lngi[30];
							for (i = 0; i < 30; i++) {
								lngi[i] = "";
							}
							for (i = 0; i < len2; i++) {
								if (list[3][i] == '.') {
									lngi[index++] = str;
									str = "";
									continue;
								}
								str += list[3][i];
							}
							latup = lati[0].toInt();
							latun = lati[1].toInt();
							//この時、下1桁の0だったりは消えてしまうので注意
							lngup = lngi[0].toInt();
							lngun = lngi[1].toInt();//この時、下1桁の0だったりは消えてしまうので注意
															 //139XXは通らない
							if (3900 < lngup && lngup <13900) {//39XX,40XXを処理
								lngup = 10000 + lngup;
							}
							if (900 < lngup && lngup <960) {//9XXを処理
								lngup = 13000 + lngup;
							}
							if (lngup < 60) {//XXを処理
								if (30<lngup) {
									lngup = 13900 + lngup;
								}
								else {
									lngup = 14000 + lngup;
								}
							}
						}
					}
				}
			}

			if (latupgo == latup && lngupgo == lngup) {
				if (0 <= latungo - latun <= 5) {
					t++;
				}
				if (0<latun - latungo <= 5) {
					t++;
				}
				if (0 <= lngungo - lngun <= 5) {
					t++;
				}
				if (0<lngun - lngungo <= 5) {
					t++;
				}
				if (t >= 2) {
					f++;
				}
			}
		}
		if (f >= 2) {
			goal++;
		}
	}
}//goal判定用
