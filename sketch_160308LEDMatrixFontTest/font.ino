/*********************************************************************
フォント用ライブラリ Font drawing library

下記のサイトに詳細情報を掲載しています。

http://www.geocities.jp/bokunimowakaru/diy/arduino/matrix_led.html

本ソースコード及びソフトウェアはコピーレフトのフリーソフトウェアです。
GNU GPL Ver 2.1(または以降)に基づいて利用、編集、再配布等が行えます。
詳細は末尾の「参考文献・ライセンスについて」を参照ください。

                               Copyright (c) 2014 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/

#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <inttypes.h>

#define	DISPLAY_ROWS 8
#define	LOWERCASE

#define	L(x) letter_##x
#define	C(x) PROGMEM const uint8_t L(x)[]
#define	P(c,r) ((c << 4) | r)
#define	END 255

#define	KANJI_NUM		146		// 漢字フォント数
#define	EEPROM_WRITE		// EEPROMへの書込み実行時に定義します
								// プログラム容量を節約する場合は削除
								// ※約1402バイトの節約が可能です
boolean font_kana_mode=0;		// ひらがな=0 カタカナ=1
byte    font_kanji_code=0x00;

#ifdef EEPROM_WRITE
static const byte font_kanji[KANJI_NUM][8] ={
	{0x10,0x7C,0x20,0x76,0x98,0x28,0x1E,0x0},		//を
	{0x0,0x10,0x3C,0x10,0x3C,0x5A,0x32,0x0},		//ぁ
	{0x0,0x0,0x48,0x44,0x44,0x50,0x20,0x0},			//ぃ
	{0x0,0x38,0x0,0x38,0x44,0x8,0x30,0x0},			//ぅ
	{0x0,0x38,0x0,0x78,0x10,0x30,0x4C,0x0},			//ぇ
	{0x0,0x20,0x74,0x20,0x38,0x64,0x28,0x0},		//ぉ
	{0x0,0x28,0x2C,0x72,0x14,0x10,0x8,0x0},			//ゃ
	{0x0,0x10,0x58,0x74,0x54,0x18,0x20,0x0},		//ゅ
	{0x0,0x8,0x8,0xC,0x38,0x48,0x34,0x0},			//ょ
	{0x0,0x0,0x0,0x18,0x64,0x4,0x18,0x0},			//っ
	{0x20,0x7C,0x20,0x3C,0x6A,0xB2,0x64,0x0},		//あ
	{0x0,0x88,0x84,0x82,0x82,0x50,0x20,0x0},		//い
	{0x3C,0x0,0x3C,0x42,0x2,0x4,0x38,0x0},			//う
	{0x3C,0x0,0x7C,0x8,0x18,0x28,0x46,0x0},			//え
	{0x20,0xF4,0x22,0x3C,0x62,0xA2,0x6C,0x0},		//お
	{0x20,0x20,0xF4,0x2A,0x4A,0x48,0xB0,0x0},		//か
	{0x10,0x7C,0x8,0x7E,0x24,0x40,0x3C,0x0},		//き
	{0x4,0x8,0x30,0x40,0x30,0x8,0x4,0x0},			//く
	{0x88,0x88,0xBE,0x88,0x88,0x88,0x10,0x0},		//け
	{0x0,0x3C,0x0,0x0,0x20,0x40,0x3E,0x0},			//こ
	{0x8,0x8,0x7E,0x4,0x24,0x40,0x3C,0x0},			//さ
	{0x20,0x20,0x20,0x20,0x20,0x22,0x1C,0x0},		//し
	{0x8,0xFE,0x18,0x28,0x18,0x8,0x10,0x0},			//す
	{0x24,0x24,0xFE,0x24,0x2C,0x20,0x1E,0x0},		//せ
	{0x3C,0x8,0x10,0x7E,0x10,0x10,0xC,0x0},			//そ
	{0x20,0xF0,0x2E,0x40,0x48,0x50,0x8E,0x0},		//た
	{0x8,0x7E,0x10,0x1C,0x22,0x2,0x1C,0x0},			//ち
	{0x0,0x3C,0xC2,0x2,0x2,0x1C,0x0,0x0},			//つ
	{0xE,0x74,0x8,0x10,0x10,0x8,0x6,0x0},			//て
	{0x20,0x20,0x26,0x18,0x20,0x40,0x3E,0x0},		//と
	{0x20,0xF4,0x22,0x44,0x9C,0x26,0x18,0x0},		//な
	{0x80,0x9C,0x80,0x80,0x90,0xA0,0x9E,0x0},		//に
	{0x8,0x48,0x5C,0x6A,0xB2,0xA6,0x56,0x0},		//ぬ
	{0x20,0x2C,0xF2,0x22,0x66,0xAA,0x24,0x0},		//ね
	{0x0,0x38,0x54,0x92,0xA2,0x44,0x18,0x0},		//の
	{0x84,0x84,0xBE,0x84,0x9C,0xA4,0x9A,0x0},		//は
	{0x28,0xE4,0x26,0x44,0x44,0x44,0x38,0x0},		//ひ
	{0x10,0x8,0x10,0x10,0x4C,0x4A,0xB2,0x0},		//ふ
	{0x0,0x20,0x50,0x88,0x6,0x0,0x0,0x0},			//へ
	{0x80,0xBE,0x88,0xBE,0x88,0xBC,0xBA,0x0},		//ほ
	{0x8,0x7E,0x8,0x7E,0x8,0x7C,0x7A,0x0},			//ま
	{0x70,0x10,0x24,0x7C,0xA6,0xC4,0x18,0x0},		//み
	{0x20,0xF4,0x22,0x60,0xA0,0x62,0x3C,0x0},		//む
	{0x8,0x48,0x5C,0x6A,0xB2,0xA2,0x54,0x0},		//め
	{0x10,0x7C,0x20,0x7C,0x22,0x22,0x1C,0x0},		//も
	{0x48,0x5C,0xE2,0x24,0x20,0x10,0x10,0x0},		//や
	{0x10,0xBC,0xD2,0x92,0xBC,0x10,0x20,0x0},		//ゆ
	{0x8,0x8,0xE,0x8,0x38,0x4C,0x32,0x0},			//よ
	{0x30,0x8,0x40,0x5C,0x62,0x2,0x3C,0x0},			//ら
	{0x58,0x64,0x44,0x44,0x4,0x8,0x30,0x0},			//り
	{0x3C,0x8,0x10,0x3C,0x42,0x32,0x3C,0x0},		//る
	{0x20,0x2C,0xF4,0x24,0x64,0xA4,0x22,0x0},		//れ
	{0x3C,0x8,0x10,0x3C,0x42,0x2,0x3C,0x0},			//ろ
	{0x20,0x2C,0xF2,0x22,0x62,0xA2,0x2C,0x0},		//わ
	{0x10,0x10,0x20,0x20,0x50,0x52,0x8C,0x0},		//ん
	{0x0,0x0,0x7C,0x14,0x18,0x10,0x20,0x0},			//ァ
	{0x0,0x0,0x4,0x8,0x18,0x68,0x8,0x0},			//ィ
	{0x0,0x0,0x10,0x7C,0x44,0x8,0x30,0x0},			//ゥ
	{0x0,0x0,0x0,0x38,0x10,0x10,0x7C,0x0},			//ェ
	{0x0,0x0,0x8,0x7C,0x18,0x68,0x18,0x0},			//ォ
	{0x0,0x0,0x20,0x3C,0x64,0x10,0x10,0x0},			//ャ
	{0x0,0x0,0x0,0x38,0x8,0x8,0x7C,0x0},			//ュ
	{0x0,0x0,0x3C,0x4,0x1C,0x4,0x3C,0x0},			//ョ
	{0x0,0x0,0x0,0x54,0x54,0x8,0x30,0x0},			//ッ
	{0x7E,0x2,0x14,0x18,0x10,0x10,0x20,0x0},		//ア
	{0x2,0x4,0x8,0x18,0x68,0x8,0x8,0x0},			//イ
	{0x10,0x7E,0x42,0x2,0x4,0x8,0x30,0x0},			//ウ
	{0x0,0x7C,0x10,0x10,0x10,0xFE,0x0,0x0},			//エ
	{0x8,0xFE,0x8,0x18,0x28,0xC8,0x18,0x0},			//オ
	{0x10,0x7E,0x12,0x12,0x12,0x22,0x46,0x0},		//カ
	{0x10,0x7C,0x10,0x10,0x7E,0x8,0x8,0x0},			//キ
	{0x10,0x1E,0x22,0x42,0x4,0x8,0x30,0x0},			//ク
	{0x40,0x7E,0x48,0x88,0x8,0x10,0x20,0x0},		//ケ
	{0x0,0x7E,0x2,0x2,0x2,0x2,0x7E,0x0},			//コ
	{0x24,0xFE,0x24,0x24,0x4,0x8,0x30,0x0},			//サ
	{0x40,0x20,0x42,0x22,0x4,0x8,0x70,0x0},			//シ
	{0x0,0x7C,0x4,0x8,0x8,0x34,0xC2,0x0},			//ス
	{0x20,0x20,0xFE,0x22,0x24,0x20,0x1E,0x0},		//セ
	{0x42,0x22,0x22,0x2,0x4,0x8,0x30,0x0},			//ソ
	{0x10,0x1E,0x22,0x5A,0x4,0x8,0x30,0x0},			//タ
	{0xC,0x70,0x10,0xFE,0x10,0x10,0x20,0x0},		//チ
	{0x0,0xA2,0x52,0x52,0x4,0x8,0x30,0x0},			//ツ
	{0x7C,0x0,0xFE,0x10,0x10,0x10,0x20,0x0},		//テ
	{0x20,0x20,0x20,0x38,0x24,0x20,0x20,0x0},		//ト
	{0x10,0x10,0xFE,0x10,0x10,0x20,0x40,0x0},		//ナ
	{0x0,0x7C,0x0,0x0,0x0,0xFE,0x0,0x0},			//ニ
	{0x7C,0x4,0x34,0x8,0x18,0x24,0xC0,0x0},			//ヌ
	{0x10,0x7C,0x8,0x10,0x34,0xD2,0x10,0x0},		//ネ
	{0x4,0x4,0x4,0x8,0x10,0x20,0xC0,0x0},			//ノ
	{0x0,0x28,0x24,0x24,0x42,0x42,0x82,0x0},		//ハ
	{0x40,0x40,0x46,0x78,0x40,0x40,0x3E,0x0},		//ヒ
	{0x0,0x7E,0x2,0x2,0x4,0x8,0x30,0x0},			//フ
	{0x0,0x20,0x50,0x88,0x4,0x2,0x0,0x0},			//ヘ
	{0x10,0x10,0xFE,0x10,0x54,0x92,0x30,0x0},		//ホ
	{0x0,0xFE,0x2,0x4,0x28,0x10,0x8,0x0},			//マ
	{0x70,0xC,0x20,0x18,0x0,0x70,0xC,0x0},			//ミ
	{0x10,0x10,0x20,0x20,0x48,0x44,0xFA,0x0},		//ム
	{0x4,0x4,0x74,0x8,0x14,0x24,0xC0,0x0},			//メ
	{0x3C,0x10,0x10,0x7E,0x10,0x10,0xE,0x0},		//モ
	{0x20,0x2E,0xF2,0x24,0x10,0x10,0x10,0x0},		//ヤ
	{0x0,0x78,0x8,0x8,0x8,0xFE,0x0,0x0},			//ユ
	{0x0,0x7E,0x2,0x3E,0x2,0x2,0x7E,0x0},			//ヨ
	{0x3C,0x0,0x7E,0x2,0x4,0x8,0x30,0x0},			//ラ
	{0x44,0x44,0x44,0x44,0x4,0x8,0x30,0x0},			//リ
	{0x8,0x28,0x28,0x28,0x2A,0x4C,0x88,0x0},		//ル
	{0x20,0x20,0x20,0x22,0x24,0x28,0x30,0x0},		//レ
	{0x0,0x7E,0x42,0x42,0x42,0x42,0x7E,0x0},		//ロ
	{0x0,0x7E,0x42,0x2,0x4,0x8,0x30,0x0},			//ワ
	{0x40,0x20,0x2,0x2,0x4,0x8,0x70,0x0},			//ン
	{0x24,0x5E,0xC4,0x56,0x56,0x50,0x5E,0x0},		//値
	{0x7E,0x42,0x42,0x7E,0x42,0x42,0x7E,0x0},		//日
	{0x3E,0x22,0x3E,0x22,0x3E,0x42,0x86,0x0},		//月
	{0x10,0x10,0x54,0x58,0x10,0x28,0xC6,0x0},		//火
	{0x10,0x12,0xF4,0x38,0x54,0x92,0x30,0x0},		//水
	{0x10,0xFE,0x10,0x38,0x54,0x92,0x10,0x0},		//木
	{0x10,0x28,0xFE,0x10,0xFE,0x54,0xFE,0x0},		//金
	{0x10,0x10,0x7C,0x10,0x10,0x10,0xFE,0x0},		//土
	{0x40,0x7E,0x90,0x7C,0x50,0xFE,0x10,0x0},		//年
	{0x8,0xFC,0xA8,0xFE,0xA4,0xFE,0x14,0x0},		//時
	{0x28,0x28,0x44,0xBE,0x14,0x24,0xCC,0x0},		//分
	{0xC8,0x5C,0xDA,0x68,0xCA,0xC4,0x58,0x0},		//秒
	{0xFE,0x92,0x92,0xFE,0x82,0x82,0x86,0x0},		//円
	{0x8,0x7E,0x54,0x7E,0x54,0x48,0xB6,0x0},		//度
	{0x48,0x78,0x58,0xBC,0x1A,0x28,0xC8,0x0},		//外
	{0x10,0xFE,0x92,0xAA,0xC6,0x82,0x86,0x0},		//内
	{0xBC,0x24,0xBC,0x0,0xBC,0xAC,0xFE,0x0},		//温
	{0xBE,0x22,0xBE,0x14,0xB6,0x94,0xBE,0x0},		//湿
	{0xFE,0xAA,0xF2,0xAE,0xEE,0x0,0xAA,0x0},		//照
	{0xEE,0xAA,0xEE,0xAA,0xEE,0x12,0x26,0x0},		//明
	{0x38,0xFE,0xB6,0x7C,0x54,0x7C,0xE,0x0},		//電
	{0x40,0x7E,0xB8,0x7C,0x54,0x24,0xD2,0x0},		//気
	{0x7E,0x48,0x48,0x5C,0x48,0x48,0xBE,0x0},		//圧
	{0x84,0x14,0x9E,0x36,0x94,0x92,0x9E,0x0},		//池
	{0xBA,0x2E,0xBE,0x2E,0xBA,0x82,0xAA,0x0},		//測
	{0x10,0xFE,0xBA,0x10,0x5C,0x50,0xBE,0x0},		//定
	{0x10,0xFE,0xB2,0x28,0x74,0x10,0xFE,0x0},		//室
	{0x44,0xE4,0x1E,0xC4,0x4,0xE4,0xE4,0x0},		//計
	{0x22,0xFA,0x4A,0xAA,0x52,0x22,0xD6,0x0},		//刻
	{0x10,0xFE,0x92,0x92,0xFE,0x10,0x10,0x0},		//中
	{0x10,0x10,0x1C,0x10,0x10,0x10,0xFE,0x0},		//上
	{0xFE,0x10,0x18,0x14,0x10,0x10,0x10,0x0},		//下
	{0x10,0x28,0xDE,0x0,0x7C,0x8,0x30,0x0},			//今
	{0xFE,0x10,0x7C,0x10,0x10,0x28,0xC6,0x0},		//天
	{0x44,0x54,0xDE,0x76,0x54,0xD2,0x1E,0x0},		//地
	{0x70,0x10,0x10,0x28,0x28,0x44,0x82,0x0},		//入
	{0x5E,0x4A,0xEA,0x4A,0x4A,0x6A,0x16,0x0}		//切
};
#endif

byte font_kanji_read(byte id, byte x){							// id=0～15(外字漢字), x=0～7(画素)
	byte ret=0x00;

	#ifdef EEPROM_WRITE
	for(byte yy=0;yy<8;yy++){
		ret |= ((font_kanji[id][yy]>>(7-x))&0x01)<<(yy);
	}
	#endif
	return(ret);
}

byte font_kanji_init(){
	byte c,i,j,ret=0;
	
	#ifdef EEPROM_WRITE
	for(i=0;i<KANJI_NUM;i++){
		for(j=0;j<7;j++){
			c=font_kanji_read(i,j);
			if(c != EEPROM.read(i*7+j)){
				EEPROM.write(i*7+j,c);
				ret++;
			}
		}
	}
	#endif
	return(ret);
}

void utf_del_uni(char *s){
	byte i=0;
	byte j=0;
	while(s[i]!='\0'){
		if((byte)s[i]==0xEF){
			if((byte)s[i+1]==0xBE) s[i+2] += 0x40;
			i+=2;
		}
		s[j]=s[i];
		i++;
		j++;
	}
	s[j]=s[i];
}

C(33) = { P(1,1), P(1,2), P(1,3), P(1,4), P(1,5), P(1,7), END };
C(39) = { P(1,3), P(2,1), P(2,2), END };
C(44) = { P(1,7), P(2,5), P(2,6), END };
C(45) = { P(1,4), P(2,4), P(3,4), END };
C(46) = { P(1,7), END };
C(47) = { P(1,6), P(2,5), P(3,4), P(4,3), END };
C(48) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,7), P(3,1), P(3,7), P(4,2), P(4,3), P(4,4), P(4,5), P(4,6), END };
C(49) = { P(1,2), P(1,7), P(2,1), P(2,2), P(2,3), P(2,4), P(2,5), P(2,6), P(2,7), P(3,7), END };
C(50) = { P(0,2), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,3), P(4,7), END };
C(51) = { P(0,2), P(0,6), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,3), P(4,5), P(4,6), END };
C(52) = { P(0,4), P(0,5), P(1,3), P(1,5), P(2,2), P(2,5), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), P(4,5), END };
C(53) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,1), P(4,5), P(4,6), END };
C(54) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,5), P(4,6), END };
C(55) = { P(0,1), P(1,1), P(2,1), P(2,5), P(2,6), P(2,7), P(3,1), P(3,3), P(3,4), P(4,1), P(4,2), END };
C(56) = { P(0,2), P(0,3), P(0,5), P(0,6), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,3), P(4,5), P(4,6), END };
C(57) = { P(0,2), P(0,3), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,4), P(3,7), P(4,2), P(4,3), P(4,4), P(4,5), P(4,6), END };
C(58) = { P(0,3), P(0,6), P(1,3), P(1,6), P(0,2), P(0,5), P(1,2), P(1,5), END };
C(65) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(2,1), P(2,4), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(66) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,2), P(3,3), P(3,5), P(3,6), END };
C(67) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,7), P(3,2), P(3,6), END };
C(68) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,7), P(2,1), P(2,7), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), END };
C(69) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,7), END };
C(70) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(2,1), P(2,4), P(3,1), END };
C(71) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,5), P(2,7), P(3,2), P(3,5), P(3,6), P(3,7), END };
C(72) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,4), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(73) = { P(0,1), P(0,7), P(1,1), P(1,2), P(1,3), P(1,4), P(1,5), P(1,6), P(1,7), P(2,1), P(2,7), END };
C(74) = { P(0,7), P(1,1), P(1,7), P(2,1), P(2,2), P(2,3), P(2,4), P(2,5), P(2,6), P(3,1), END };
C(75) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,3), P(2,5), P(3,1), P(3,2), P(3,6), P(3,7), END };
C(76) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,7), P(2,7), P(3,7), END };
C(77) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,2), P(2,3), P(3,2), P(4,1), P(4,2), P(4,3), P(4,4), P(4,5), P(4,6), P(4,7), END };
C(78) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,3), P(2,4), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(79) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,7), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), END };
C(80) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(2,1), P(2,4), P(3,2), P(3,3), END };
C(81) = { P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,1), P(1,7), P(2,1), P(2,6), P(3,2), P(3,3), P(3,4), P(3,5), P(3,7), END };
C(82) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,1), P(1,4), P(2,1), P(2,4), P(2,5), P(3,2), P(3,3), P(3,6), P(3,7), END };
C(83) = { P(0,2), P(0,3), P(0,6), P(1,1), P(1,4), P(1,7), P(2,1), P(2,4), P(2,7), P(3,2), P(3,5), P(3,6), END };
C(84) = { P(0,1), P(1,1), P(2,1), P(2,2), P(2,3), P(2,4), P(2,5), P(2,6), P(2,7), P(3,1), P(4,1), END };
C(85) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,7), P(2,7), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), END };
C(86) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(1,6), P(2,7), P(3,6), P(4,1), P(4,2), P(4,3), P(4,4), P(4,5), END };
C(87) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(1,7), P(2,4), P(2,5), P(2,6), P(3,7), P(4,1), P(4,2), P(4,3), P(4,4), P(4,5), P(4,6), END };
C(88) = { P(0,1), P(0,2), P(0,6), P(0,7), P(1,3), P(1,5), P(2,4), P(3,3), P(3,5), P(4,1), P(4,2), P(4,6), P(4,7), END };
C(89) = { P(0,1), P(0,2), P(1,3), P(2,4), P(2,5), P(2,6), P(2,7), P(3,3), P(4,1), P(4,2), END };
C(90) = { P(0,1), P(0,6), P(0,7), P(1,1), P(1,5), P(1,7), P(2,1), P(2,4), P(2,7), P(3,1), P(3,3), P(3,7), P(4,1), P(4,2), P(4,7), END };

#ifdef LOWERCASE
C(97) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(98) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(1,7), P(2,4), P(2,7), P(3,5), P(3,6), END };
C(99) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), END };
C(100) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), P(3,1), P(3,2), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(101) = { P(0,4), P(0,5), P(0,6), P(1,3), P(1,5), P(1,7), P(2,3), P(2,5), P(2,7), P(3,4), P(3,5), P(3,7), END };
C(102) = { P(0,4), P(1,2), P(1,3), P(1,4), P(1,5), P(1,6), P(1,7), P(2,1), P(2,4), P(3,1), END };
C(103) = { P(0,4), P(0,5), P(0,8), P(1,3), P(1,6), P(1,8), P(2,3), P(2,6), P(2,8), P(3,3), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(104) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,4), P(3,5), P(3,6), P(3,7), END };
C(105) = { P(0,2), P(0,4), P(0,5), P(0,6), P(0,7), END };
C(106) = { P(0,8), P(1,8), P(2,2), P(2,4), P(2,5), P(2,6), P(2,7), END };
C(107) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), P(1,5), P(2,4), P(2,6), P(3,7), END };
C(108) = { P(0,1), P(0,2), P(0,3), P(0,4), P(0,5), P(0,6), P(0,7), END };
C(109) = { P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,5), P(2,6), P(2,7), P(3,4), P(4,5), P(4,6), P(4,7), END };
C(110) = { P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,4), P(3,5), P(3,6), P(3,7), END };
C(111) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), P(3,5), P(3,6), END };
C(112) = { P(0,4), P(0,5), P(0,6), P(0,7), P(0,8), P(1,4), P(1,7), P(2,4), P(2,7), P(3,5), P(3,6), END };
C(113) = { P(0,5), P(0,6), P(1,4), P(1,7), P(2,4), P(2,7), P(3,4), P(3,5), P(3,6), P(3,7), P(3,8), END };
C(114) = { P(0,4), P(0,5), P(0,6), P(0,7), P(1,4), P(2,4), END };
C(115) = { P(0,4), P(0,7), P(1,3), P(1,5), P(1,7), P(2,3), P(2,5), P(2,7), P(3,3), P(3,6), END };
C(116) = { P(0,4), P(1,3), P(1,4), P(1,5), P(1,6), P(1,7), P(2,4), P(2,7), P(3,7), END };
C(117) = { P(0,4), P(0,5), P(0,6), P(1,7), P(2,7), P(3,4), P(3,5), P(3,6), P(3,7), END };
C(118) = { P(0,4), P(0,5), P(0,6), P(1,7), P(2,4), P(2,5), P(2,6), END };
C(119) = { P(0,4), P(0,5), P(0,6), P(1,7), P(2,4), P(2,5), P(2,6), P(3,7), P(4,4), P(4,5), P(4,6), P(4,7), END };
C(120) = { P(0,4), P(0,7), P(1,5), P(1,6), P(2,4), P(2,7), END };
C(121) = { P(0,4), P(0,5), P(1,6), P(2,7), P(3,4), P(3,5), P(3,6), P(0,8), P(1,8), END };
C(122) = { P(0,4), P(0,7), P(1,4), P(1,6), P(1,7), P(2,4), P(2,5), P(2,7), P(3,4), P(3,7), END };
#endif

const unsigned char fontMin=33;
#ifdef LOWERCASE
const unsigned char fontMax=122;
#else
const unsigned char fontMax=90;
#endif

PROGMEM const uint8_t* const font[fontMax-fontMin+1] = {
  L(33)  /*!*/, 0, 0, 0, 0, 0,
  L(39)  /*'*/, 0, 0, 0, 0,
  L(44)  /*,*/,
  L(45)  /*-*/,
  L(46)  /*.*/,
  L(47)  /*-*/,
  L(48)  /*0*/, L(49)  /*1*/, L(50)  /*2*/, L(51)  /*3*/, L(52)  /*4*/,
  L(53)  /*5*/, L(54)  /*6*/, L(55)  /*7*/, L(56)  /*8*/, L(57)  /*9*/,
  L(58)  /*:*/, 0, 0, 0, 0, 0, 0,
  L(65)  /*A*/, L(66)  /*B*/, L(67)  /*C*/, L(68)  /*D*/, L(69)  /*E*/,
  L(70)  /*F*/, L(71)  /*G*/, L(72)  /*H*/, L(73)  /*I*/, L(74)  /*J*/,
  L(75)  /*K*/, L(76)  /*L*/, L(77)  /*M*/, L(78)  /*N*/, L(79)  /*O*/,
  L(80)  /*P*/, L(81)  /*Q*/, L(82)  /*R*/, L(83)  /*S*/, L(84)  /*T*/,
  L(85)  /*U*/, L(86)  /*V*/, L(87)  /*W*/, L(88)  /*X*/, L(89)  /*Y*/,
  L(90)  /*Z*/,
#ifdef LOWERCASE
  0, 0, 0, 0, 0, 0,
  L(97)  /*a*/, L(98)  /*b*/, L(99)  /*c*/, L(100) /*d*/, L(101) /*e*/,
  L(102) /*f*/, L(103) /*g*/, L(104) /*h*/, L(105) /*i*/, L(106) /*j*/,
  L(107) /*k*/, L(108) /*l*/, L(109) /*m*/, L(110) /*n*/, L(111) /*o*/,
  L(112) /*p*/, L(113) /*q*/, L(114) /*r*/, L(115) /*s*/, L(116) /*t*/,
  L(117) /*u*/, L(118) /*v*/, L(119) /*w*/, L(120) /*x*/, L(121) /*y*/,
  L(122) /*z*/,
#endif
};

/* -----------------------------------------------------------------  */
/** Draws a figure (0-9). You should call it with c=1, 
 * wait a little them call it again with c=0
 * @param figure is the figure [0-9]
 * @param x,y coordinates, 
 * @param c is 1 or 0 to draw or clear it
 */
uint8_t font_draw(char letter, int x) {
  uint8_t maxx=0, data;
  const uint8_t* character;

	if( (byte)letter == 0xB0 ) letter='-';
	if( (byte)letter == 0xA1 || (byte)letter == 0xA5) letter='.';
	if( (byte)letter == 0xA4 ) letter=',';
	if( (byte)letter == 0xA2 ) letter='(';
	if( (byte)letter == 0xA3 ) letter=')';
	if( (byte)letter == 0xDE ) {
		if(x>1)leds[x-2]	&= 0b11111000;
		if(x>0)leds[x-1]	 = 0b00000110;
		leds[x+0] 			 = 0b00001000;
		leds[x+1] 			 = 0b00000011;
		leds[x+2] 			 = 0b00000100;
		return(3);
	}
	if( (byte)letter == 0xDF ){
		if(x>0)leds[x-1] = 0b00000010;
		leds[x+0] = 0b00000101;
		leds[x+1] = 0b00000010;
		return(3);
	}
	if(font_kana_mode && letter>='0' && letter<='F' ){
		if( letter >= 'A' ){
			font_kanji_code += (byte)(letter - 'A')+10;
		}else{
			if(letter<='9') font_kanji_code += (byte)(letter - '0');
		}
		if(font_kanji_code < 0x80){
			font_kanji_code *= 16;
			return(0);
		}
		letter=(char)font_kanji_code;
		font_kanji_code=0x00;
	}
	if( letter=='{' ){
		font_kana_mode=1;
		return(0);
	}
	if( letter=='}' ){
		font_kana_mode=0;
		return(0);
	}
	
	if( (byte)letter >= 0x80){							// EEPROM内の外字コードとする
		byte letter_eep;							// EEPROM内の文字コード
		if( (byte)letter >= 0xA6 && (byte)letter <= 0xDD ){		// あいうえお
			letter_eep = (byte)letter - 0xA6;
			if( (byte)letter > 0xB0 ) letter_eep--;
			/*
			Serial.print((byte)letter,HEX);
			Serial.print("->");
			Serial.println(letter_eep,HEX);
			*/
			if( font_kana_mode && letter_eep ) letter_eep += 54;	// 「を」以外の文字をカタカナへ
		}else if( (byte)letter <= 0x9F ){
			letter_eep = ((byte)letter - 0x80) + 109;
		}else if( (byte)letter >= 0xFB){
			letter_eep = ((byte)letter - 0xFB) + 141;		// 今天地入切
		}
		for(byte xx=0;xx<7;xx++){
			leds[x+xx]=EEPROM.read( letter_eep*7 + xx );
		}
		return(8);
	}

	if ( letter==' ' ||
		((byte)letter>33 && (byte)letter<39)||
		((byte)letter>39 && (byte)letter<44)||
		((byte)letter>58 && (byte)letter<65)||
		((byte)letter>90 && (byte)letter<97)	) return 3+2;
	if ( (byte)letter<fontMin ||
		 (byte)letter>fontMax  ) return 0;

	character = (const uint8_t *)pgm_read_word_near(&font[(byte)letter-fontMin]);
	data = pgm_read_byte_near(character++);

	while (data != END) {
		uint8_t charCol = data >> 4, charRow = data & 15;
		if (charCol>maxx) maxx=charCol;
		if (
			charCol + x <DISPLAY_COLS && 
			charCol + x >=0 && 
			charRow - 1 <DISPLAY_ROWS && 		// y=-1を埋め込み
			charRow - 1 >=0						// y=-1を埋め込み
		) {
			leds[charCol+x] |= 0x01<<(charRow - 1); // y=-1を埋め込み
		} 
		data = pgm_read_byte_near(character++);
	}
	return maxx+2;
}

/*
資料

キャラクタ文字コード（本来）

		00 10 20 30 40 50 60 70 80 90 A0 B0 C0 D0 E0 F0
00		NL DE SP ０ ＠ Ｐ ‘ ｐ 　 　 　 ー タ ミ 　 　 
01		SH D1 ！ １ Ａ Ｑ ａ ｑ 　 　 。 ア チ ム 　 　 
02		SX D2 ” ２ Ｂ Ｒ ｂ ｒ 　 　 「 イ ツ メ 　 　 
03		EX D3 ＃ ３ Ｃ Ｓ ｃ ｓ 　 　 」 ウ テ モ 　 　 
04		ET D4 ＄ ４ Ｄ Ｔ ｄ ｔ 　 　 、 エ ト ヤ 　 　 
05		EQ NK ％ ５ Ｅ Ｕ ｅ ｕ 　 　 ・ オ ナ ユ 　 　 
06		AK SN ＆ ６ Ｆ Ｖ ｆ ｖ 　 　 ヲ カ ニ ヨ 　 　 
07		BL EB ’ ７ Ｇ Ｗ ｇ ｗ 　 　 ァ キ ヌ ラ 　 　 
08		BS CN （ ８ Ｈ Ｘ ｈ ｘ 　 　 ィ ク ネ リ 　 　 
09		HT EM ） ９ Ｉ Ｙ ｉ ｙ 　 　 ゥ ケ ノ ル 　 　 
0A		LF SB ＊ ： Ｊ Ｚ ｊ ｚ 　 　 ェ コ ハ レ 　 　 
0B		HM EC ＋ ； Ｋ ［ ｋ ｛ 　 　 ォ サ ヒ ロ 　 　 
0C		CL → ， ＜ Ｌ ￥ ｌ ｜ 　 　 ャ シ フ ワ 　 　 
0D		CR ← － ＝ Ｍ ］ ｍ ｝ 　 　 ュ ス ヘ ン 　 　 
0E		SO ↑ ． ＞ Ｎ ＾ ｎ ￣ 　 　 ョ セ ホ ゛ 　 　 
0F		SI ↓ ／ ？ Ｏ ＿ ｏ DL 　 　 ッ ソ マ ゜ 　 　 

	http://hp.vector.co.jp/authors/VA008536/data/ascii.html

EEPROM 記録可能数＝146文字
日本語割り当て 1E～1F、80～FF(128),10～1F(16)、0E～0F(2)

アルファベット文字およびDE(゛)とDF(゜)はプログラム側で保持

ひらがな							55文字
カタカナ							54文字
							合計	59文字	残37文字

曜日	月～日						7文字
時刻	年時分秒					4文字
単位	円度値						3文字
測定	温湿照明電気圧				7文字
単語前	池測定室計刻				6文字
度合い	上中下入切外内				7文字
その他	今天地						3文字

							合計	37文字	残り0文字

呼び出し

		00 10 20 30 40 50 60 70 80 90 A0 B0 C0 D0 E0 F0
00		NL DE SP ０ ＠ Ｐ ‘ ｐ 値 温 　(ー)た み 　 　 
01		SH D1 ！ １ Ａ Ｑ ａ ｑ 月 湿(。)あ ち む 　 　 
02		SX D2 ” ２ Ｂ Ｒ ｂ ｒ 火 照(「)い つ め 　 　 
03		EX D3 ＃ ３ Ｃ Ｓ ｃ ｓ 水 明(」)う て も 　 　 
04		ET D4 ＄ ４ Ｄ Ｔ ｄ ｔ 木 電(、)え と や 　 　 
05		EQ NK ％ ５ Ｅ Ｕ ｅ ｕ 金 気(・)お な ゆ 　 　 
06		AK SN ＆ ６ Ｆ Ｖ ｆ ｖ 土 圧(を)か に よ 　 　 
07		BL EB ’ ７ Ｇ Ｗ ｇ ｗ 日 池 ぁ き ぬ ら 　 　 
08		BS CN （ ８ Ｈ Ｘ ｈ ｘ 年 測 ぃ く ね り 　 　 
09		HT EM ） ９ Ｉ Ｙ ｉ ｙ 時 定 ぅ け の る 　 　 
0A		LF SB ＊ ： Ｊ Ｚ ｊ ｚ 分 室 ぇ こ は れ 　 　 
0B		HM EC ＋ ； Ｋ ［ ｋ ｛ 秒 計 ぉ さ ひ ろ 　 今 
0C		CL → ， ＜ Ｌ ￥ ｌ ｜ 円 刻 ゃ し ふ わ 　 天 
0D		CR ← － ＝ Ｍ ］ ｍ ｝ 度 中 ゅ す へ ん 　 地 
0E		SO ↑ ． ＞ Ｎ ＾ ｎ ￣ 外 上 ょ せ ほ(゛)　 切 
0F		SI ↓ ／ ？ Ｏ ＿ ｏ DL 内 下 っ そ ま(゜)UT 入 

※カタカナは「｛」「｝」でくくって表示する
※A6「ヲ」はひらがな「を」のみ表示可能


文字コード参考情報

A date is specified by the ISO week-numbering year in the format YYYY, a week number
in the format ww prefixed by the letter 'W', and the weekday number, a digit d from
1 through 7, beginning with Monday and ending with Sunday.

										http://en.wikipedia.org/wiki/ISO_week_date

NEC	P6code 1024文字
							http://p6ers.net/mm/pc-6001/doc/p6mkII_kanjicode.html

Sharp MZ 13文字

		D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF
		日 月 火 水 木 金 土 生 年 時 分 秒 円 ￥ ￡ ～
		

UTF8の解析

	sprintf(rx,"ｱｲｳ");
	for(i=0;i<15;i++)Serial.println((byte)rx[i],HEX);

	uni   c'      (c)
	EF BD B1	あ(B1)
	EF BD B2	い(B2)
	EF BD B3	う(B3)
	EF BE 8E	ほ(CE) = c' + 0x40
	
	・EFから始まる場合はUTF8長は3バイトである。
	・BDが続く場合は3バイト目は半角カナそのままのコード
	・BEが続く場合は3バイト目に0x40を加算すれば半角カナのコードになる
*/

/*
参考文献・ライセンスについて

本スケッチの作成に下記の資料とデータを使用しました。

・LoLシールド用 Font drawing library

	https://github.com/johnmccombs/arduino-libraries/blob/master/LolShield/Font.cpp

	Font drawing library

	Copyright 2009/2010 Benjamin Sonntag <benjamin@sonntag.fr> http://benjamin.sonntag.fr/

	History:
	2010-01-01 - V0.0 Initial code at Berlin after 26C3

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.

・Arduinoでドットマトリックス・美咲フォントを表示してみる

	http://www.surveytec.com/cgi-bin/diarypic/diary.cgi?no=32
	
	本ユーティリティに下記の美咲フォントを使用しています
	
・美咲フォントFONTX版

	http://www.geocities.jp/littlimi/
	
	These fonts are free softwares.
	Unlimited permission is granted to use, copy, and distribute it,
	with or without modification, either commercially and noncommercially.
	THESE FONTS ARE PROVIDED "AS IS" WITHOUT WARRANTY.

	これらのフォントはフリー（自由な）ソフトウエアです。
	あらゆる改変の有無に関わらず、また商業的な利用であっても、自由にご利用、
	複製、再配布することができますが、全て無保証とさせていただきます。

	Copyright(C) 2002-2012 Num Kadoma

・FONTX

	http://www.hmsoft.co.jp/lepton/software/dosv/fontx.htm
	
	このパッケージに含まれている全てのファイルの著作権は作者が保持します。
	ただし作者は本パッケージの所持者に対して、
		1.プログラムを動作させること。
		2.本パッケージを無変更で第三者に配布すること。
	を認めています。またこのパッケージを使用・配布した結果、いかなる損害が
	発生しようとも作者はそれを補償しません。
	これらに同意できない場合は使用・配布をしないでください。
*/

