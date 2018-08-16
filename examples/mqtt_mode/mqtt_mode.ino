#include <Microduino_Ai.h>
#include <aJSON.h>

#define WIFI_SSID   "ssid"
#define WIFI_PWD    "password"
#define MQTTSERVER   "mqtt://mCotton.microduino.cn"
#define ID   "58f881ae42c66e00013cf445"
#define USER   "58f881ae42c66e00013cf445"
#define PASS   "4KOaAZ3Ng3e5CMD9f5SUB5O7Ag85HX2X"
#define SUBSCRIBTOPIC   "ca/58f881ae42c66e00013cf445"
#define SUBSCRIBTOPICP   "cp/58f881ae42c66e00013cf445"
#define PUBLISHTOPIC   "dp/58f881ae42c66e00013cf445"

#define INTERVAL_sensor 2000
unsigned long sensorlastTime = millis();

MicroAi mAi(&Serial1);

String jsonData;
char strBuf[64] = {0};

void setup() {
  Serial.begin(115200);

  Serial.print("MicroAi set mode: MQTT.");
  Serial.println("start initiation. ");     
  if(mAi.begin(MODE_MQTT)){
    Serial.println("init OK!");  
  }else{
    Serial.println("init failed!");
    while(1);  
  }
  
  if(mAi.setWifi(WIFI_SSID, WIFI_PWD)){
    Serial.println("set wifi ssid and password OK!");  
  }else{
    Serial.println("set wifi ssid and password failed!");
    while(1);  
  }
  Serial.print("wifi connecting"); 
  while(mAi.getRssi()==0){
    Serial.print("."); 
    delay(1000);
  }
  Serial.println("\r\nwifi connected.");     

  if (mAi.mqttSetServer(MQTTSERVER)) {
    Serial.println(F("set mqtt server OK!"));
  } else {
    Serial.println(F("set mqtt server failed!"));
    while(1);  
  }

  if (mAi.mqttConnect(ID, USER, PASS)) {
    Serial.println(F("set mqtt connect params OK!"));
  } else {
    Serial.println(F("set mqtt connect params failed!"));
    while(1);  
  }

  Serial.print("mqtt server connecting"); 
  while(mAi.mqttGetStatus()<=0){
    Serial.print("."); 
    delay(1000);
  }
  Serial.println("\r\nmqtt server connected.");    

  if (mAi.mqttSetSubscrib(SUBSCRIBTOPIC)) {
    Serial.println(F("set mqtt subscrib ca topic OK!"));
  } else {
    Serial.println(F("set mqtt subscrib ca topic failed"));
  }

  if (mAi.mqttSetSubscrib(SUBSCRIBTOPICP)) {
    Serial.println(F("set mqtt subscrib cp topic OK!"));
  } else {
    Serial.println(F("set mqtt subscrib cp topic failed"));
  }

  char *str = "{\"Humidity\":66.66}";
  if (mAi.mqttPublish(PUBLISHTOPIC, str)) {
    Serial.println(F("mqtt publish OK!"));
  } else {
    Serial.println(F("mqtt publish failed!"));
  }

  delay(1000);
}

void loop() {

  if(mAi.mqttQuery(strBuf)>0){
    Serial.print("recv: ");
    Serial.println(strBuf);
  }
  
  if (mAi.mqttGetStatus()>1) {
    if (sensorlastTime > millis())
      sensorlastTime = millis();
    if (millis() - sensorlastTime > INTERVAL_sensor) {
      float temp = map(analogRead(A2), 0, 1023, 0, 255);
      float humi = map(analogRead(A1), 0, 1023, 0, 255);
      float lightness = map(analogRead(A0), 0, 1023, 0, 255);
      jsonData = "{\"Humidity\":";
      jsonData += String(humi);
      jsonData += ",\"Temperature\":";
      jsonData += String(temp);
      jsonData += ",\"Lightness\":";
      jsonData += String(lightness);
      jsonData += "}";

      int str_len = jsonData.length() + 1; 
      jsonData.toCharArray(strBuf, str_len);

      if (mAi.mqttPublish(PUBLISHTOPIC, strBuf)) {
        Serial.println(F("mqtt publish OK!"));
      } else {
        Serial.println(F("mqtt publish failed!"));
      }
      sensorlastTime = millis();
    }
  }
  delay(100);
}





