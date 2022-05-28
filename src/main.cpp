#include <Arduino.h>
#include <WiFi.h>
#include "cJSON.h"
#include <HTTPClient.h>

const char *ssid = "Andante"; //网络名称
const char *password = "Ll10211019"; //网络密码
HTTPClient httpClient;

//重启标志位
int i = 1;

//空气传感器问询帧
int item1[8] = {0x02,0x03,0x01,0xf4,0x00,0x08,0x04,0x31};
//土壤传感器问询帧
int item2[8] = {0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0B};

//获取空气温度监测值
double getTem(String temp) {
    int commaPosition = -1;
    String info[21];  // 用字符串数组存储
    for (int i = 0; i < 21; i++) {
        commaPosition = temp.indexOf(',');
        if (commaPosition != -1)
        {
            info[i] = temp.substring(0, commaPosition);
            temp = temp.substring(commaPosition + 1, temp.length());
        }
        else {
            if (temp.length() > 0) {  // 最后一个会执行这个
                info[i] = temp.substring(0, commaPosition);
            }
        }
    }
    return (info[5].toInt() * 256 + info[6].toInt()) / 10.0;
}


//获取空气湿度监测值
double getHem(String temp) {
    int commaPosition = -1;
    String info[21];  // 用字符串数组存储
    for (int i = 0; i < 21; i++) {
        commaPosition = temp.indexOf(',');
        if (commaPosition != -1)
        {
            info[i] = temp.substring(0, commaPosition);
            temp = temp.substring(commaPosition + 1, temp.length());
        }
        else {
            if (temp.length() > 0) {  // 最后一个会执行这个
                info[i] = temp.substring(0, commaPosition);
            }
        }
    }
    return (info[3].toInt() * 256 + info[4].toInt()) / 10.0;
}

//获取光照监测值
double getLux(String temp) {
    int commaPosition = -1;
    String info[21];  // 用字符串数组存储
    for (int i = 0; i < 21; i++) {
        commaPosition = temp.indexOf(',');
        if (commaPosition != -1)
        {
            info[i] = temp.substring(0, commaPosition);
            temp = temp.substring(commaPosition + 1, temp.length());
        }
        else {
            if (temp.length() > 0) {  // 最后一个会执行这个
                info[i] = temp.substring(0, commaPosition);
            }
        }
    }
    return ((info[17].toInt()<<16)|(info[18].toInt()&0xFFFF))/10.0;
}

//获取土壤温度监测值
double getsoilTem(String temp) {
    int commaPosition = -1;
    String info[21];  // 用字符串数组存储
    for (int i = 0; i < 21; i++) {
        commaPosition = temp.indexOf(',');
        if (commaPosition != -1)
        {
            info[i] = temp.substring(0, commaPosition);
            temp = temp.substring(commaPosition + 1, temp.length());
        }
        else {
            if (temp.length() > 0) {  // 最后一个会执行这个
                info[i] = temp.substring(0, commaPosition);
            }
        }
    }
    return (info[5].toInt() * 256 + info[6].toInt()) / 10.0;
}

//获取土壤湿度监测值
double getsoilHem(String temp) {
    int commaPosition = -1;
    String info[21];  // 用字符串数组存储
    for (int i = 0; i < 21; i++) {
        commaPosition = temp.indexOf(',');
        if (commaPosition != -1)
        {
            info[i] = temp.substring(0, commaPosition);
            temp = temp.substring(commaPosition + 1, temp.length());
        }
        else {
            if (temp.length() > 0) {  // 最后一个会执行这个
                info[i] = temp.substring(0, commaPosition);
            }
        }
    }
    return (info[3].toInt() * 256 + info[4].toInt()) / 10.0;
}

//获取Json数据包发送至串口
void getJson(double tem,double hem,double lux,double soiltem,double soilhem){
    cJSON *pRoot = cJSON_CreateObject();                         // 创建JSON根部结构体
    cJSON_AddStringToObject(pRoot,"id","1");    // 添加字符串类型数据到根部结构体
    cJSON_AddNumberToObject(pRoot,"A_temperature",tem);                   // 添加整型数据到根部结构体
    cJSON_AddNumberToObject(pRoot,"A_humidity",hem);
    cJSON_AddNumberToObject(pRoot,"L_intensity",lux);
    cJSON_AddNumberToObject(pRoot,"S_temperature",soiltem);
    cJSON_AddNumberToObject(pRoot,"S_humidity",soilhem);
    cJSON_AddStringToObject(pRoot,"information","lmh-fy");

    char *sendData = cJSON_Print(pRoot);                        // 从cJSON对象中获取有格式的JSON对象
    Serial.println(sendData);
    cJSON_free((void *) sendData);                             // 释放cJSON_Print ()分配出来的内存空间
    cJSON_Delete(pRoot);                                       // 释放cJSON_CreateObject ()分配出来的内存空间
}

//获取Json数据包发送至17gz8
void getnwJson(double tem,double hem,double lux,double soiltem,double soilhem){
    httpClient.begin("https://api.17gz8.com/server/Device/v1/api/upload-data");
    cJSON *pRoot = cJSON_CreateObject();                         // 创建JSON根部结构体
    cJSON_AddStringToObject(pRoot,"id","6270b7a5a5ef31166d0bc89b");    // 添加字符串类型数据到根部结构体
    cJSON_AddNumberToObject(pRoot,"A_temperature",tem);                   // 添加整型数据到根部结构体
    cJSON_AddNumberToObject(pRoot,"A_humidity",hem);
    cJSON_AddNumberToObject(pRoot,"L_intensity",lux);
    cJSON_AddNumberToObject(pRoot,"S_temperature",soiltem);
    cJSON_AddNumberToObject(pRoot,"S_humidity",soilhem);
    cJSON_AddStringToObject(pRoot,"information","lmh-fy");

    char *sendData = cJSON_Print(pRoot);                        // 从cJSON对象中获取有格式的JSON对象
    httpClient.addHeader("Content-Type","application/json");
    int httpCode = httpClient.POST(sendData); // 发起POST请求
    if (httpCode > 0) // 如果状态码大于0说明请求过程无异常
    {
        Serial.println(httpCode);
        if (httpCode == HTTP_CODE_OK) // 请求被服务器正常响应，等同于httpCode == 200
        {
            Serial.println("remote-successful");
        }else{
            Serial.println("error");
        }
    }
    Serial.println(sendData);
    cJSON_free((void *) sendData);                             // 释放cJSON_Print ()分配出来的内存空间
    cJSON_Delete(pRoot);                                       // 释放cJSON_CreateObject ()分配出来的内存空间
    httpClient.end();
}

//获取Json数据包发送至云端web
void getremoteJson(double tem,double hem,double lux,double soiltem,double soilhem){
    httpClient.begin("http://124.221.111.98/receiveAPI.php");
    cJSON *pRoot = cJSON_CreateObject();                         // 创建JSON根部结构体
    cJSON_AddStringToObject(pRoot,"id","1");    // 添加字符串类型数据到根部结构体
    cJSON_AddNumberToObject(pRoot,"A_temperature",tem);                   // 添加整型数据到根部结构体
    cJSON_AddNumberToObject(pRoot,"A_humidity",hem);
    cJSON_AddNumberToObject(pRoot,"L_intensity",lux);
    cJSON_AddNumberToObject(pRoot,"S_temperature",soiltem);
    cJSON_AddNumberToObject(pRoot,"S_humidity",soilhem);
    cJSON_AddStringToObject(pRoot,"information","lmh-fy");

    char *sendData = cJSON_Print(pRoot);                        // 从cJSON对象中获取有格式的JSON对象
    httpClient.addHeader("Content-Type","application/json");
    int httpCode = httpClient.POST(sendData); // 发起POST请求
    if (httpCode > 0) // 如果状态码大于0说明请求过程无异常
    {
        Serial.println(httpCode);
        if (httpCode == HTTP_CODE_OK) // 请求被服务器正常响应，等同于httpCode == 200
        {
            Serial.println("remote-successful");
        }else{
            Serial.println("error");
        }
    }
    Serial.println(sendData);
    cJSON_free((void *) sendData);                             // 释放cJSON_Print ()分配出来的内存空间
    cJSON_Delete(pRoot);                                       // 释放cJSON_CreateObject ()分配出来的内存空间
    httpClient.end();
}

//获取Json数据包发送至本地web端
void getlocalJson(double tem,double hem,double lux,double soiltem,double soilhem){
    httpClient.begin("http://192.168.43.128:8888/update");
    cJSON *pRoot = cJSON_CreateObject();                         // 创建JSON根部结构体
    cJSON_AddNumberToObject(pRoot,"lightIntenisty",lux);
    cJSON_AddNumberToObject(pRoot,"indoorTemperature",tem);                   // 添加整型数据到根部结构体
    cJSON_AddNumberToObject(pRoot,"airHumidity",hem);
    cJSON_AddNumberToObject(pRoot,"soilTemperature",soiltem);
    cJSON_AddNumberToObject(pRoot,"soilHumidity",soilhem);
    cJSON_AddStringToObject(pRoot,"information","lmh-fy");

    char *sendData = cJSON_Print(pRoot);                        // 从cJSON对象中获取有格式的JSON对象
    httpClient.addHeader("Content-Type","application/json");
    int httpCode = httpClient.POST(sendData); // 发起POST请求
    if (httpCode > 0) // 如果状态码大于0说明请求过程无异常
    {
        Serial.println(httpCode);
        if (httpCode == HTTP_CODE_OK) // 请求被服务器正常响应，等同于httpCode == 200
        {
            Serial.println("local-successful");
        }else{
            Serial.println("error");
        }
    }
    Serial.println(sendData);
    cJSON_free((void *) sendData);                             // 释放cJSON_Print ()分配出来的内存空间
    cJSON_Delete(pRoot);                                       // 释放cJSON_CreateObject ()分配出来的内存空间
    httpClient.end();
}

void setup() {
    //串口使能
    Serial.begin(9600);
    Serial2.begin(9600);
    while (!Serial);
    pinMode(2,OUTPUT);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); //关闭STA模式下wifi休眠，提高响应速度
    WiFi.begin(ssid, password); //连接网络
    while (WiFi.status() != WL_CONNECTED) //等待网络连接成功
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected, IP address: ");
    Serial.println(WiFi.localIP()); //打印模块IP
}

void loop() {
    //接收数据
    double tem = 0;
    double hem = 0;
    double lux = 0;
    double soiltem = 0;
    double soilhem = 0;

    //接收应答帧
    String data1 = "";
    String data2 = "";

    //显示是否连接上WiFi
    if(WiFi.isConnected()){
        digitalWrite(2,1);
    }else
        digitalWrite(2,0);

    //空气测试
    delay(500);  // 放慢输出频率

    for (int i = 0 ; i < 8; i++) {  // 发送
        Serial2.write(item1[i]);
    }

    delay(500);  // 等待应答帧返回
    while (Serial2.available()) {//从串口中读取数据
        unsigned char in = (unsigned char)Serial2.read();
        Serial.print(in, HEX);
        Serial.print(',');
        data1 += in;
        data1 += ',';
    }

    delay(1000);

    //土壤测试
    delay(500);  // 放慢输出频率

    for (int i = 0 ; i < 8; i++) {  // 发送
        Serial2.write(item2[i]);
    }

    delay(500);  // 等待应答帧返回

    while (Serial2.available()) {//从串口中读取数据
        unsigned char in = (unsigned char)Serial2.read();
        Serial.print(in, HEX);
        Serial.print(',');
        data2 += in;
        data2 += ',';
    }

    //提取数据
    tem = getTem(data1);
    hem = getHem(data1);
    lux = getLux(data1);
    soiltem = getsoilTem(data2);
    soilhem = getsoilHem(data2);

    //串口格式整理
    Serial.println("");
    Serial.println("");

    //获取并发送json包
    //getremoteJson(tem,hem,lux,soiltem,soilhem);
    //getlocalJson(tem,hem,lux,soiltem,soilhem);
    //getnwJson(tem,hem,lux,soiltem,soilhem);
    getJson(tem,hem,lux,soiltem,soilhem);

    //定时重启
    i++;
    if(i==30){
        ESP.restart();
    }

    delay(1000*10);
}