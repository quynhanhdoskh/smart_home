#include <WiFiManager.h>     
#include <EEPROM.h>

#include <FirebaseESP32.h>

#define FIREBASE_HOST "https://smarthome-60fcb-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "KZO1O5CMhf95ptz8WWlbIY8TjyExCmktZCWVVNXO"  // QA

FirebaseData FirebaseRead;
FirebaseData FirebaseWrite;

#define LM35 35         // chân cảm biến nhiệt độ 
#define RESETWIFI  12    // chân nút để thay đổi mạng wifi 
#define CTHT  14        // chân công tắc hành trình nhận biết trạng thái cửa 
#define GASPIN  33      // chân cảm biến khí ga 

#define FAN_GAS  4     // chân tín hiệu đk quạt hút khí ga 
#define CBCD  32        // chân cảm biến chuyển động quạt trần 
#define CBCD2  27       // chân cảm biến chuyển động đèn 
#define QUAT_TRAN  15   // chân tín hiệu đk quạt trần 
#define CBAS  26        // chân cảm biến ánh sáng 
#define LED  2         // chân tín hiệu đk đèn 
#define LEDCUA  13      // chân tín hiệu đk led bao cua  


//
int gas ,set_gas_on = 30 , set_gas_off = 10 , set_fan_on = 32 ,set_fan_off = 27  ;
int temp_lm35;
int tt_chedo;

// biến về thời gian sáng của đèn 
unsigned long time_led  ;
unsigned long tt_time_led =1 , thoigiansang = 10000;

// biến về thời gian sau bao lâu không đóng cửa sẽ kêu cảnh báo  
unsigned long time_door  ;
unsigned long tt_time_door =1 , thoigianmocua = 10000;


// linh để in trạng thái thiết bị lên firebase
String URL_STATE_DEN = "/CSDL/CONTROL/STATE_DEN";
String URL_STATE_DOOR = "/CSDL/CONTROL/STATE_DOOR";
String URL_STATE_FAN_GAS = "/CSDL/CONTROL/STATE_FAN_GAS";
String URL_STATE_FAN_TRAN = "/CSDL/CONTROL/STATE_FAN_TRAN";

// linh in lên thống số nhiệt độ và lồng độ khí ga 
String VALUE_GAS = "/CSDL/VALUES/GAS";
String VALUE_TEMP = "/CSDL/VALUES/TEMP";

// linh lấy tín hiệu điều khiển quạt trần ở chế độ tay 
String URL_FAN_APP = "/CSDL/CONTROL/FAN_APP";

// linh lấy giá trị chế độ auto quạt trần 
String AUTO_FAN = "/CSDL/CONTROL/AUTO_FAN";

// linh lấy giá trị on , off quạt trần ở chế độ auto
String VALUE_SETTING_AUTO_FAN_ON = "/CSDL/SETTING_AUTO/FAN_ON";
String VALUE_SETTING_AUTO_FAN_OFF = "/CSDL/SETTING_AUTO/FAN_OFF";

// linh lay giá trị on , off quạt hút ga 
String VALUE_SETTING_GAS_ON = "/CSDL/SETTING_AUTO/GAS_ON";
String VALUE_SETTING_GAS_OFF = "/CSDL/SETTING_AUTO/GAS_OFF";

WiFiManager wifiManager;   

void setup() {

  Serial.begin(9600);

  pinMode(CTHT, INPUT_PULLUP);
  pinMode(RESETWIFI, INPUT_PULLUP);
  pinMode(CBCD, INPUT);
  pinMode(CBCD2, INPUT);
  pinMode(CBAS, INPUT);
  

  pinMode(FAN_GAS, OUTPUT);
  pinMode(QUAT_TRAN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(LEDCUA, OUTPUT);

  digitalWrite(FAN_GAS , 0);
  digitalWrite(QUAT_TRAN , 0);
  digitalWrite(LED , 0);
  digitalWrite(LEDCUA , 0);
  
  //kết nối wifi
  EEPROM.begin(512); 
  wifiManager.autoConnect("DO_AN_QUYNH_ANH");
  
  // kết nối wifi ok thì tiến hành kết nối firebase
  if (WiFi.status() == WL_CONNECTED) 
  {
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
  }
}

void loop() {

  tes();

  buttonwifi();
  quattran();
  Light();
  gas_fan();
  read_tt();
  canhbaocua();

}

//--------------ham đọc trạng thái của các thiết bị và in lên firebase---------------------
void read_tt() {

// nếu có kết nối wifi
if (WiFi.status() == WL_CONNECTED) 
{
  // tt thái cửa ra vào 
  Firebase.setInt(FirebaseWrite, URL_STATE_DOOR, digitalRead(CTHT));
  // tt đèn 
  Firebase.setInt(FirebaseWrite, URL_STATE_DEN, digitalRead(LED));
  // tt quạt trần 
  Firebase.setInt(FirebaseWrite, URL_STATE_FAN_GAS, digitalRead(FAN_GAS));
  // tt quạt hút khí ga 
  Firebase.setInt(FirebaseWrite, URL_STATE_FAN_TRAN, digitalRead(QUAT_TRAN));


  // nồng độ khí ga 
  Firebase.setInt(FirebaseWrite, VALUE_GAS, gas);
  // nhiệt độ 
  Firebase.setFloat(FirebaseWrite, VALUE_TEMP, temp_lm35);

}

}
//------------------hàm cảnh báo khi không đóng cửa -------------
void canhbaocua(){

  // đọc tt công tắt hành trình 
  int tt_congtac = digitalRead(CTHT);

    // nếu mà cửa đang mở                                       
    if (tt_congtac == 1) 
    {  
      // cưa mở sau 5s mà không đóng thì kêu 
      if(tt_time_door ==1)
      {
        time_door = millis();
        tt_time_door =0;
      }
      if ( (millis() - time_door )>= thoigiansang) 
      {
        if(digitalRead(CTHT) == 1)
        {
          digitalWrite(LEDCUA , !digitalRead(LEDCUA));
          //delay(80);
          
        }
      }
    }
    // neu mà cửa đóng reset tt để chuẩn bị cho lần kêu tiếp theo
    else
    {
      tt_time_door = 1;
      digitalWrite(LEDCUA , 0);
    }
     

}

//---------------- ham bật tắt quạt trần -----------------------------
// cam bien hong ngoai khi == 0 la co  nguoi, = 1 là không có
void quattran() {

  int sum =0 ;
  int tb_adc =0;

  // đọc nhiệt độ của cảm biến 15 lần
  for(int i=0 ; i<15 ; i++)
  {
    int adc = analogRead(LM35);
    sum += adc ; 
  }

  // lấy giá trị tb 
  tb_adc = sum /15;

  // chuyển thành giá trị nhiệt độ 
  temp_lm35 = map(tb_adc , 0,4095 , 0 , 330);
  
  if(WiFi.status() == WL_CONNECTED) 
  {
  // đọc xem đang ở chế độ auto hay thủ công 
  Firebase.getInt(FirebaseRead, AUTO_FAN);
  tt_chedo = FirebaseRead.intData();

  Firebase.getInt(FirebaseRead, VALUE_SETTING_AUTO_FAN_ON);
  set_fan_on= FirebaseRead.intData();

  Firebase.getInt(FirebaseRead, VALUE_SETTING_AUTO_FAN_OFF);
  set_fan_off = FirebaseRead.intData();

  }
  // nếu đang ở chế độ auto
  if (tt_chedo == 1 ) 
  { 
    // nếu có người 
    if(digitalRead(CBCD)== 0)     
    {
      if(temp_lm35 >= set_fan_on)
      {
        digitalWrite(QUAT_TRAN , 1);
      }

      if(temp_lm35 <= set_fan_off)
      {
        digitalWrite(QUAT_TRAN , 0);
      }
    } 
    // nếu không có người
    else
    {
      digitalWrite(QUAT_TRAN , 0);
    }                        
  } 

  // nếu đang ở chế độ thủ công 
  if (tt_chedo == 0 )
  {
    // đọc tín hiệu đk từ app trên firebase
    Firebase.getInt(FirebaseRead, URL_FAN_APP);
    int tt = FirebaseRead.intData();

    // bật tắt thiết bị 
    if(tt == 1)
    {
      digitalWrite(QUAT_TRAN , 1);
    }
    if(tt==0)
    {
      digitalWrite(QUAT_TRAN , 0);
    }
    
  }

}

//-------------------hàm bật tắt đèn tự động ------------------------
// cam bien ánh sáng tối = 1, ngày  = 0
// cam bien hong ngoai 0 co người, 1 không có

void Light() {

  // đọc cảm biến ánh sáng 
  int camBienAnhSang = digitalRead(CBAS);

    // nếu trời tối và có người                                        
    if (camBienAnhSang == 1 && digitalRead(CBCD2) == 0) 
    {  
      // bật đèn 
      digitalWrite(LED, 1);
      tt_time_led=1;
    }
     // bật sau 10s thì tắt 
    if(tt_time_led ==1)
    {
      time_led = millis();
      tt_time_led =0;
    }
    if ( (millis() - time_led )>= thoigiansang) 
    {
      digitalWrite(LED, 0);
    }

}

//---------------hàm chạy quạt khi khí ga vượt ngưỡng cài đặt--------------------
void gas_fan() {

  // đọc cảm biến ga 
  int adc_gas = analogRead(GASPIN);

  // chuyển thành nồng độ 0-100%
  gas = map(adc_gas, 000, 4095, 0, 100);

  if (WiFi.status() == WL_CONNECTED) 
  {
  // đọc dữ liệu trên firebase 
    Firebase.getInt(FirebaseRead, VALUE_SETTING_GAS_ON);
    set_gas_on = FirebaseRead.intData();
    //
    Firebase.getInt(FirebaseRead, VALUE_SETTING_GAS_OFF);
    set_gas_off = FirebaseRead.intData();

  }
  // nếu nồng độ ga vượt mức set thì bật quạt thông gió 
  if (gas >= set_gas_on)
  {  
    digitalWrite(FAN_GAS, 1);
  }
  // nếu nồng độ ga giảm xuống mức set thì tắt 
  if (gas <= set_gas_off)
  {
    digitalWrite(FAN_GAS, 0);
  }

}
// ---------------------ham nut thiết lập bắt wifi mới ------------------
void buttonwifi(){

  if (digitalRead(RESETWIFI) == LOW) {
    delay(30);
    if (digitalRead(RESETWIFI) == LOW) {
      delay(1500);
      if (digitalRead(RESETWIFI) == LOW) {
        wifiManager.resetSettings();  
        wifiManager.autoConnect("DO_AN_QUYNH_ANH"); 
      }
    }
  }
}
//------------------------tes firebase ---------------------------
 void tes ()
{
  Firebase.setInt(FirebaseWrite, "tes",random(0,1000));
  delay(100);
  Firebase.getInt(FirebaseRead, "tes");
  int random = FirebaseRead.intData();
  Serial.print("READ FIREBASE RANDOM : ");
  Serial.println(random);
}
