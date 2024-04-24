#include <Keypad.h>
#include<EEPROM.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         A0
#define SS_PIN          10
MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);
char password[4];  // mảng lưu mk nhập từ bàn phím 
char initial_password[4],new_password[4];  // mnagr lưu mk gốc trong eeprom và mk mới ở chế độ đổi mật khẩu 

#define coi A2
#define khoa A1
#define buttonmokhoa A3
int i=0;  // biến đếm số ký tự mã nhập từ bàn phím 
int solannhapsai =0;
unsigned long time =0, time1 =0  ,time2 , time3 ,cotime3=1;

int y=0; //biến vị trí cho viện lấy dữ liệu từ erom ra để so sánh 

int thedatontai=0 ;
int thechuatontai=0;
int themaster =0;

int gt_otrong , diachi_otrong;


int UID[4];
int ID_MASTER[4] ,ID_EEPROM[4] ,ID_SS[4];

byte diachi = 7;  //ô dầu tiên lưu thẻ master 7,8,9,10

byte n=11; // địa chỉ ô nhớ bắt đầu quét 

byte ktmaster =1 ;  // cờ kiểm tra xem có thẻ master chưa 
byte chedothemthe=0;  // cờ chế độ thêm thẻ
byte chedodocthetu=1; // cờ chế độ đọc thẻ
byte chedoxoathe =0; // cờ chế độ xóa thẻ 
byte chedophim=1;  // cờ chế độ đọc key
byte chedodoimaster =0 ;// cờ chế độ đổi thẻ master 
byte doimaster =0; // cờ cho chức năng đổi master
byte docma =0;

int a;  // gia trị ô nhớ số 6 ô nhớ lưu giá trị ô nhớ đã sử dụng tới ô nhớ bao nhiêu 
int m =11; // dịa chỉ ô đầu tiên lưu thẻ từ phụ

char  key_pressed=0;
const byte rows = 4; 
const byte columns = 4; 
char hexaKeys[rows][columns] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte row_pins[rows]       = {9,8,7,6};
byte column_pins[columns] = {5,4,3,2};
Keypad keypad_key=Keypad( makeKeymap(hexaKeys),row_pins,column_pins,rows,columns);

int gtmas = 0;  int gttong = 0; // biến xác định xem ô nhớ 0,1,2,3 đã có dữ liệu lưu chưa 
int gtmas1 = 0; int gttong1 = 0;
void setup(){

  pinMode(coi , OUTPUT);
  pinMode(khoa , OUTPUT);
  pinMode(buttonmokhoa,INPUT_PULLUP);
  //pinMode(A6 , INPUT_PULLUP);

  digitalWrite(coi ,LOW);
  digitalWrite(khoa , LOW);
  
  // khởi tạo lcd 
  EEPROM.begin();

  // // nhấn giữ nut khi cắm nguồn lại để resset lại hết eeprom 
  // if(digitalRead(A6)==LOW)
  // {
  //   for(int i=0; i<EEPROM.length(); i++)
  //   {
  //     EEPROM.write(i,0);
  //   }
  // }
  
  

  SPI.begin();    
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("   SMART CLOCK  ");
  delay(2000);

  lcd.setCursor(0, 0);
  lcd.print("    * IOT *    ");
  lcd.setCursor(0, 1);
  lcd.print("DO QUYNH ANH");
  delay(2000);
  
  // ô lưu mật khẩu 0,1,2,3,4  , ô lưu số lần nhập sai 5 , ô lưu số ô nhớ đã sử dụng 6 ,  ô lưu thẻ master 7,8,9,10


  // kiểm tra xem đã có mk gốc chưa 
  for(int j = 0; j < 4; j++) // đọc từ 1 - 4 xem trong ô nhớ có gì ko, Nếu không là chưa có mât khẩu 
  {
    gtmas = EEPROM.read(j);
    gttong = gttong + gtmas;
     
  } 
  if(gttong==0) // chưa có mật khẩu 
  {  
    int j;
    for(j=0;j<4;j++)
    EEPROM.write(j,j+49);  // ghi mật khẩu ban đầu là 1234
    for(j=0;j<4;j++)
    initial_password[j]=EEPROM.read(j);
  }
  lcd.clear();

}

////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
  
  kiemtramaster();
  docthetu();
  khoahethong();
  themthetuphu();
  xoathebatky();
  doithemaster();

  //kiêm tra xem có nút nào nhấn không 
  if(chedophim==1)
  {
    key_pressed = keypad_key.getKey();
   
    if (key_pressed) // nếu nhấn số khác chữ c
    {
      password[i++]=key_pressed;
      bip();
      lcd.setCursor(i+5, 1);
      lcd.print("x");
    }
    if (key_pressed == 'C')  // nếu nhấn nut C thì xóa hết mk vừa nhập cho nhập lại từ đầu 
    {
      i=0;  
      bip();
      lcd.setCursor(0,1);
      lcd.print("     CLEAR      ");
      delay(800);
      lcd.setCursor(0,1);
      lcd.print("                ");
    }
    if (i==4)
    {
      ssmatkhau();
    }
  }
  //ham nut nhấn mở khóa từ bên trong //
  nutmokhoa();

  }
  

///////////////////////////////////////////////khoa hệ thống khi nhập sai quá 4 lần /////////////////////
void khoahethong()
{
   //Đọc ô nhớ số 5 xem số lần sai là bao nhiêu 
  solannhapsai= EEPROM.read(5);
  // khóa hẹ thống khi nhập sai quá 4 lần 
  if(solannhapsai>=4)
  {
    if(cotime3==1)
    {
      time3= millis();
      cotime3=0;
    }
    //chạy ham tiếng cảnh báo 
    if((unsigned long)(millis()-time3)<30000)
    {
      bipcanhbao();
    }
    else{
      noTone(coi);
    }
  
    // khoa hết hệ thống 
    chedodocthetu=0;
    chedothemthe=0;
    chedoxoathe=0;
    chedophim=0;

    lcd.setCursor(0,0);
    lcd.print("HE THONG BI KHOA");
    lcd.setCursor(0,1);
    lcd.print("NHAP SAI >3 LAN ");

    digitalWrite(khoa,LOW);
    

    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    bip();
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++) 
    {        
      UID[i] = mfrc522.uid.uidByte[i];
    }
    mfrc522.PICC_HaltA();  
    mfrc522.PCD_StopCrypto1();

    // kiểm tra xem thẻ đưa vào có phải thẻ master không
    if (UID[0] == EEPROM.read(7) && UID[1] == EEPROM.read(8) && UID[2] == EEPROM.read(9) && UID[3] == EEPROM.read(10))  
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MO KHOA HE THONG");
      lcd.setCursor(0,1);
      lcd.print(">>>>>>>><<<<<<<<");
      digitalWrite(khoa,HIGH);
      bip();
      delay(3000);
      digitalWrite(khoa,LOW);
      EEPROM.write(5,0);  // set số lần nhập sai về 0 
      chedodocthetu=1;
      chedophim=1;
      cotime3=1;

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("NHAP MAT KHAU:");
      lcd.setCursor(0,1);
      
    }
  }
}

/// ham đổi mật khẩu //////////////////////////////////////////////////////////////////////////////////////
void change(){

  int j=0;
  lcd.clear();
  lcd.print("  DOI MAT KHAU  ");
  delay(1000);
  
  lcd.clear();
  lcd.print("MAT KHAU CU:    ");
  lcd.setCursor(0,1);
  // get mk nhập từ bàn phím 
  while(j<4)
  {
    char key=keypad_key.getKey();
    if(key)
    {
      new_password[j++]=key;
      bip();
      lcd.setCursor(j+5, 1);
      lcd.print("x");
    }
    if (key == 'C')  // nếu nhấn nut C thì xóa hết mk vừa nhập cho nhập lại từ đầu 
    {
      j=0;  
      bip();
      lcd.setCursor(0,1);
      lcd.print("     CLEAR      ");
      delay(800);
      lcd.setCursor(0,1);
      lcd.print("                ");
    }
    key=0;
  }
  // lấy mk trong eeprom 
  for(int j=0;j<4;j++)
  initial_password[j]=EEPROM.read(j);
  // so sánh mk nhập từ bàn phím với mk trong eeprom 
  if((strncmp(new_password, initial_password, 4)))  // nếu không giống nhau 
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  MAT KHAU SAI  ");
    lcd.setCursor(0, 1);
    bip_sai();
    lcd.print("****************");
    delay(1500);
  }
  else                                              // nếu giống nhau 
  {
    j=0;
    lcd.clear();
    lcd.print("MAT KHAU MOI:   ");
    lcd.setCursor(0,1);
    bip_dung();
    // lấy mật khẩu mới lưu vào eeprom 
    while(j<4)
    {
      char key=keypad_key.getKey();
      if(key)
      {
        initial_password[j]=key;
        EEPROM.write(j,key);
        j++;
        bip();
        lcd.setCursor(j+5, 1);
        lcd.print("x");
      }
      if (key == 'C')  // nếu nhấn nut C thì xóa hết mk vừa nhập cho nhập lại từ đầu 
      {
      j=0;  
      lcd.setCursor(0,1);
      lcd.print("     CLEAR      ");
      bip();
      delay(800);
      lcd.setCursor(0,1);
      lcd.print("                ");
      }
      
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  DOI MAT KHAU  ");  // thông báo thay đổi mk thành công 
    lcd.setCursor(0, 1);
    lcd.print("***THANH CONG***");
    bip_dung();
    delay(1500);
  }
  lcd.setCursor(0,0);
  lcd.print("NHAP MAT KHAU:");
  lcd.setCursor(0,1);
  lcd.clear();
  key_pressed=0;
}
// ham so sánh mật khẩu nhập vào với eeprom ///////////////////////////////////////////////
void ssmatkhau()
{
  for(int j=0;j<4;j++)
  initial_password[j]=EEPROM.read(j);

  if(!(strncmp(password, initial_password,4)))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" MAT KHAU DUNG  ");
    lcd.setCursor(0, 1);
    lcd.print("****************");
    digitalWrite(khoa,HIGH);

    EEPROM.write(5,0); // mỗi lần nhập đúng reset so lần nhập sai về 0
    bip_dung();

    delay(4000);
    digitalWrite(khoa,LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("NHAP MAT KHAU:");
    lcd.setCursor(0,1);
    i=0;// set cờ i về 0 để nhập lại mk
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  SAI MAT KHAU  ");
    lcd.setCursor(0, 1);
    lcd.print("****************");

    solannhapsai++;
    EEPROM.write(5,solannhapsai);

    digitalWrite(khoa,LOW);
    bip_sai();
    delay(1500);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("NHAP MAT KHAU:");
    lcd.setCursor(0,1);
    i=0; // set cờ i về 0 để nhập lại mk
  }
}
/////////////////////////setupp thẻ master ////////////////////////////////////////////
void kiemtramaster()
{
  if (ktmaster == 1) // kiểm tra xem có thẻ master chưa 
  {
    /// kiểm tra xem đã có thẻ master chưa 
    for(int j = 7; j<11; j++) // đọc từ 7 - 10 xem trong ô nhớ có gì ko, Nếu không là chưa có mât khẩu 
    {
      gtmas1 = EEPROM.read(j);
      gttong1 = gttong1 + gtmas1;
    } 

    if (gttong1==0)  // nếu chưa có thẻ master
    {
      chedodocthetu=0; // khoa chế độ 
      chedophim=0; // khoa che độ 

      lcd.setCursor(0,0);
      lcd.print("**SETUP MASTER**");
      bip();
      lcd.setCursor(0,1);
      lcd.print("MOI NHAP THE....");

      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      { return; } 
      bip();
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      { return; }
      for (byte i = 0; i < 4; i++) 
      { 
        UID[i] = mfrc522.uid.uidByte[i];

        EEPROM.write(diachi, UID[i]); //bắt đầu lưu từ ô 7-10
        diachi = diachi + 1;
      }
      mfrc522.PICC_HaltA();  
      mfrc522.PCD_StopCrypto1();
      delay(50);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("**THIET LAP OK**");
      bip();

      delay(1000);
      lcd.clear();  
    }
    else
    {
      chedodocthetu=1;// mở lại chế độ 
      chedophim=1 ;// mở lại che độ 
      ktmaster = 0; // set cờ kiểm tra the master về 0
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("NHAP MAT KHAU:");
      lcd.setCursor(0,1);
    }
  }

}
////////////////////////// hàm đọc thẻ thẻ từ ////////////////////////////////////
void docthetu ()
{
  if(chedodocthetu==1)  // nếu cờ chế độ dọc thẻ từ bằng 1 
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    bip();
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++) 
    {        
      UID[i] = mfrc522.uid.uidByte[i];
    
    }
    mfrc522.PICC_HaltA();  
    mfrc522.PCD_StopCrypto1();

    // kiểm tra xem thẻ đưa vào có phải thẻ master không
    if (UID[0] == EEPROM.read(7) && UID[1] == EEPROM.read(8) && UID[2] == EEPROM.read(9) && UID[3] == EEPROM.read(10))  
    {
      lcd.clear();
      lcd.print("***THE MASTER***");
      digitalWrite(khoa,HIGH);
      delay(2000);
      digitalWrite(khoa,LOW);
      //EEPROM.write(5 , 0); // set lại số lần nhập sai bằng 0 khi nhập thẻ master
      lcd.clear();

      chedothemthe=0;
      chedoxoathe=0;
      chedodoimaster=0;

      time2=millis();// lấy giá trị trước khi vào vòng while giá này dùng để thoát vongd while khi không nhấn j

      // VONG LẶP CHO MENU 
      while(true)
      { 
        if ((unsigned long)(millis() - time1) > 1500)
        {
          bip();
          lcd.setCursor(0,0);
          lcd.print("1. XOA THE      ");
          lcd.setCursor(0,1);
          lcd.print("2. THEM THE     ");
          time1 = millis();
         
        }  
   
        else if ((unsigned long)(time1 - time) > 1500)
        {
          
          lcd.setCursor(0,0);
          lcd.print("3. DOI MAT KHAU ");
          lcd.setCursor(0,1);
          lcd.print("4. DOI THE MASTE");
          time = millis();
          
        }
        
        if((unsigned long)(millis()-time2)>15000)
        {
          lcd.clear();
          break;
        }
       

        char key=keypad_key.getKey();
        if(key == 'A') { bip();lcd.clear();    break ;}  // nếu nhấn A sẽ thoát chế độ 
        if(key == '3') { bip();change();             }  // nếu nhấn số 3 sẽ chạy hàm đổi mk 
        if(key == '2') { bip();chedothemthe=1; break;}  // nếu nhấn số 2 thì set cờ chế độ thêm thẻ lên 1
        if(key == '1') { bip();chedoxoathe=1 ; break;}  // chọn số 1 vào chế độ xóa thẻ 
        if(key == '4') { bip();chedodoimaster=1 ;docma=1; break;} // chọn số 4 vào chế độ đổi thẻ master 

      }
    }
    else // nếu thẻ đưa vào k phải thẻ master
    {
       a = EEPROM.read(6);
       if(a==0)
       {
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("CHUA CO THE PHU ");
         lcd.setCursor(0, 1);
         lcd.print("****************");
         bip_sai();
         delay(1000);
       }
       if (a!=0)
       {
         ssthetu_eeprom();
         if(thedatontai==1)
         {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("****************");
            lcd.setCursor(0,1);
            lcd.print("    DUNG THE    ");
            digitalWrite(khoa,HIGH);

            EEPROM.write(5,0);  // KHI NHẬP ĐÚNG thẻ set số lần nhập sai về 0
            bip_dung();

            thedatontai=0;
            thechuatontai=0;
            themaster=0;
            y=0;
            m=11;
            delay(3000);
            digitalWrite(khoa,LOW);

         }
         else if(thechuatontai==1)
         {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("****************");
            lcd.setCursor(0,1);
            lcd.print("     SAI THE    ");
            digitalWrite(khoa,LOW);

            solannhapsai++;
            EEPROM.write(5,solannhapsai); //khi nhập sai thẻ tăng số lần nhập sai lên 1
            bip_sai();

            thedatontai=0;
            thechuatontai=0;
            themaster=0;
            y=0;
            m=11;
            delay(1500);
         }
       }

    }

    //key_pressed=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("NHAP MAT KHAU:  ");
    lcd.setCursor(0,1);
  }
}

///////////////////////////////// ham thêm thẻ từ ///////////////////////////////////////////////
void themthetuphu()
{ 
  if(chedothemthe==1)  // nếu cờ chế độ thêm thẻ từ lên 1
  { 
    chedodocthetu=0; // cờ chế độ đọc thẻ từ về 0
    chedoxoathe = 0; // cờ chế độ xóa thẻ 
    chedophim=0;  // tắt cờ chế độ phím
    chedodoimaster=0; //tắt cờ chế độ đổi thẻ master

    lcd.setCursor(0,0);
    lcd.print("CHE DO THEM THE ");
    lcd.setCursor(0,1);
    lcd.print(">>MOI QUET THE<<");

    char key=keypad_key.getKey();
    if(key == 'A')  //nếu nhấn nut c thì thoát chế độ 
    { 
      bip();
      lcd.setCursor(0,0);
      lcd.print("NHAP MAT KHAU:  ");
      lcd.setCursor(0,1);
      lcd.print("                ");

      chedothemthe=0;  // set cờ chế độ thêm thẻ về 0 để thoát chế độ 
      chedodocthetu=1; // sau khi thoát chế độ thêm thẻ thì set cờ chế độ đọc thể từ lên 1
      chedophim=1;  // bật lại chế độ phím sau khi thoát chế  độ thêm thẻ 
    }

    a = EEPROM.read(6); //đọc ô nhớ 6 xem đã sử dụng bao nhiêu ô nhớ
    if (a == 0) //Nếu chưa có thẻ PHỤ nào
    {
      int diachi_phu =11 ; // 11,12,13,14
      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      { return; } 
      bip();
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      { return; }  
      for (byte i = 0; i < 4; i++)  //Quét thẻ mới
      {          
        UID[i] = mfrc522.uid.uidByte[i];
        
      }
      mfrc522.PICC_HaltA(); 
      mfrc522.PCD_StopCrypto1();
      bip(); 
      // nếu là thẻ master thì thông báo nhập lại 
      if (UID[0] == EEPROM.read(7) && UID[1] == EEPROM.read(8) && UID[2] == EEPROM.read(9) && UID[3] == EEPROM.read(10))    
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(">>>THE MASTER<<<");
        lcd.setCursor(0,1);
        lcd.print("MOI QUET THE LAI");
        bip_sai();
        delay(1500);
      }
      // nếu thẻ mới không phải thẻ master thì tiến hành thêm thẻ 
      else
      {
        for (byte i = 0; i < 4; i++)
        {
          EEPROM.write(diachi_phu, UID[i]);
          diachi_phu = diachi_phu + 1;
          a = diachi_phu;
        }
        EEPROM.write(6, a); //Sau khi lưu 1 thẻ mới vào thì cập nhật số ô nhớ đã sử dụng vào ô 0
        lcd.setCursor(0,1);
        lcd.print("***SAVE  CARD***"); 
        bip();
        delay(1000);
        lcd.clear(); 
      }
    }
    else if( a != 0)  // nếu đã có 1 thẻ phụ hoặc nhiều thẻ phụ 
    {
      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      { return; } 
      bip();
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      { return; }  
      for (byte i = 0; i < 4; i++)  //Quét thẻ mới
      {          
        UID[i] = mfrc522.uid.uidByte[i];

      }
      mfrc522.PICC_HaltA(); 
      mfrc522.PCD_StopCrypto1();

      ssthetu_eeprom();

      if (themaster==1)    
      {
        lcd.clear();
        themaster=0;
        thedatontai=0;
        thechuatontai=0;
        y=0;
        m=11;
        lcd.setCursor(0,0);
        lcd.print("***THE MASTER***");
        lcd.setCursor(0,1);
        lcd.print("MOI QUET THE LAI");
        bip_sai();
        delay(1500);
      }
      
      else if(thedatontai==1)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("*******!!*******");
        lcd.setCursor(0,1);
        lcd.print(" THE DA TON TAI ");
        bip_sai();

        thedatontai=0;
        thechuatontai=0;
        themaster=0;
        y=0;
        m=11;
        delay(1000);
      }

      else if(thechuatontai==1)
      {
        y=0;
        m=11;
        thechuatontai=0;
        thedatontai=0;
        themaster=0;

        timotrong();
        
        // NẾU trong đoạn từ ô 11 đến ô đã sử dụng k có ô nào trống thì lưu thẻ 
       if (diachi_otrong == a)  
        {
          for(int i=0; i<4; i++)   // lưu thẻ vào vị trí tiếp theo 
          {
            EEPROM.write(diachi_otrong, UID[i]);
            diachi_otrong = diachi_otrong + 1;
            a = diachi_otrong;
          }
          EEPROM.write(6, a); //Sau khi lưu 1 thẻ mới vào thì cập nhật số ô nhớ đã sử dụng vào ô 0
        
        }
        else if(diachi_otrong == n) ////Nếu trong đoạn từ 11 đến số ô đã sử dụng CÓ ô = 0, thì gán ô đó vào n
        {
          for(int i=0; i<4; i++) //Lưu thẻ mới vào bắt đầu từ ô 0 đó
          {
            EEPROM.write(diachi_otrong, UID[i]);
            diachi_otrong = diachi_otrong + 1;
          }
        }
        lcd.setCursor(0,1);
        lcd.print("***SAVE  CARD***"); 
        bip();
        delay(1000);

      }
    }
  }
}
//////////////////////////////// ham so sanh the tu nhập vào với các thẻ có trong eeprom////////
void ssthetu_eeprom()
{ 
  while(m<a)   // điều kiện thoát 
  {
    int moi = m + 4; //moi = 15
    for(m; m<moi; m++)
    {
      int redeeprom = EEPROM.read(m);
      ID_EEPROM[y] = redeeprom;
      y++;
    }

    if(UID[0] == ID_EEPROM[0] && UID[1] == ID_EEPROM[1] && UID[2] == ID_EEPROM[2] && UID[3] == ID_EEPROM[3]) // Nếu thẻ đã tồn tại 
    {
      thedatontai = 1;
      thechuatontai=0;
      themaster=0;
      break;
    }

    else if(UID[0] != ID_EEPROM[0] or UID[1] != ID_EEPROM[1] or UID[2] != ID_EEPROM[2] or UID[3] != ID_EEPROM[3]) //Nếu thẻ KO có trong EEPROM
    {
      thechuatontai = 1;
      thedatontai=0;
      themaster=0;
    }

    if (UID[0] == EEPROM.read(7) && UID[1] == EEPROM.read(8) && UID[2] == EEPROM.read(9) && UID[3] == EEPROM.read(10))  
    {
      themaster=1;
      thechuatontai=0;
      thedatontai=0;
    }  
    y=0; // set biên về 0
    m=moi; // cập nhập lại giá trị bắt đầu tiếp theo
  }
 
}
//////////////////////////////////////////ham tim vị trí trống đầu tiên////////////////////////////
void timotrong()
{
  a = EEPROM.read(6);  // đọc địa chỉ ô nhớ số 6 để lấy giá trị ô nhớ trống đầu tiên 
  while( n < a) //Tìm từ ô 11 đến ô đã sử dụng là a xem ô nào có giá trị =0
  {
    gt_otrong = EEPROM.read(n);
    if (gt_otrong == 0)
    {
      diachi_otrong = n;  // cho địa chỉ ô trống bằng n 
      break;
    }     
    n++;
  }
  if (diachi_otrong ==0) // nếu từ ô 11 đến ô đã sử dụng không có ô nào trống thì ô trống chính là a
  {
    diachi_otrong =a ;
    n=11;// set lại biến cho lần quét tiếp theo 
  }
}
//////////////////////////////ham xoa the bat ky /////////////////////////////////////////////////
void xoathebatky()
{
  if(chedoxoathe==1)
  {
    chedodocthetu=0; // tăt cờ chế độ đọc thẻ từ 
    chedothemthe =0; // tắt cờ chế độ thêm thẻ từ 
    chedophim=0;    // tắt cờ chế độ phím 
    chedodoimaster=0;// cờ chế độ doi thẻ master

    lcd.setCursor(0,0);
    lcd.print("*CHE DO XOA THE*");
    // lcd.setCursor(0,1);
    // lcd.print("XIN MOI QUET THE");

    char key=keypad_key.getKey();
    if(key == 'A')  //nếu nhấn nut A thì thoát chế độ 
    { 
      bip();
      lcd.setCursor(0,0);
      lcd.print("NHAP MAT KHAU:  ");
      lcd.setCursor(0,1);
      lcd.print("                ");

      chedoxoathe=0;  // set cờ chế độ xóa thẻ về 0
      chedodocthetu=1; // sau khi thoát chế độ thêm thẻ thì set cờ chế độ đọc thể từ lên 1
      chedophim=1; // bật lại chế độ phím sau khi thoát chế độ xóa thẻ 
    }

    a = EEPROM.read(6); //đọc số ô nhớ đã sử dụng 

    if (a == 0) //Nếu chưa có thẻ PHỤ nào
    { 
      lcd.setCursor(0,1);
      lcd.print("CHUA CO THE PHU ");
      bip_sai();
    }

    else if( a!= 0)
    {
      lcd.setCursor(0,1);
      lcd.print("                ");
      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      { return; } 
      bip();
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      { return; }  
      for (byte i = 0; i < 4; i++)  //Quét thẻ mới
      {          
        UID[i] = mfrc522.uid.uidByte[i];
        
      }
      mfrc522.PICC_HaltA(); 
      mfrc522.PCD_StopCrypto1(); 

      ssthetu_eeprom();

      if(thedatontai==1)
      {
        m=m-4;
        for(int i=0; i<4; i++)
        {
        EEPROM.write(m, 0);
        m = m + 1; 
        }
        lcd.setCursor(0,1);
        lcd.print(">DA XOA THE.... "); 
        bip();
        delay(1000); 
        //timotrong();

        thedatontai=0;
        thechuatontai=0;
        themaster=0;
        y=0;
        m=11;
      }
      else if ( thechuatontai==1)
      {
        lcd.setCursor(0,1);
        lcd.print("!THE CHUA CO...."); 
        bip_sai();
        delay(1000); 

        thedatontai=0;
        thechuatontai=0;
        themaster=0;
        y=0;
        m=11;
      }
      // nếu là thẻ master thì thông báo nhập lại 
      else if (themaster)    
      {
        thedatontai=0;
        thechuatontai=0;
        themaster=0;
        y=0;
        m=11;

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("***THE MASTER***");
        lcd.setCursor(0,1);
        lcd.print(" KHONG THE XOA! ");
        bip_sai();
        delay(1500);
      }
    }
  }
}
////////////////////////////////ham doi thẻ master///////////////////////////
void doithemaster()
{
  if(chedodoimaster==1)
  {
    chedodocthetu=0; // tăt cờ chế độ đọc thẻ từ 
    chedothemthe =0; // tắt cờ chế độ thêm thẻ từ 
    chedophim=0;    // tắt cờ chế độ phím
    chedoxoathe=0;  // tắt cờ chế độ xóa thẻ  
    int j=0;
    
    lcd.setCursor(0,0);
    lcd.print("*DOI THE MASTER*");
    ///
    if(docma==1)
    {
      lcd.setCursor(0,1);
      lcd.print("MAT KHAU:       ");
      lcd.setCursor(10,1);

      // get mk nhập từ bàn phím 
      while(j<4)
      {
        char key=keypad_key.getKey();
        if(key)
        {
          bip();
          password[j++]=key;
          lcd.setCursor(j+9, 1);
          lcd.print("x");
        }
        if (key == 'C')  // nếu nhấn nut C thì xóa hết mk vừa nhập cho nhập lại từ đầu 
        {
          bip();
          j=0;  
          lcd.setCursor(10,1);
          lcd.print("       ");
        }
        
        key=0;
      }
      // lấy mk trong eeprom 
      for(int j=0;j<4;j++)
      initial_password[j]=EEPROM.read(j);
      // so sánh mk nhập từ bàn phím với mk trong eeprom 
      if((strncmp(password, initial_password, 4)))  // nếu không giống nhau 
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  MAT KHAU SAI  ");
        lcd.setCursor(0, 1);
        lcd.print("****************");
        j=0; // set cờ về 0
        chedodoimaster=0; // set cờ chế độ đổi maaster về 0 để thoát chế độ 
        chedodocthetu=1; // sau khi thoát chế độ thêm thẻ thì set cờ chế độ đọc thể từ lên 1
        chedophim=1; // bật lại chế độ phím sau khi thoát chế độ xóa thẻ 
        doimaster=0; //set cờ chế độ về 0
        docma=0; // set cờ về 0
        bip_sai();
        delay(1000);

        lcd.setCursor(0,0);
        lcd.print("NHAP MAT KHAU:  ");
        lcd.setCursor(0,1);
        lcd.print("                ");

        
        
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  MAT KHAU DUNG ");
        lcd.setCursor(0, 1);
        lcd.print("****************");
        docma=0; // set cờ về 0 để qua phần quẹt thẻ đổi thẻ mas
        j=0;  // set cờ về 0
        doimaster=1;// set cờ lên 1 để qua phần quẹt thẻ 
        bip_dung();
        delay(1000);
      }

    }
    
    //
    if(doimaster==1 )
    {
      lcd.setCursor(0,1);
      lcd.print("  NHAP THE MOI  ");

      char key=keypad_key.getKey();
      if(key == 'A')  //nếu nhấn nut A thì thoát chế độ 
      { 
        bip();
        lcd.setCursor(0,0);
        lcd.print("NHAP MAT KHAU:  ");
        lcd.setCursor(0,1);
        lcd.print("                ");

        chedodoimaster=0; // set cờ chế độ đổi maaster về 0 để thoát chế độ 
        chedodocthetu=1; // sau khi thoát chế độ thêm thẻ thì set cờ chế độ đọc thể từ lên 1
        chedophim=1; // bật lại chế độ phím sau khi thoát chế độ xóa thẻ 
        doimaster=0; //set cờ chế độ về 0
        docma=0; // set cờ về 0
        
      }

      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      { return; } 
      bip();
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      { return; }
      for (byte i = 0; i < 4; i++) 
      { 
        UID[i] = mfrc522.uid.uidByte[i];

        // EEPROM.write(diachi, UID[i]); //bắt đầu lưu từ ô 7-10
        // diachi = diachi + 1;
      }
      mfrc522.PICC_HaltA();  
      mfrc522.PCD_StopCrypto1();
      // tiến hành hiểm tra xem thẻ master có là thẻ thàng viên k 
      ssthetu_eeprom();

      if(thedatontai==1)  /// nếu thẻ đưa vào là thẻ phụ thì xóa thẻ đó trong eeprom và lưu vào vị trid thẻ master 
      { 
        m=m-4;
        for(int i=0; i<4; i++)
        {
        EEPROM.write(m, 0);
        m = m + 1; 
        }
        
        // tiến hành lưu
        diachi=7;
        for (byte i = 0; i < 4; i++) 
        { 
         EEPROM.write(diachi, UID[i]); //bắt đầu lưu từ ô 7-10
         diachi = diachi + 1;
        }
        // set lại cờ giá trị 
        themaster=0;
        thedatontai=0;
        thechuatontai=0;
        y=0;
        m=11;
      }

      else  // nếu thẻ mas mới chưa tông tại trong thẻ phụ thì tiến hành lưu
      {
        diachi=7;
        // tiến hành lưu
        for (byte i = 0; i < 4; i++) 
        { 
         EEPROM.write(diachi, UID[i]); //bắt đầu lưu từ ô 7-10
         diachi = diachi + 1;
        }
        // set lại cờ giá trị 
        themaster=0;
        thedatontai=0;
        thechuatontai=0;
        y=0;
        m=11;
      }

      delay(50);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("** DOI THE OK **");
      bip();
      delay(1000);
    }  
  }
}
/////////////////////ham nut nhấn mở khóa từ bên trong ////////////
void nutmokhoa()
{
  if(digitalRead(A3)==HIGH)
  {
    digitalWrite(khoa,HIGH);
    bip_dung();
    delay(3000);
    digitalWrite(khoa,LOW);
  }
}
/////////////// ham tạo tiếng bip mỗi khi nhấn nút //////////////////////////
void bip ()
{
  tone(coi,1000,50);
}

void bip_sai()
{
  tone(coi,200,500);
}
void bip_dung()
{
  tone(coi,1000,500);
}

/// hàm tạo tiếng kêu cảnh báo //////////
void bipcanhbao()
{
  float sinVal;
  int toneVal;

   for(int x=0; x<180; x++)
   {
      // convert degrees to radians then obtain value
      sinVal = (sin(x*(3.1412/180)));
      // generate a frequency from the sin value
      toneVal = 2000+(int(sinVal*1000));
      tone(coi, toneVal,1);
     }  
}
