#include <Arduino.h>
// Bluetooth用設定
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
#include "Secret.h"
#include "Esp32BT.h"

const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 16;
const int IN4 = 17;
const int BUZZER = 27;

// アナログ値読み取りピン
// const int analogPin1 = 13;

// 関数宣言
void advance(int ad_int);
void back(int bk_int);
void motorStop();
void lightOn();
void lightOff();
void buzzerOn();
void buzzerOff();

void setup() {
  Serial.begin(115200);
  Serial.println("BOOT_ESP32");
  /*mac addressの取得*/
  EspBTSetUpClass EspBTSetObj;
  EspBTSetObj.getMacAddress();
  SerialBT.begin("ESP32");
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

int motorValue = 0;
int i = 0;
int receivedData;// 受信データ
int bukRecevedData = 0;// バックアップ用データ

/*受信用構造体*/
struct ReceiveData {
  int slideVal1;
  int slideVal2;
  int sld_sw1;
  int sld_sw2;
  int sld_sw3;
  int sld_sw4;
  int sw1;
  int sw2;
  int sw3;
  int sw4;
  int sw5;
  int sw6;
  int sw7;
  int sw8;
};
// 受信したデータを格納するためのバッファを作成(一時的に保存するため？)
uint8_t buffer[sizeof(ReceiveData)];
int bytesReceived;
int j = 0;
// 受信データを格納するための構造体作成
ReceiveData receiveData;
ReceiveData beforeReceiveData;

void loop() {
  /*
  通信途切れ判定
  停止時はモーターなど停止させる
  通信時はメインプログラム
  */
  if (SerialBT.available()) {
    bytesReceived = SerialBT.readBytes(buffer, sizeof(buffer));
    if (bytesReceived = sizeof(ReceiveData)) {
      /*
      受信データを構造体にコピー
      ひとつ前のデータを保存
      */ 
      memcpy(&receiveData, buffer, sizeof(ReceiveData));
      if (j == 0) {
        beforeReceiveData = receiveData;
        j = 1;
      }
      
      /*モーター出力制限(2.9V)*/
      if (receiveData.slideVal1 > 245)
        {
          receiveData.slideVal1 = 245;
        }

      /*モータ出力操作、前進後進切替*/
      if (receiveData.sld_sw1 != beforeReceiveData.sld_sw1) {
        motorStop();
        Serial.print("stop");
      }else if (receiveData.sld_sw1 == 0) {
        advance(receiveData.slideVal1);
      }else if (receiveData.sld_sw1 == 1) {
        back(receiveData.slideVal1);
      }
      beforeReceiveData = receiveData;
      
      /*ライト操作*/
      if (receiveData.sw1 == 0 && receiveData.sld_sw2 == 0) {
        lightOff();
      }else if (receiveData.sw1 == 0 || receiveData.sld_sw2 == 0) {
        lightOn();
      }else {
        lightOff();
      }

      /*ブザー操作*/
      if (receiveData.sw2 == 0) {
        buzzerOn();
      }else{
        buzzerOff();
      }
      
      #if 1
      Serial.print(receiveData.sld_sw1);
      Serial.print(receiveData.sld_sw2);
      Serial.print(receiveData.sld_sw3);
      Serial.print(receiveData.sld_sw4);
      Serial.print(receiveData.sw1);
      Serial.print(receiveData.sw2);
      Serial.print(receiveData.sw3);
      Serial.print(receiveData.sw4);
      Serial.print(receiveData.sw5);
      Serial.print(receiveData.sw6);
      Serial.print(receiveData.sw7);
      Serial.print(receiveData.sw8);
      Serial.print(receiveData.slideVal1);
      Serial.print(receiveData.slideVal2);
      Serial.print("  ");
      Serial.print(beforeReceiveData.sld_sw1);
      Serial.print(beforeReceiveData.sld_sw2);
      Serial.print(beforeReceiveData.sld_sw3);
      Serial.print(beforeReceiveData.sld_sw4);
      Serial.print(beforeReceiveData.sw1);
      Serial.print(beforeReceiveData.sw2);
      Serial.print(beforeReceiveData.sw3);
      Serial.print(beforeReceiveData.sw4);
      Serial.print(beforeReceiveData.sw5);
      Serial.print(beforeReceiveData.sw6);
      Serial.print(beforeReceiveData.sw7);
      Serial.print(beforeReceiveData.sw8);
      Serial.print(beforeReceiveData.slideVal1);
      Serial.println(beforeReceiveData.slideVal2);
      #endif
    } else if (bytesReceived != sizeof(ReceiveData)) {
      /* pass */
    }
    
    
    
    SerialBT.write(0);// 通信返答用
    i = 0;
  } else {
    Serial.println("Lost");
    i++;
    /*通信途切れ20回以上でモーター停止*/
    if (i > 20){
      motorStop();
      lightOff();
    }
  }

  delay(50);
}

void advance(int ad_int) {
  /*
  モーター前進(逆になるかも)
  int ad_int:モーターの回転量
  int ad_sec:待機時間
  */
  analogWrite(IN1, ad_int);
  analogWrite(IN2, ad_int);
}

void back(int bk_int) {
  /*
  モーター後進(逆になるかも)
  int bk_int:モーターの回転量
  */
  analogWrite(IN1, bk_int);
  analogWrite(IN2, 0);
}

void motorStop() {
  /*
  モーター停止
  */
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
}

void lightOn() {
  /*
  前照灯オン
  */
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}

void lightOff() {
  /*
  前照灯オフ
  */
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void buzzerOn() {
  /*ブザー*/
  digitalWrite(BUZZER, HIGH);
}

void buzzerOff() {
  /*ブザー*/
  digitalWrite(BUZZER, LOW);
}