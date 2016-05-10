/*********************************************************************
EEPROMへフォントを書き込みます。

下記のサイトに詳細情報を掲載しています。

http://www.geocities.jp/bokunimowakaru/diy/arduino/matrix_led.html

本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/

※注意
	ライブラリ「FrequencyTimer2」と「Time」を使用します。
	同ライブラリの制約上、Leonardoでは動作いたしません。

本スケッチの作成に下記の資料を参照しました。
http://playground.arduino.cc/Main/DirectDriveLEDMatrix
*********************************************************************/

#include <FrequencyTimer2.h>
#include <EEPROM.h>
#include <Time.h>

#define	DISPLAY_COLS	240		// 表示用のバッファサイズ 最大256-16
#define	LED_COMMON		1		// LEDの極性。カソードがROW側(Aタイプ)は0、COL側(Bタイプ)は1
#define	RX_MAX			64		// シリアル受信バッファサイズ
#define	LED_DURATION	75		// LEDの点灯時間[ms]

// マトリクスLEDの1番ピンから16番ピンをArduinoのどのDigital入力ピンに接続するかを定義します。
// Arduinoの使いやすいピンをLEDに接続することが出来ます。14～19番ピンはAnalog 0～5に相当します。
// 下記のpins[17]の-1よりも後の数字を書き換えてください。

const static
//             /*LED    */  --  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
byte pins[17]= /*Arduino*/ {-1,14,15,16,17,18, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12};

/*
ブレッドボード用：         {-1, 5, 4, 3, 2,14,15,16,17,13,12,11,10, 9, 8, 7, 6};
シールド製作用：           {-1,13,12,11,10,14,15,16,17, 2, 3, 4, 5, 6, 7, 8, 9};
*/


// マトリクスLEDのピン番号と表示位置をLEDの仕様書から書き写してください。
// colsは列、rowsは行です。それぞれpins[番号]のように書きます。
const static
byte cols[8] = {pins[13],pins[ 3],pins[ 4],pins[10],pins[ 6],pins[11],pins[15],pins[16]};
const static
byte rows[8] = {pins[ 9],pins[14],pins[ 8],pins[12],pins[ 1],pins[ 7],pins[ 2],pins[ 5]};

////////////////////////////////////////////////////////////////////////////////////////////////

/* シリアルからの文字入力用の変数定義 */
char rx[RX_MAX+1];				// 表示テキスト入力データ保持用(ここにテキスト文字を代入する)
byte rx_i			= 0;		// シリアル受信したテキスト文字の位置番号

/* マトリクスLEDの駆動用の変数定義 */
byte leds[DISPLAY_COLS];		// LEDに表示する画素データ保持用
byte pattern 		= 0;		// スクロール表示のまめの表示位置指定用の変数
byte pattern_max 	= 0;		// スクロール表示の最終桁
byte col 			= 0;		// ダイナミック点灯表示中の桁番号

/* 時計用の変数定義 */
boolean esc_enable	= 0;		// ESCコード状態
byte clock_disp		= 0;		// 時計位置、0の時は表示オフ
char clock_str[7];				// 時刻テキスト

void setup() {
	byte i;
	Serial.begin(9600);
	for(i=1;i<=16;i++) pinMode(pins[i], OUTPUT);			// sets the pins as output
	for(i=1;i<= 8;i++) digitalWrite(cols[i-1], LED_COMMON);	// set up cols and rows
	for(i=1;i<= 8;i++) digitalWrite(rows[i-1], LED_COMMON);
	          //0123456789012345
	i=font_kanji_init();
//	Serial.println((byte)i);
	clearLeds();
	sprintf(rx,"{ﾎﾞｸ}ﾆﾓﾜｶﾙ%c%c{ﾃﾞｼﾞﾀﾙ}",(char)0xFD,(char)0x9E);	//
//	for(i=0;i<30;i++){ Serial.print((byte)rx[i],HEX);Serial.print(" ");}Serial.println();
	utf_del_uni(rx);
	setPattern(rx);
//	Serial.println(rx);
//	for(i=0;i<15;i++){ Serial.print((byte)rx[i],HEX);Serial.print(" ");}Serial.println();
}

void loop() {
	if (Serial.available() == 0) {							// シリアル入力に何も無い時
		pattern = ++pattern;								// LEDスクロール位置変更
		if(pattern > pattern_max){							// 最後まで表示したら
			clearLeds();									// LED表示用のピクセルバッファを消去
			if(rx_i>0){
				rx[rx_i]='\0';								// シリアルバッファに終了文字を入れる
				if(esc_enable){								// ESCコード
					switch(rx[0]){
						case 'T':	// 時刻設定
							if( isdigit(rx[1]) ){			// [ESC][T]の後に数字が続くとき
								clock_adj( &rx[1] );		// 時刻調整を実施
							}
							clock_disp=1;
							rx[0]=0x89; rx[1]=0x9C;			// 「時刻」の文字を代入
							rx_i=2;
							break;
						default:
							break;
					}
					esc_enable=0;
				}				
				if( clock_disp ){
					clock_disp = rx_i;						// 時計表示の開始位置
					if( rx_i+7 > RX_MAX) clock_disp = RX_MAX-7;
				}
				rx_i=0;										// 次の受信に備える
			}
			if(clock_disp){
				clock(clock_str);							// 時計表示データを作成
				rx[clock_disp]=' ';							// スペースの挿入
				for(byte i=0;i<7;i++)rx[i+clock_disp+1]=clock_str[i]; // 時刻6文字
			}														 // スペースと合わせて7文字
			utf_del_uni(rx);
			setPattern(rx);									// 受信した文字をセット
		//	Serial.println(rx);								// これから表示する内容をシリアルへ
			pattern=0;										// 最初に戻す
		}
		delay(LED_DURATION);								// スクロール用のウェイト
	}else{
		/*シリアルからの受信処理*/
		rx[rx_i]=Serial.read();								// 受信値をバッファへ保存
		if( rx[rx_i]==0x1B ) esc_enable=1;					// ESCコードの時
		if( isprint(rx[rx_i]) || (byte)rx[rx_i] > 0x80){	// 表示可能な文字の時
			rx_i++;
			if(rx_i>RX_MAX) rx_i=RX_MAX;
		}else if(rx[rx_i]=='\n') rx_i=RX_MAX;				// これ以上、受信しない
	}
}

void clearLeds() {
	FrequencyTimer2::disable();								// Turn off toggling of pin 11
	for(int i = 0; i < DISPLAY_COLS; i++) leds[i] = 0x00;	// Clear display array
}

void setPattern(char *s) {
	int i=0,j;
	pattern=0;
	
	while(s[i] != '\0' && pattern < DISPLAY_COLS-8){
		pattern += font_draw(s[i],pattern);
		i++;
	}
	pattern_max = pattern+16;
	pattern=0;
	FrequencyTimer2::setPeriod(2000);						// Set refresh rate
	FrequencyTimer2::setOnOverflow(display);				// Set interrupt routine to be called
}

// Interrupt routine
void display() {
	byte row;
	digitalWrite(cols[col], LED_COMMON); 					// Turn whole previous column off
//	for(row = 0; row < 8; row++) digitalWrite(rows[row], LED_COMMON);
	col++;
	if (col == 8) col = 0;
	
	for (row = 0; row < 8; row++) {
		if(	col+pattern >= 8 &&
			col+pattern < pattern_max-8 &&
			(leds[col+pattern-8]>>row)&1 == 1	){
			digitalWrite(rows[row], LED_COMMON);			// Turn on this led
		} else digitalWrite(rows[row], !LED_COMMON);
	}
	digitalWrite(cols[col], !LED_COMMON);					// Turn whole column on at once

}

void clock(char *s){
	byte hh = (byte)hour();
	byte mm = (byte)minute();
	s[0] = (char)( hh/10 )+'0';
	s[1] = (char)( hh%10 )+'0';
	s[3] = (char)( mm/10 )+'0';
	s[4] = (char)( mm%10 )+'0';
	s[2] = ':';
	s[5] = '\0';
}

void clock_adj(char *s){
	if( s[0] >= '0' && s[0] <= '9' &&
		s[1] >= '0' && s[1] <= '9' &&
		s[2] == ':' &&
		s[3] >= '0' && s[3] <= '9' &&
		s[4] >= '0' && s[4] <= '9'){
		setTime(	(int)(	 (s[0]-'0')*10
							+(s[1]-'0')		),
					(int)(	 (s[3]-'0')*10
							+(s[4]-'0')		),0,NULL,NULL,NULL);
	}
}
