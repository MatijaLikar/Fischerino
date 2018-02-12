/*
 * Avtor: Anonimno
 * V tem programu lahko na zaslonu na dotik spremljamo
 * temperaturo senzorja. Ob dotiku na tipko na zaslonu
 * se trenutna temperatura pošlje na strežnik.
 * 
 * Zaslon: TJCTM24028
 * Gonilnik zaslona: ILI9341
 * Senzor temperature je priključen na A0
 */
 
//Knjižnjica za povezavo v strežnik
#include <CayenneESP8266Shield.h> 

//Knjižnjice za zaslon na dotik
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <URTouch.h>

//Knjižnjice za merjenje časa
#include <Time.h> 
#include <TimeLib.h>

// Šifra, s katero se je mogoče prijaviti v spletišče
char token[] = "*********"; 

// Podatki o brezžičnem omrežju
char ssid[] = "********";
char password[] = "********";

//Ustvarjanje 'Serial' objekta za komunikacijo  Wi-Fi modulom
#define EspSerial Serial

//Priključki za upravljanje zaslona
#define TFT_CS 10
#define TFT_DC 4
#define TFT_RST 9

//Ustvarjanje Wi-Fi modula,senzorja za dotik in zaslona
ESP8266 wifi(EspSerial);
URTouch myTouch(5, 8, 6, 7, 2); 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  //Vzpostavitev povezave s strežnikom
  EspSerial.begin(115200);
  delay(10);
  Cayenne.begin(token, wifi, ssid, password);

  //Zagon zaslona na dotik
  tft.begin();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  //Nastavi barvo zaslona na črno in nastavi parametre zaslona in orientacije
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(2);
  tft.setTextSize(3);

  //Odčita vrednost termometra in posodobi zaslon
  getTemperature();
}

void loop()
{
  //Funkcija, ki ohranja povezavo z strežnikom
  Cayenne.run();
  
  //Pošlje temperaturo strežniku, če je tipka pritisnjena
  if (myTouch.dataAvailable() == true) {
    long x,y;
    myTouch.read();
    y = 320 - myTouch.getX();
    x = myTouch.getY();
    if (y<250&&y>200&&x<190&&x>50){
      sendToServer();
    }
  }
  //Vsako sekundo osveži prikaz temperature
  int u = millis()%1000;
  if (u < 10){
    getTemperature();
  } 
}
float getTemperature(){

  //Odčita vrednost iz A0 in jo pretvori v stopinje Celzija
  int t;
  t = analogRead(A0);
  t = 1023 - t;
  float voltage = t * 5.0;
  voltage /= 1024.0; 
  float temperatureC = (voltage - 0.5) * 100;

  //Izbriše zaslon
  tft.fillScreen(ILI9341_BLACK);

  //Prikaže tipko in besedilo
  tft.fillRect(50, 200, 140, 50, ILI9341_WHITE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0,100);
  tft.print(temperatureC);
  tft.print("*C");
  tft.setCursor(0, 60);
  tft.print("Temperature");
  tft.setCursor(85, 210);
  tft.setTextColor(ILI9341_RED);
  tft.print("SEND");

  //Vrne temperaturo v stopinjah Celzija
  return temperatureC;
}
void sendToServer(){
  //Odčita temperaturo in jo pošlje strežniku
  float temp = getTemperature();
  Cayenne.virtualWrite(V1, temp);

  //Sporoči uporabniku, da je bila vrednost poslana
  tft.fillScreen(ILI9341_GREEN);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(4);
  tft.setCursor(20,100);
  tft.print("Sent to");
  tft.setCursor(20,150);
  tft.print("server");
  tft.setTextSize(3);
  tft.setCursor(20,200);
  tft.print("Value: ");
  tft.print(temp);
  delay(2000);
}

