/*
Thực hiện kết nối wifi MQTT để điều khiển
23/10 đã chạy được tiến lùi qua mqtt
Đã điều khiển được tốc độ

note: bỏ quẹt nhanh ở trường hợp mà 2 limnit đang có tín hiệu => ok
chống nhiễu limitsw => ok
26/11 Đã thực hiện chuyển đổi kết nối giữa LAN và WIfi ok
Ưu tiên kết nối Lan, nếu không có Lan sẽ dùng wifi
Kết nối MQTT ok

== Đã thêm điều khiển offline qua LAN, WIfi bằng API
/up
/down
/stop

====ngày 1/12/2022
- Điều chỉnh độ dài đi ra lớn lên từ 5mm lên 8mm
- Điều chỉnh ADC max từ 1650 lên 1750 => va chạm bên trong mạnh hơn
- Điều chỉnh delat ADC = 6% tăng độ nhạy va chạm ngoài
======V3.3=======
- Thêm điều kiện bắt ngưỡng delta ADC khi va chạm trong
Điều chỉnh 2 tham số sau cho hợp lý
float Adc_delta1 = 1.06;  //delta ADC va chạm ngoài
float Adc_delta2 = 1.06;  //delta ADC va chạm trong
*/

#include <SPI.h>
#include <Ethernet.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <Update.h>
#include <PubSubClient.h>
#include <IPAddress.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
bool STATIC = false;  // set to False to disable DHCP (adjust myip/gwip values below)
// ethernet interface ip address
static byte myip[] = { 192, 168, 2, 160 };
// gateway ip address
static byte gwip[] = { 192, 168, 2, 1 };
byte mymac[6];
// byte Ethernet::buffer[500];  // tcp/ip send and receive buffer


BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 123;

TimerHandle_t xOneShotTimer;
BaseType_t xTimer1Started;


// change UUID authentication encoding
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define DIR 27
#define STEP 25
#define EN_Pin 26
#define alam 2
#define SEN_IN 16
#define SW1 33
#define SW2 32
#define CURRENT_SENSOR 34
#define Button1 17
#define Button2 18
#define SS_Pin 15  //pin SS Ethernet

#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS(5)
#define ADC_MAX_SAMPLE 5
#define CHECK_IN_CRASH 0
#define CHECK_OUT_CRASH 1
#define MAX_ADC_OUT_CRASH 1300
#define MAX_ADC_IN_CRASH 900 // 1140
#define DEFAULT_PEAK_VAL 2400
static int in_crash_counter = 0;
static int check_adc_period_cnt = 0;
const byte DNSPORT = 53;
// IPAddress apIP(8,8,4,4); // The default android DNS
DNSServer dnsServer;
WebServer server(80);
// WiFiServer wifiServer(8088);
EthernetServer EthServer(80);
Preferences preferences;
String Fw = "V3.2";
String softAP_ssid = "AP_TouchAV_";
String softAP_password = "12345678";
String myHostname = "touchav-";

const char *mqtt_server = "103.28.39.158";
int mqtt_port = 1884;
const char *mqtt_user = "admin";
const char *mqtt_pass = "123456";

char ssid[32] = "";
char password[32] = "";

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
/** Should I connect to WLAN asap? */
boolean connect;
EthernetClient ethClient;
WiFiClient espClient;
PubSubClient client;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0, time_remtqq;

/** Current WLAN status */
int status = WL_IDLE_STATUS;
String chipId, st, stt_wifi, stt_lan;

int current_val, current_old = 0;
char inchar, Blechar, Lanchar, WifiChar;
unsigned long time_run, time_cur, first_time_current_peak;
unsigned long time_step, time_noi;
int step_in, step_in2;
bool stt_step, dk_run = false, dk_cur = false;
int speed_max = 150,  //us
  speed_start = 400,  //us
  speed_min = 450,    //us
  Step_up_down = 4500,
    Step_speed_down = 49000,
    Step_speed_min = 53000;  //52500
int sped;
float Adc_delta1 = 1.05;  //delta ADC va chạm ngoài
float Adc_delta2 = 1.04;  //delta ADC va chạm trong
//int Adc_max = 1180;      //NGưỡng max ADC, nếu vượt qua ngưỡng này sẽ dừng động cơ

int Val_current, Val_current_rate_for_incrash;
int Val_current_make_crash = 0;
int peak_Val_in_crash = DEFAULT_PEAK_VAL;
int sumVal = 0;
float Accelera;
byte k = 0;
bool first_time_adc = true;
bool new_adc_value = false;
int check_current_for_direction = 1;
bool there_is_in_crash = false, there_is_out_crash =  false;
int counting_peak_time = 0;


int Val_array[ADC_MAX_SAMPLE];
int Old_val1 = 100, Old_val2 = 100;
int Old_val1_make_crash = 0, Old_val2_make_crash = 0;
bool dk_current = true, dk_button = true, dk_serial = true, dk_bluetooth = true, dk_sensor = true, dk_mqtt = true;
bool stt_direction, stt_read = false, stt_LM = false;
String mqtt_func;
bool Lan_Connect = true;
bool dk_lan;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Blechar = rxValue[0];
      Serial.print("Nhan duoc tu buletooth: ");
      Serial.println(Blechar);
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(EN_Pin, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(STEP, OUTPUT);

  pinMode(alam, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SEN_IN, INPUT);
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(CURRENT_SENSOR, INPUT);
  Motor_stop();
  buzz(1);
  preferences.begin("CapPortAdv", false);
  Serial.println("Bat dau khoi dong chuong trinh");
  WiFi.macAddress(mymac);
  chipId = String(mymac[3], HEX) + String(mymac[4], HEX) + String(mymac[5], HEX);
  Serial.printf("This device's MAC Wifi address is: %x:%x:%x:%x:%x:%x", mymac[0], mymac[1], mymac[2], mymac[3], mymac[4], mymac[5]);
  mymac[5] = mymac[5] + 1;  //set địa chỉ MAC LAN Khác Wifi
  Serial.printf("This device's MAC LAN address is: %x:%x:%x:%x:%x:%x", mymac[0], mymac[1], mymac[2], mymac[3], mymac[4], mymac[5]);
  Serial.println("Chip ID: " + chipId);
  myHostname = myHostname + chipId;
  stt_wifi = "No Wifi Setup";
  stt_lan = "Lan Ethernet No Connect";
  Serial.println("Khoi dong module LAN");
  //ket noi lan
  // register udpSerialPrint() to port 1888
  // ether.udpServerListenOnPort(&udpSerialPrint, 1888);

  // Create the BLE Device
  BluetoothInt();
  //check status SW Limit
  if (digitalRead(SW1) == 0) {
    digitalWrite(EN_Pin, 1);
    stt_direction = 1;
    stt_LM = false;
    step_in = 0;
    step_in2 = 0;
  }
  if (digitalRead(SW2) == 0) {
    digitalWrite(EN_Pin, 1);
    stt_direction = 0;
    stt_LM = false;
    step_in = 0;
    step_in2 = 0;
  }
  if (digitalRead(SW1) == 1 && digitalRead(SW2) == 1) {
    stt_direction = 0;
    stt_LM = true;
  }

  Serial.println("Start AP Wifi..");
  WifiAp_Int();

  if (stt_direction == 1) {
    buzz(3);
  } else {
    buzz(2);
  }

  Accelera = (speed_start - speed_max) * 10.0 / Step_up_down;

  scan_wifi();
  Lanchar = 's';
  WifiChar = 's';
  Serial.println("Start AV Control V3.2");

  xOneShotTimer = xTimerCreate("OneShot", mainONE_SHOT_TIMER_PERIOD, pdTRUE, 0, prvOneShotTimerCallback );
  if(xOneShotTimer != NULL)
  {
    Serial.println("Creat timer ok");
    xTimer1Started = xTimerStart( xOneShotTimer, 0 );
  }
  if(  xTimer1Started == pdPASS  )
  {
  /* Start the scheduler. */
    Serial.println("Start Timer ok");
  }
}

void loop() {
  Read_Bluetooth();
  readserial();
  check_Sensor();
  check_button();

  if (!Lan_Function()) {
    Wifi_function();
  }

  // if (in_crash_counter && (in_crash_counter < 20))
  // {
  //   Serial.println("In crash counter " + in_crash_counter);
  // }
}

void buzz(int so_lan) {
  for (int i = 0; i < so_lan; i++) {
    digitalWrite(alam, 1);
    delay(100);
    digitalWrite(alam, 0);
    delay(100);
  }
}

void longbuzz() {
  digitalWrite(alam, 1);
  delay(600);
  digitalWrite(alam, 0);
  //    delay(1000);
}

int last_check_current_for_direction, last_counting_peak_time, last_there_is_in_crash;

void prvOneShotTimerCallback( TimerHandle_t xTimer )
{

    Val_array[k] = analogRead(CURRENT_SENSOR);//doc du lieu camr bien
    sumVal = 0;
    if (first_time_adc)
    {
        for (int i = 0; i < ADC_MAX_SAMPLE; i++)
        {
          Val_array[i] =  Val_array[k];
        }
        
    }

    k++;

    if (k > (ADC_MAX_SAMPLE - 1))
    {
      k = 0;
    }

    for (int i = 0; i < ADC_MAX_SAMPLE; i++)
    {
      sumVal = sumVal + Val_array[i];
    }

    Val_current = sumVal / ADC_MAX_SAMPLE;
    Val_current_rate_for_incrash = sped/speed_max*Val_current;
    if (first_time_adc)
    {
      Old_val2 = Val_current;
      Old_val1 = Old_val2;
    }
    first_time_adc = false;
    if (check_adc_period_cnt++ > 4)
    {
      check_adc_period_cnt == 0;
      // if (last_check_current_for_direction != check_current_for_direction
      //   || last_counting_peak_time != counting_peak_time
      //   || last_there_is_in_crash != there_is_in_crash)
      // {
      //   last_check_current_for_direction = check_current_for_direction;
      //   last_counting_peak_time = counting_peak_time;
      //   last_there_is_in_crash = there_is_in_crash;

      //   ets_printf("[%d] check dir = %d, counter %d, in crash %d\r\n", 
      //           millis(), check_current_for_direction, counting_peak_time, there_is_in_crash ? 1 : 0);
      // }
      
      if (check_current_for_direction == CHECK_IN_CRASH)
      {
        if (stt_direction && Val_current_rate_for_incrash)
        {
          ets_printf("s1:[%d],v1:{%d},v2(%d)\r\n",counting_peak_time, peak_Val_in_crash, Val_current_rate_for_incrash);
          //ets_printf("p:%d\r\n",counting_peak_time);
        }
        
        if ((Val_current_rate_for_incrash > peak_Val_in_crash) && (sped/speed_max > 2))
        {
          if ((Val_current_rate_for_incrash - peak_Val_in_crash) > 100) //Neu dong hien tai lon hon nguong qua nhieu -> set lai nguong
          {
            peak_Val_in_crash = Val_current_rate_for_incrash;
            counting_peak_time = 0;   //reset counter
          }
          counting_peak_time++;       //san sang cho lan quet tiep
          //first_time_current_peak = millis();
        }
        else if (counting_peak_time && (Val_current_rate_for_incrash > (peak_Val_in_crash * 7 / 10)))
        {
          counting_peak_time++;
        }
        else if (Val_current_rate_for_incrash < (peak_Val_in_crash * 7 / 10))
        {
          peak_Val_in_crash = DEFAULT_PEAK_VAL;
          counting_peak_time = 0;
        }

                //int is_crashed_by_adc = ;
        
        //if (((Old_val1 > MAX_ADC_IN_CRASH) && (Old_val2 >= (Old_val1 * 104 / 100)) && (Val_current >= (Old_val1 * 104 / 100)))
        if (counting_peak_time > 100) 
        {
          //in_crash_counter = 2;
          // if ((counting_peak_time > 500) )//&& ((millis() - first_time_current_peak) > 8000))
          {
              //in_crash_counter = 0;
              peak_Val_in_crash = DEFAULT_PEAK_VAL;
              counting_peak_time = 0;
              Old_val1_make_crash = Old_val1;
              Old_val2_make_crash = Old_val2;
              Val_current_make_crash = Val_current;
              there_is_in_crash = true;
          }
        }

        

        // if (in_crash_counter)
        // {
        //   in_crash_counter--;
        //   if (in_crash_counter == 0)
        //   {
        //     Old_val1_make_crash = Old_val1;
        //     Old_val2_make_crash = Old_val2;
        //     Val_current_make_crash = Val_current;
        //     there_is_in_crash = true;
        //   }
        // }
      }
      else if (check_current_for_direction == CHECK_OUT_CRASH)
      {   
          if (((Old_val1 > 900) 
          && (Old_val2 >= (Old_val1 * 105 / 100)) && 
          (Val_current >= (Old_val1 * 105 / 100))) || (Val_current > MAX_ADC_OUT_CRASH)) 
          {
          // Motor_stop_in_isr();
          Old_val1_make_crash = Old_val1;
          Old_val2_make_crash = Old_val2;
          Val_current_make_crash = Val_current;
          there_is_out_crash = true;
          }
        
      }
      Old_val1 = Old_val2;
      Old_val2 = Val_current;
    }
}
