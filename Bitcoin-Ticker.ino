#include "NotoSansBold36.h"
#include "NotoSansBold15.h"
#include <SPI.h>
#include <TFT_eSPI.h>  
#include "images.h"
#include "WiFi.h"
#include <ArduinoJson.h>
#include<HTTPClient.h>
#include "time.h"

// The font names are arrays references, thus must NOT be in quotes ""

#define AA_FONT_LARGE NotoSansBold36
#define AA_FONT_SMALL NotoSansBold15

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200;
const int   daylightOffset_sec = 0;



TFT_eSPI tft = TFT_eSPI();

const char* ssid     = "SSID";
const char* password = "PASSWORD";

HTTPClient http;
HTTPClient www;
WiFiClient client;



String data;
String data2;

void printLocalTime()
{
  tft.loadFont(AA_FONT_SMALL); 
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  tft.fillRect (14, 204, 240, 32, TFT_BLACK); // Overprint with a filled rectangle
  tft.setCursor(14, 204);
  tft.print(&timeinfo, "%A, %B %d %Y");
  tft.setCursor(100, 219);
  tft.print(&timeinfo, "%H:%M");
  tft.unloadFont(); // Remove the font to recover memory used
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  tft.begin();

  tft.setRotation(0);
  
  tft.fillScreen(TFT_BLACK);
  tft.loadFont(AA_FONT_LARGE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, 30);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  tft.print("Connecting");
  tft.setCursor(100, 80);
  tft.print("to");
  tft.setCursor(10, 130);
  tft.print(ssid);
  delay(1500);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }

  Serial.println("");
  tft.fillScreen(TFT_BLACK);
  Serial.print(WiFi.status());

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi connected");
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(80, 50);
    tft.print("WiFi");
    tft.setCursor(20, 100);
    tft.print("connected!");
  }
  delay(1500);
  tft.fillScreen(TFT_BLACK);

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  tft.unloadFont(); // Remove the font to recover memory used

}


void loop() {

  delay(1000);
  printLocalTime();
  getandprintData();
  delay(5000);

}

void getandprintData()
{

  // Send request
  http.begin("http://api.coindesk.com/v1/bpi/currentprice.json");
  http.GET();
  www.begin("https://min-api.cryptocompare.com/data/price?fsym=ETH&tsyms=USD,EUR,CNY,JPY,GBP");
  www.GET();

  // Print the response
  //Serial.print(http.getStream());

  // Parse response
  DynamicJsonDocument doc(8192);
  
  deserializeJson(doc, http.getStream());

  // Read values
  String sensor = doc["bpi"]["USD"]["rate_float"];
  data = sensor + " " + "$";
  Serial.println(sensor);
  // Disconnect
  http.end();



  //print data
  tft.loadFont(AA_FONT_LARGE); 
  //tft.fillScreen(TFT_BLACK);
  //              x  y  xbm   xbm width  xbm height  fg color   bg color
  tft.drawXBitmap(4, 69, bitcoin, 45, 45, TFT_ORANGE, TFT_BLACK);
  tft.setCursor(50, 80);
  tft.print("BTC price:");
  tft.fillRect (30, 130, 240, 40, TFT_BLACK); // Overprint with a filled rectangle
  tft.setCursor(30, 130);
  tft.print(data);
  tft.unloadFont(); // Remove the font to recover memory used


}
