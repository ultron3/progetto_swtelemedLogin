#include <SPI.h>
#include <Seeed_FS.h>
#include "SD/Seeed_SD.h"
#include <rpcWiFi.h>
#include <TFT_eSPI.h>
#include <Arduino_JSON.h>
#include "WiFiClient.h"
#include "HTTPClient.h"
#include "Ultrasonic.h"
#include "Adafruit_NeoPixel.h"
#define PIN PIN_WIRE_SCL      //digital port a cui sono connessi i led
#define NUMPIXELS 10          //numero di led
#define BUZZER_PIN WIO_BUZZER /* sig pin of the buzzer */

int boxPort[5] = { 0, 8, -1, -1, -1 };  //porte pin wio
int status[5] = { 0, 0, 0, 0, 0 };      //status boxes
int alarms[5] = { 0, 0, 0, 0, 0 };      //alarm 0 null 1 take 2 alarm 3 taken boxes
//int ids[5] = { 0, 0, 0, 0, 0 };         //gli id dei box su database
int id0 = 0;
int id1 = 0;
int id2 = 0;
int ch1 = 0;
int ch2 = 0;
int ch3 = 0;


String posologia[5] = { "p1", "p2", "p3", "p4", "p5" };
int numBoxes = 2;  //numboxes pharmbox
int w = 320;
int h = 240;
Ultrasonic boxes[] = { (boxPort[0]), (boxPort[1]) };
TFT_eSPI tft;            //schermo lcd
String wifi_name = "";   // wifiname
String wifi_pwd = "";    //wifipwd
String macaddress = "";  //macaddress terminal
String site = "http://itspharm.pythonanywhere.com";
String urlmac = site + "/api/arduino/pharms/macaddress/boxes";
//String url="https://itspharm.pythonanywhere.com/api/pharms/2/boxes";
////////////////

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int led_div, totbox;


///////////

void setup() {
  //buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  //setup lcd
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(2);
  tft.fillScreen(0x0);
  //setup ultrasonic

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  //while (!Serial);  //non printa finchè non apri
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  String mac = WiFi.macAddress();
  urlmac.replace("macaddress", mac);
  tft.drawString((String)WiFi.macAddress(), 10, 120);
  //while (!setup_wifi()) ;           // attende completamento wifi letto da sd
  setupLed(2);  //numero totale di box da prendere da request
  //sendStatus();
  delay(10000);
}

void loop() {
  delay(5000);        //tempo di ritardo
  checkUltrasonic();  // aggiorno status boxes
  checkAlarm();
}

void checkAlarm() {
  // posologia
  // allarmi
  // chiamata web per ricere json array con i dati
  getjsonArrayBoxes();  // dopo chiamata ar posologia e alarm aggiornati
  //scorriamo i boxes e settimao allarmi corretti
  for (int i = 0; i < numBoxes; i++) {
    int al = (int)alarms[i];
    int st = (int)status[i];

    if (al == 0 && st == 1) {  //caso spento e status 1
      setLed(i + 1, 0, 0, 0);
    }
    if (al == 2 && st == 0) {  //presa
      alarms[i] = 3;
      setLed(i + 1, 0, 0, 0);
    }

    if (al == 1 && st == 1) {  //caso ok da prendere
      setLed(i + 1, 0, 200, 0);
    }
    if (al == 1 && st == 0) {  //caso appena preso
      setLed(i + 1, 0, 0, 0);
      alarms[i] = 3;
      // messaggio log preso in orario
    }
    if (al == 2 && st == 0) {  //caso appena preso in ritardo
      setLed(i + 1, 0, 0, 0);
      alarms[i] = 3;

      // messaggio log preso in ritardo
    }
    if (al == 2 && st == 1) {  //caso da prendere assolutamente
      setLed(i + 1, 200, 0, 0);
      buzz();
      // messaggio log preso in ritardo
    }
  }
  String t = "al 1 = " + (String)alarms[0] + " - al 2 =  " + (String)alarms[1];
  tft.drawString(t, 10, 80);
  delay(2000);
  sendStatus();
}

void sendStatus() {
  WiFiClient client;  // or WiFiClientSecure for HTTPS
  HTTPClient http;

  for (int i = 0; i < numBoxes; i++) {
    int idbox = 0;  // ids[i];
    if (i == 0) idbox = id0;
    if (i == 1) idbox = id1;
    if (i == 2) idbox = id2;

    String url = site + "/api/boxes/" + idbox + "/logs";
    Serial.println(url);
    int al = alarms[i];
    int d = 55;
    String jsondata = "{\"status\":" + (String)al + ",\"delta\":" + (String)d + "}";
    //Serial.println(jsondata);
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsondata);  //Send the actual POST request
    if (httpResponseCode > 0) {
      String response = http.getString();  //Get the response to the request
      //Serial.println(httpResponseCode);    //Print return code
      Serial.println(response);  //Print request answer
      http.end();
    } else {
      Serial.print("Error on sending POST: ");
      //Serial.println(httpResponseCode);
      http.end();
    }
    delay(2000);
  }
}


void getjsonArrayBoxes() {

  //WiFiClientSecure client;  // or WiFiClientSecure for HTTPS
  WiFiClient client;  // or WiFiClientSecure for HTTPS
  HTTPClient http;
  http.begin(client, urlmac);
  //Serial.println(urlmac);
  int res = http.GET();
  //Serial.println(res);
  String input = http.getString();
  http.end();
  Serial.println(input);
  JSONVar arBoxes = JSON.parse(input);
  for (int i = 0; i < numBoxes; i++) {

    JSONVar myBox = JSON.parse(JSON.stringify(arBoxes[i]));
    //Serial.println(myBox);
    if (myBox.hasOwnProperty("timebox")) {
      const char* timebox = myBox["timebox"];
      const char* ora = myBox["ora"];
      const char* posology = myBox["message"];
      const char* colore = myBox["color"];

      posologia[i] = posology;
      printPosology(i, posology, timebox, colore);


      int id = (int)myBox["id"];
      if (i == 0) { id0 = id; }
      if (i == 1) { id1 = id; }
      if (i == 2) { id2 = id; }

      int status = myBox["status"];
      int oltre = myBox["oltre"];
      int prima = myBox["prima"];

      // caso alarm 0 siamo prima
      if (prima == 1) { alarms[i] = 0; }
      // caso alarm 1 siamo nel delta
      if (prima == 0 && oltre == 0) { alarms[i] = 1; }
      //caso alarm 2 siamo oltre tempo massimo
      if (oltre == 1) { alarms[i] = 2; }
    }
  }
}

bool setup_wifi() {
  bool finito = false;
  File myFile;
  File myNewFile;
  if (digitalRead(WIO_5S_PRESS) == LOW) {
    Serial.print("Initializing SD card...");
    tft.fillScreen(0x0);
    tft.drawString((String) "Initializing SD card...", 10, 120);
    if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI)) {
      Serial.println("initialization failed!");
      tft.fillScreen(0x0);
      tft.drawString((String) "initialization failed!", 10, 120);
      wifi_name = "Its-guest";  // wifiname
      wifi_pwd = "eet6Euru";    //wifipwd
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      int ls = wifi_name.length() + 1;
      int lp = wifi_pwd.length() + 1;
      char ssid[ls];
      char password[lp];
      wifi_name.toCharArray(ssid, ls);
      wifi_pwd.toCharArray(password, lp);
      Serial.println(ssid);
      Serial.println(password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
        tft.fillScreen(0x0);
        tft.drawString((String) "Connecting to WiFi..", 10, 120);
        WiFi.begin(ssid, password);
      }
      Serial.println("Connected to the WiFi network");
      tft.fillScreen(0x0);
      tft.drawString((String) "Connected to the WiFi network", 10, 120);
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      finito = true;
    } else {
      Serial.println("initialization done.");
      tft.fillScreen(0x0);
      tft.drawString((String) "initialization done.", 10, 120);
      myFile = SD.open("wifi.txt", FILE_READ);
      if (myFile) {
        if (myFile.available()) {
          while (!Serial);
          wifi_name = myFile.readStringUntil('\n');
          wifi_name.trim();
          Serial.println("Nome wifi: " + wifi_name);
          wifi_pwd = myFile.readStringUntil('\n');
          wifi_pwd.trim();
          Serial.println("Password wifi: " + wifi_pwd);
          String mca = myFile.readStringUntil('\n');
          mca.trim();
          Serial.println("macaddress wifi: " + mca);
          WiFi.mode(WIFI_STA);
          WiFi.disconnect();
          int ls = wifi_name.length() + 1;
          int lp = wifi_pwd.length() + 1;
          char ssid[ls];
          char password[lp];
          wifi_name.toCharArray(ssid, ls);
          wifi_pwd.toCharArray(password, lp);
          Serial.println(ssid);
          Serial.println(password);
          while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.println("Connecting to WiFi..");
            tft.fillScreen(0x0);
            tft.drawString((String) "Connecting to WiFi..", 10, 120);
            WiFi.begin(ssid, password);
          }
          Serial.println("Connected to the WiFi network");
          tft.fillScreen(0x0);
          tft.drawString((String) "Connected to the WiFi network", 10, 120);
          Serial.print("IP Address: ");
          Serial.println(WiFi.localIP());
          finito = true;
        }
        myFile.close();
        myNewFile = SD.open("wifi.txt", FILE_WRITE);
        if (myNewFile) {
          myNewFile.println(wifi_name);
          myNewFile.println(wifi_pwd);
          myNewFile.println(WiFi.macAddress());
          myNewFile.println("--fine config---");
        } else {
          Serial.println("error opening wifi.txt");
        }
      } else {
        Serial.println("error opening wifi.txt");
        tft.fillScreen(0x0);
        tft.drawString((String) "error opening wifi.txt", 10, 120);
      }
      myNewFile.close();
    }
  }
  return finito;
}
void checkUltrasonic() {
  long dist;
  for (int i = 0; i < numBoxes; i++) {
    dist = boxes[i].MeasureInCentimeters();  // two measurements should keep an interval
    Serial.print(dist);                      //0~400cm
    Serial.print(" cm box ");
    Serial.println(i);  //0~400cm
                        //!!!!! verificare distanza base tetto
    if (dist < 10)
      status[i] = 1;
    else
      status[i] = 0;
  }
  //tft.fillScreen(0x0);
  //String t = "st 1 = " + (String)status[0] + " - st 2 =  " + (String)status[1];
  //tft.drawString(t, 10, 20);
}

int setupLed(int tbox) {
  totbox = tbox;
  //inizializzazione led
  led_div = NUMPIXELS / totbox;
  pixels.setBrightness(255);
  pixels.begin();
}

void setLed(int nr_box, int r, int g, int b) {
  if (nr_box <= totbox) {
    int nbox = nr_box - 1;
    for (int i = (nbox * led_div); i < ((nbox * led_div) + led_div); i++) {
      pixels.setPixelColor(i, pixels.Color(r, g, b));
    }
    pixels.show();
  }
  //funzione per aggiornare stato su json e/o database
}


void upLed(int nr_box) {
  if (nr_box <= totbox) {
    int nbox = nr_box - 1;
    for (int i = (nbox * led_div); i < ((nbox * led_div) + led_div); i++) {
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    }
    pixels.show();
  }
  //funzione per aggiornare stato su json e/o database
}

void clearLed(int nr_box) {
  if (nr_box <= totbox) {
    int nbox = nr_box - 1;

    for (int i = (nbox * led_div); i < ((nbox * led_div) + led_div); i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }
  //funzione per aggiornare stato su json e/o database
}
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(tone);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(tone);
  }
}
void buzz() {
  //playTone(1600, 1000);
  //playTone(800, 1000);
}
void printPosology(int i, String pos, String timebox, String colore) {
  int color = 0;
  if (colore.equalsIgnoreCase("rosso")) {
    color = TFT_RED;
    Serial.print("rosso");
  }
  if (colore.equalsIgnoreCase("blu")) {
    color = TFT_BLUE;
    Serial.print("blu");
  }
  int top = i * (h / numBoxes);
  int height = h / numBoxes;
  tft.drawRect(0, top, w, height, color);
  tft.fillRect(0, top, w, height, color);
  tft.setTextColor(TFT_WHITE);          //sets the text colour to black
  tft.setTextSize(3);                   //sets the size of text
  tft.drawString(timebox, 3, top + 3);  //prints strings from (0, 0)
  tft.drawString(pos, 3, top + 43);     //prints strings from (0, 0)
}