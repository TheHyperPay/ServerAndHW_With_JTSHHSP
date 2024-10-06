#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>
#include <SPI.h>

//==========Default Configure 선언==========
#define DEVICE_CODE 0x01
#define UNIT_TEST UNITTEST_ALL
#define NETENV ENV_HOME
#define FPS 1000/5

#define SENSOR_SERVER "route.pgass.one"
#define PORT 19991

#pragma region Type //Type 선언 모아둔 곳
typedef struct Axis6_t{
  float accelX;
  float accelY;
  float accelZ;
  float pitch;
  float yaw;
  float roll;
}Axis6_t;
#pragma endregion

#pragma region SETTING //각종 세팅 설정
//About Unit Test in Arduino sensors.
#define UNITTEST_NONE 0x0
#define UNITTEST_IMU 0x1
#define UNITTEST_NETWORK 0x2
#define UNITTEST_ALL 0xF
//Operated Location
#define ENV_HOME 0x0
#define ENV_OFFICE 0x1
#define ENV_KSHHOTSPOT 0x3

#if NETENV == ENV_HOME
const char* ssid     = "KTGiGA2G_lovehouse";
const char* password = "kim52381257";
#elif NETENV == ENV_OFFICE
const char* ssid     = "DLive5G_4C0A";
const char* password = "E0014D4C09";
#elif NETENV == ENV_KSHHOTSPOT
const char* ssid     = "マ";
const char* password = "ajsj321n";
#endif

#pragma endregion

#pragma region NOTICE //Hardware 알림 시스템
#define WARN 0x01
#define ERR 0x02

void flicker(char code)
{
  for(int x=0; x<(code >> 4); x++)
  {
    if(!(x % 4))
      delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(70);
  }
  delay(130);
  for(int x=0; x<(code & 0x0F); x++)
  {
    if(!(x % 4))
      delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(90);
    digitalWrite(LED_BUILTIN, LOW);
    delay(70);
  }
  delay(130);
}

void noticeHW(char type, char errorCode, const char* message)
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  if(type == WARN)
  {
    Serial.print("WARN: ");
    Serial.println(message);
    for(int x=0; x<1; x++)
      flicker(errorCode);
  }
  else if(type == ERR)
  {
    Serial.print("ERR: ");
    Serial.println(message);
    while(true)
      flicker(errorCode); // process Stop
  }
}
#pragma endregion

#pragma region ERRORCODE //Error Code 정리
/*
상위 4비트: 오류 타입(Net, Sensor 등)
하위 4비트: 구분
상위 4비트는 크고 오래 점멸
하위 4비트는 작고 빠르게 점멸
점멸 순서는 상위비트 -> 하위비트 반복합니다.
*/
// Upper 4 Bit: 0000 / Network
#define CANNOT_WORK_WIFI_MODULE   0x02
#define SERVER_ACCESS_DENY        0x03
#define CANNOT_CONNECT_ROUTER     0x20
#define FAIL_TO_RESOLVE_IP        0x21
#define CANNOT_WORK_IMU_MODULE    0x42
#define BLOCK_ACCELERATION        0x51
#define BLOCK_GYROSCOPE           0x52
#pragma endregion

//======================================================================================
//======================================================================================

#pragma region WIFI //WIFI Module에 대한 기능 ===========================================
//Global Variables
int wifiStatus = WL_IDLE_STATUS;  // WiFi Status
WiFiUDP udp;
IPAddress serverIP;

void printWiFiStatus() 
{
  //printing Device name
  Serial.print("Network Device Name");
  Serial.println(ssid);
  //Printing IP Address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  //Printing Signal power(RSSL)
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void connectWiFi()
{
  if (WiFi.status() == WL_NO_MODULE) 
    noticeHW(ERR, CANNOT_WORK_WIFI_MODULE, "CANNOT_WORK_WIFI_MODULE");

  while (true) 
  {
    wifiStatus = WiFi.begin(ssid, password);

    delay(10000);
    if(wifiStatus == WL_CONNECTED)
      break;
    noticeHW(WARN, CANNOT_CONNECT_ROUTER, "Trying Connect WiFi...");
  }

  Serial.println("Success to Connect WiFi Network");
  printWiFiStatus();
}

bool checkServerConnection()
{
  // Send pint to Sensor Server for checkout Server Connection
  if (WiFi.ping(SENSOR_SERVER) >= 0) 
    return true;
  else 
    return false;
}

bool getIPAddress(IPAddress* serverIPAddr)
{
  if(WiFi.hostByName(SENSOR_SERVER, *serverIPAddr))
  {
    Serial.print("Resolved Server IP: ");
    Serial.println(*serverIPAddr);
    return true;
  }
  return false;
}

void telecom()
{
  udp.beginPacket(serverIP, PORT);
  udp.write("message\0");
  
  int result = udp.endPacket(); // 전송 성공 여부 확인
  if (result == 1) {  
    Serial.println("Message sent to server.");
  } else {
    Serial.print("Failed to send message. Result: ");
    Serial.println(result); // 전송 실패 시 결과 코드 출력
  }
}
#pragma endregion

#pragma region ACCEL //Accelation Module에 대한 기능 ===================================
//Global Variables
Axis6_t sensorData;

void connectIMU()
{
  if(!IMU.begin())
    noticeHW(ERR, CANNOT_WORK_IMU_MODULE, "CANNOT_WORK_IMU_MODULE");

  Serial.println("LSM6DS3 is successfully initialized!");
}

void renewIMUSensorValue(Axis6_t* axis)
{
  if(!IMU.accelerationAvailable())
    noticeHW(WARN, BLOCK_ACCELERATION, "BLOCK_ACCELERATION");
  if(!IMU.gyroscopeAvailable())
    noticeHW(WARN, BLOCK_GYROSCOPE, "BLOCK_GYROSCOPE");

  IMU.readAcceleration(axis->accelX, axis->accelY, axis->accelZ);
  IMU.readGyroscope(axis->pitch, axis->yaw, axis->roll);

#if UNIT_TEST == UNITTEST_IMU || UNIT_TEST == UNITTEST_ALL
  Serial.print(" Acc X: "); Serial.print(axis->accelX);
  Serial.print(" Acc Y: "); Serial.print(axis->accelY);
  Serial.print(" Acc Z: "); Serial.print(axis->accelZ);
  Serial.print(" Pitch: "); Serial.print(axis->pitch);
  Serial.print(" Yaw: "); Serial.print(axis->yaw);
  Serial.print(" Roll: "); Serial.println(axis->roll);
#endif
}

#pragma endregion

//======================================================================================
//======== Main ========================================================================

void setup() 
{
  //Start Serial Communication
  Serial.begin(9600);
  udp.begin(PORT);
  //Enable LED
  pinMode(LED_BUILTIN, OUTPUT);
  //Connect WiFi
  connectWiFi();
  //Check Server Connection
  if(!checkServerConnection())
    noticeHW(ERR, SERVER_ACCESS_DENY, "SERVER_ACCESS_DENY");
  if(!getIPAddress(&serverIP))
    noticeHW(ERR, FAIL_TO_RESOLVE_IP, "FAIL_TO_RESOLVE_IP");
  //Check IMU
  connectIMU();
  //initialize successful
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() 
{
  //WiFi Status 갱신

  //센서값 갱신
  renewIMUSensorValue(&sensorData);

  //데이터통신 
  //telecom();

  delay(FPS);
}






