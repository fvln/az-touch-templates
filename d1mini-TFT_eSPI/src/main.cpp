/*
 An example analogue meter using a ST7735 TFT LCD screen

 This example uses the hardware SPI only
 Needs Font 2 (also Font 4 if using large scale label)

 Updated by Bodmer for variable meter size
*/


#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <TJpg_Decoder.h>

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define URL "http://ursurnes.sirv.com/webcam-paris.jpg?w=320&h=240&format=jpg&q=85&subsampling=4:2:2"

uint8_t buff[20000] = { 0 };

WiFiClient client;



bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  //This might work instead if you adapt the sketch to use the Adafruit_GFX library
  //tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}


void get_jpg_picture(){
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");    
    ESP.restart();
    delay(2000);
  }

  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  http.begin(URL);

  Serial.print("[HTTP] GET...\n");
  int httpCode = http.GET();

  Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  // HTTP header has been send and Server response header has been handled
  if (httpCode <= 0) {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  } else {
    if (httpCode != HTTP_CODE_OK) {
      Serial.printf("[HTTP] Not OK!\n");
    } else {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      uint32_t len = http.getSize();
      Serial.printf("[HTTP] size: %d\n", len);

      if (len <= 0) {
        Serial.printf("[HTTP] Unknow content size: %d\n", len);
      } else {
        // create buffer for read
        //uint8_t buff[len] = { 0 };

        // get tcp stream
        WiFiClient * stream = http.getStreamPtr();

        // read all data from server
        uint8_t* p = buff;
        int l = len;
        while (http.connected() && (l > 0 || len == -1)) {
          // get available data size
          size_t size = stream->available();

          if (size) {
            int s = ((size > sizeof(buff)) ? sizeof(buff) : size);
            int c = stream->readBytes(p, s);
            p += c;

            //Serial.printf("[HTTP] read: %d\n", c);

            if (l > 0) {
              l -= c;
            }
          }
        }

        Serial.println();
        Serial.print("[HTTP] connection closed.\n"); 

        // Get the width and height in pixels of the jpeg if you wish
        uint16_t w = 0, h = 0;
        TJpgDec.getJpgSize(&w, &h, (const uint8_t*)buff, len);
        Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);
        // check the picture size
        if (w > 320) {
          Serial.println("Picture too large, QVGA supported only!");
          http.end(); 
          return;
        }
    
        // Draw the image, top left at 0,0
        TJpgDec.drawJpg(0, 0, (const uint8_t*)buff, len);
      }
    }
  }

  http.end(); 
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  
  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_RED);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)
  
  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1); // requires QVGA picture, 320 x 240 resolution
  // TJpgDec.setJpgScale(2); // requires VGA picture, 640 x 480 resolution
  
  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  WiFi.begin("Tofu", "17483318!");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip); 

}


void loop() {
  
  get_jpg_picture();
  delay(60000);
}