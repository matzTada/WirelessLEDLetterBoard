/*********************************************************************
下記のサイトに情報が掲載されています。

http://www.geocities.jp/bokunimowakaru/diy/xbee/xbee-arduino_3.html

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/

本スケッチの作成に下記の資料を参照しました。
http://playground.arduino.cc/Main/DirectDriveLEDMatrix


Modified by TadaMatz 2016/3/8
*********************************************************************/

#include "FrequencyTimer2.h"

#define	DISPLAY_COLS	240		// 表示用のバッファサイズ 最大256-16
#define	LED_COMMON		1		// LEDの極性。カソードがROW側(Aタイプ)は0、COL側(Bタイプ)は1

// マトリクスLEDの1番ピンから16番ピンをArduinoのどのDigital入力ピンに接続するかを定義します。
// Arduinoの使いやすいピンをLEDに接続することが出来ます。14～19番ピンはAnalog 0～5に相当します。
// 下記のpins[17]の-1よりも後の数字を書き換えてください。

const static
//             /*LED    */  --  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
byte pins[17] = /*Arduino*/ { -1, 14, 15, 16, 17, 18, 4, 3, 2, 5, 6, 7, 8, 9, 10, 11, 12};

// マトリクスLEDのピン番号と表示位置をLEDの仕様書から書き写してください。
// colsは列、rowsは行です。それぞれpins[番号]のように書きます。
const static
byte cols[8] = {pins[13], pins[ 3], pins[ 4], pins[10], pins[ 6], pins[11], pins[15], pins[16]};
const static
byte rows[8] = {pins[ 9], pins[14], pins[ 8], pins[12], pins[ 1], pins[ 7], pins[ 2], pins[ 5]};

////////////////////////////////////////////////////////////////////////////////////////////////

byte col = 0;
byte leds[DISPLAY_COLS];
byte pattern = 0;
byte pattern_max = 0;

unsigned long displayPastMillis = millis();
#define DISPLAY_INTERVAL 100
String serialStr = "";

unsigned long scrollPastMillis = millis();
#define SCROLL_INTERVAL 1

//communication
#include <XBee.h>
union fourbyte {
  uint32_t dword;
  uint16_t word[2];
  uint8_t byte[4];
};
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse(); // create reusable response objects for responses we expect to handle

int flashMoji[8][8] = {
  {1, 1, 0, 0, 0, 1, 0, 0},
  {1, 0, 1, 0, 1, 0, 1, 0},
  {1, 0, 1, 0, 1, 1, 1, 0},
  {1, 1, 0, 0, 1, 0, 1, 0},
  {0, 1, 1, 1, 0, 0, 1, 0},
  {0, 0, 1, 0, 0, 1, 0, 1},
  {0, 0, 1, 0, 0, 1, 1, 1},
  {0, 0, 1, 0, 0, 1, 0, 1},
};


void setup() {
  byte i;
  for (i = 1; i <= 16; i++) pinMode(pins[i], OUTPUT);			// sets the pins as output
  for (i = 1; i <= 8; i++) digitalWrite(cols[i - 1], HIGH);		// set up cols and rows
  for (i = 1; i <= 8; i++) digitalWrite(rows[i - 1], LOW);
  clearLeds();

  Serial.begin(9600);
  Serial.println("LED MATRIX SERIAL COME ON! SERIAL");

  //  FrequencyTimer2::disable();								// Turn off toggling of pin 11
  //  FrequencyTimer2::setPeriod(2000);						// Set refresh rate
  //  FrequencyTimer2::setOnOverflow(display);				// Set interrupt routine to be called
  setPattern("LED MATRIX COME ON! SERIAL");

  xbee.begin(Serial);

  flashLeds();
}

void loop() {
  if (millis() - displayPastMillis > DISPLAY_INTERVAL) {
    displayPastMillis = millis();
    pattern = ++pattern;
    if (pattern > pattern_max) pattern = 0;
  }

  if (millis() - scrollPastMillis > SCROLL_INTERVAL) {
    scrollPastMillis = millis();
    display();
  }

  //  if (Serial.available() > 0) {
  //    char c = Serial.read();
  //    serialStr += c;
  //    if (c == '\n') {
  //      clearLeds();
  //      setPattern((char*)serialStr.c_str());
  //      Serial.println(serialStr);
  //      Serial.println((char*)serialStr.c_str());
  //      serialStr = "";
  //    }
  //  }

  //xbee receiving
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);

      flashLeds();

      clearLeds();

      String serialStr = "";
      for (int i = 0; i < rx.getDataLength(); i++) {
        serialStr += (char)rx.getData(i);
      }
      setPattern((char*)serialStr.c_str());
      Serial.println(serialStr);
      serialStr = "";
    }
  }
}

void clearLeds() {
  for (int i = 0; i < DISPLAY_COLS; i++) leds[i] = 0x00;	// Clear display array
}

void flashLeds() {
  for ( int i = 0; i < 8; i += 1 ) {
    for ( int j = 0; j < 8; j += 1) {
      digitalWrite(rows[j], HIGH);
      digitalWrite(cols[i], LOW);
      delay(5);
      digitalWrite(rows[j], LOW);
      digitalWrite(cols[i], HIGH);
    }
  }

  for (int i = 1; i <= 8; i++) digitalWrite(cols[i - 1], HIGH);    // set up cols and rows
  for (int i = 1; i <= 8; i++) digitalWrite(rows[i - 1], LOW);


  //flashMoji loop
  // カソードのループ
  for (int i = 0; i < 12 ; i ++) {
    for ( int cat = 0; cat < 8; cat++ )
    {
      digitalWrite( cols[cat], LOW ); // LOWに変更
      // アノードのループ
      for ( int ano = 0; ano < 8; ano++ )
      {
        digitalWrite( rows[ano], flashMoji[ano][cat] );  // HIGH or LOW
      }
      delay(10); //点灯時間
      for ( int ano = 0; ano < 8; ano++ )
      {
        digitalWrite( rows[ano], LOW ); // LOWに戻す
      }
      digitalWrite( cols[cat], HIGH );  // HIGHに戻す
    }
 }

  for ( int j = 0; j < 8; j += 1) {
    for ( int i = 0; i < 8; i += 1 ) {
      digitalWrite(rows[j], HIGH);
      digitalWrite(cols[i], LOW);
      delay(5);
      digitalWrite(rows[j], LOW);
      digitalWrite(cols[i], HIGH);
    }
  }
}

void setPattern(char *s) {
  int i = 0, j;
  pattern = 0;
  while (s[i] != '\0' && pattern < DISPLAY_COLS - 8) {
    pattern += Font_Draw(s[i], pattern, -1);
    i++;
  }
  pattern_max = pattern + 16;
  pattern = 0;
}

// Interrupt routine
void display() {
  byte row;
  digitalWrite(cols[col], LED_COMMON); 					// Turn whole previous column off
  //	for(row = 0; row < 8; row++) digitalWrite(rows[row], LED_COMMON);
  col++;
  if (col == 8) col = 0;

  for (row = 0; row < 8; row++) {
    if (	col + pattern >= 8 &&
          col + pattern < pattern_max - 8 &&
          (leds[col + pattern - 8] >> row) & 1 == 1	) {
      digitalWrite(rows[row], LED_COMMON);			// Turn on this led
    } else digitalWrite(rows[row], !LED_COMMON);
  }
  digitalWrite(cols[col], !LED_COMMON);					// Turn whole column on at once

}
