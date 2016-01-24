/*
 * DHT22 -> LCD1602
 * using afafriut lib
 */

#include <LiquidCrystal.h>
#include "U8glib.h"
#include "DHT.h"
 
#define DHTPIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);
U8GLIB_SH1106_128X64 u8g(10, 9, 12, 11, 13);

String dispbuf;

void setup() {
  Serial.begin(38400);
  dht.begin();
  // set up the LCD's number of columns and rows:
  //lcd.begin(16, 2);
  u8g.setFont(u8g_font_8x13);    
  u8g.setFontPosTop();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(1000);
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("DHT,");
  Serial.print(h);
  Serial.print(",");
  Serial.print(t);
  Serial.println();
 
  // set the cursor to column 0, line 0
  lcd.setCursor(0, 0);
  lcd.print("T = ");
  lcd.print(t);
  lcd.print("C");

  // set the cursor to column 0, line 1
  lcd.setCursor(0, 1);
  lcd.print("H = ");
  lcd.print(h);
  lcd.print("%");

  u8g.firstPage();  
  do 
  {
    dispbuf = "T = ";
    dispbuf.concat(t);
    dispbuf += "C";
    u8g.drawStr(0, 0, dispbuf.c_str());

    dispbuf = "H = ";
    dispbuf.concat(h);
    dispbuf += "%";
    u8g.drawStr(0, 14, dispbuf.c_str());
  } while(u8g.nextPage());
}

