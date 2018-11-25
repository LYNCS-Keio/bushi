#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#define BMP085_ADDRESS 0x77  // I2C address of BMP085
const unsigned char OSS = 0;  // Oversampling Setting

							  // Calibration flightpin2ues
int ac1;
int ac2;
int ac3;
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1;
int b2;
int mb;
int mc;
int md;
// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5;

int i = 0;  //緊急エスケープ
int j = 0;  //同上

Servo servor;     //右,パラ分離
Servo servol;  //左,キャリア展開
int flightpin1 = 7;  //フライトピン
int flightpin2 = 0;         //同上
SoftwareSerial softwareserial(8, 4); //GPS


void setup() {
	Serial1.begin(9600);
	//softwareserial.begin(9600);
	Wire.begin();
	bmp085Calibration();
	servor.attach(9);     //右、パラ分離
	servol.attach(10); //左、キャリア展開(これらは基板によってピン変わる)
	pinMode(flightpin1, INPUT);
	delay(5000);

	height_servo_flightpin();

	delay(3000);

	gps();
}



void loop() {
	;
}

//以下は高度センサの値取得と計算用
// Stores all of the bmp085's calibration flightpin2ues into global variables
// Calibration flightpin2ues are required to calculate temp and pressure
// This function should be called at the beginning of the program
void bmp085Calibration() {
	ac1 = bmp085ReadInt(0xAA);
	ac2 = bmp085ReadInt(0xAC);
	ac3 = bmp085ReadInt(0xAE);
	ac4 = bmp085ReadInt(0xB0);
	ac5 = bmp085ReadInt(0xB2);
	ac6 = bmp085ReadInt(0xB4);
	b1 = bmp085ReadInt(0xB6);
	b2 = bmp085ReadInt(0xB8);
	mb = bmp085ReadInt(0xBA);
	mc = bmp085ReadInt(0xBC);
	md = bmp085ReadInt(0xBE);
}

// Calculate temperature in deg C
float bmp085GetTemperature(unsigned int ut) {
	long x1, x2;
	x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
	x2 = ((long)mc << 11) / (x1 + md);
	b5 = x1 + x2;
	float temp = ((b5 + 8) >> 4);
	temp = temp / 10;
	return temp;
}

// Calculate pressure given up
// calibration flightpin2ues must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// flightpin2ue returned will be pressure in units of Pa.
long bmp085GetPressure(unsigned long up) {
	long x1, x2, x3, b3, b6, p;
	unsigned long b4, b7;
	b6 = b5 - 4000;
	// Calculate B3
	x1 = (b2 * (b6 * b6) >> 12) >> 11;
	x2 = (ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((((long)ac1) * 4 + x3) << OSS) + 2) >> 2;
	// Calculate B4
	x1 = (ac3 * b6) >> 13;
	x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;
	b7 = ((unsigned long)(up - b3) * (50000 >> OSS));
	if (b7 < 0x80000000)
		p = (b7 << 1) / b4;
	else
		p = (b7 / b4) << 1;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p += (x1 + x2 + 3791) >> 4;
	long temp = p;
	return temp;
}

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address) {
	unsigned char data;
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(address);
	Wire.endTransmission();
	Wire.requestFrom(BMP085_ADDRESS, 1);
	while (!Wire.available());
	return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address) {
	unsigned char msb, lsb;
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(address);
	Wire.endTransmission();
	Wire.requestFrom(BMP085_ADDRESS, 2);
	while (Wire.available()<2);
	msb = Wire.read();
	lsb = Wire.read();
	return (int)msb << 8 | lsb;
}

// Read the uncompensated temperature flightpin2ue
unsigned int bmp085ReadUT() {
	unsigned int ut;
	// Write 0x2E into Register 0xF4
	// This requests a temperature reading
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(0xF4);
	Wire.write(0x2E);
	Wire.endTransmission();
	// Wait at least 4.5ms
	delay(5);
	// Read two bytes from registers 0xF6 and 0xF7
	ut = bmp085ReadInt(0xF6);
	return ut;
}

// Read the uncompensated pressure flightpin2ue
unsigned long bmp085ReadUP() {
	unsigned char msb, lsb, xlsb;
	unsigned long up = 0;
	// Write 0x34+(OSS<<6) into register 0xF4
	// Request a pressure reading w/ oversampling setting
	Wire.beginTransmission(BMP085_ADDRESS);
	Wire.write(0xF4);
	Wire.write(0x34 + (OSS << 6));
	Wire.endTransmission();
	// Wait for conversion, delay time dependent on OSS
	delay(2 + (3 << OSS));
	// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
	msb = bmp085Read(0xF6);
	lsb = bmp085Read(0xF7);
	xlsb = bmp085Read(0xF8);
	up = (((unsigned long)msb << 16) | ((unsigned long)lsb << 8) | (unsigned long)xlsb) >> (8 - OSS);
	return up;
}

void writeRegister(int deviceAddress, byte address, byte flightpin2) {
	Wire.beginTransmission(deviceAddress); // start transmission to device 
	Wire.write(address);       // send register address
	Wire.write(flightpin2);         // send flightpin2ue to write
	Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address) {
	int v;
	Wire.beginTransmission(deviceAddress);
	Wire.write(address); // register to read
	Wire.endTransmission();
	Wire.requestFrom(deviceAddress, 1); // read a byte
	while (!Wire.available()) {}// waiting
	v = Wire.read();
	return v;
}

float calcAltitude(float pressure, float temperature) {
	float A = 102100 / pressure; //左は地面の気圧
	float B = 1 / 5.257;
	float C = pow(A, B);
	float D = temperature + 273.15; //左は気温
	C = C - 1;
	C = C * D;
	C = C / 0.0065;
	return C;
}

/*
気圧センサから高度の値を計算し続けて、
フライトピンが開いたら（パラシュートが展開したら）
その値をシリアルでラズパイに送る。
その後、高度が2ｍ未満に2連続でなったらサーボを回す。
（空中分離）その後、もう一つのサーボを回しキャリアを分離。
*/
void height_servo_flightpin() {
	servor.write(180);
	delay(5000);
	servol.write(0);
	delay(5000);
	delay(900000);
	while (1) {
		flightpin2 = digitalRead(flightpin1);
		/*servor.write(180);
		delay(100);
		servol.write(0);
		delay(100);*/
		float temperature = bmp085GetTemperature(bmp085ReadUT()); //MUST be called first
		float pressure = bmp085GetPressure(bmp085ReadUP());
		float altitude = calcAltitude(pressure, temperature); //Uncompensated caculation - in Meters 

		if (flightpin2 == 0) {
			if (i == 0) {
				Serial1.println("f");
				delay(100);
			}
			//Serial1.print("Altitude: ");
			Serial1.println(altitude, 2); //display 2 decimal places
										  //Serial1.println(" m");
			i = i + 1;
			if (i > 600) {
				servor.write(50);
				delay(5000);
				Serial1.println("s1");
				delay(1000);
				goto secondservo;
			}
			if (altitude < 2.5) {
				while (1) {
					j = j + 1;
					float temperature = bmp085GetTemperature(bmp085ReadUT()); //MUST be called first
					float pressure = bmp085GetPressure(bmp085ReadUP());
					float altitude = calcAltitude(pressure, temperature); //Uncompensated caculation - in Meters 
																		  //Serial1.print("Altitude: ");
					Serial1.println(altitude, 2); //display 2 decimal places
												  //Serial1.println(" m");
					if (altitude < 2.5) {
						servor.write(50);
						delay(5000);
						Serial1.println("s1");
						delay(1000);
						goto secondservo;
					}
					if (j > 600) {
						servor.write(50);
						delay(5000);
						Serial1.println("s1");
						delay(1000);
						goto secondservo;
					}
					delay(200);
				}
			}
		}
		else { ; }
		delay(200); //wait a second and get values again
	}
secondservo:
	delay(10000);
	servol.write(130);
	delay(5000);
	Serial1.println("s2");
	delay(10000);
}


/*GPSの値を読み取ってstringにし、
緯度、経度、方向（真北を0とした0～359.99）
をコンマ区切りの一行にしてラズパイに渡す*/

void gps() {
	softwareserial.begin(9600);
	delay(3000);
	//Serial1.println("GPS Start!");
	String buf;
	char ch = Serial1.read();
	while (1) {
		if (ch == '1') {
			while (1) {
				if (softwareserial.available()) {
					buf = softwareserial.readStringUntil('\n');

					if (buf != "") {
						int i, index = 0, len = buf.length();
						String str = "";

						String list[30];
						for (i = 0; i < 30; i++) {
							list[i] = "";
						}// StringListの生成(簡易)

						for (i = 0; i < len; i++) {
							if (buf[i] == ',') {
								list[index++] = str;
								str = "";
								continue;
							}
							str += buf[i];
						}// 「,」を区切り文字として文字列を配列にする

						Serial1.print(list[3]);//latitude
						Serial1.print(",");
						Serial1.print(list[5]);//longitude
						Serial1.print(",");
						Serial1.println(list[8]);//direction
						break;
					}
				}
				else { ; }
			}
		}
		else { ; }
	}
}


