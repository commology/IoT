/*
 * DHT22 -> LCD1602/LCD12864
 * using afafriut DHT22 lib
 * U8glib Arduino
 * DS18B02; with its specific small PCB
 */

#include <LiquidCrystal.h>
#include "U8glib.h"
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define FREQ 5

#define DHTPIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define ONE_WIRE_BUS 3


LiquidCrystal lcd(8, 7, 6, 5, 4, 3);
U8GLIB_SH1106_128X64 u8g(10, 9, 12, 11, 13);

String dispbuf;


DHT dht(DHTPIN, DHTTYPE);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature ds18b02(&oneWire);

unsigned long cyc = 0;
unsigned long seq = 0;

const unsigned long CYC_LIMIT = FREQ * (1000L*1000L);

void setup_timer() {
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 62500;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();
}

void setup() {
  setup_timer();
  
  Serial.begin(38400);
  dht.begin();
  ds18b02.begin();
  
  // set up the LCD's number of columns and rows:
  //lcd.begin(16, 2);
  u8g.setFont(u8g_font_8x13);    
  u8g.setFontPosTop();

  cyc = 0;
  seq = 0;
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  cyc++;

  if (cyc >= CYC_LIMIT)
    cyc = 1;
  
  if (cyc % FREQ != 1)
    return;

  seq++;
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
   
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  ds18b02.requestTemperatures();      // Send the command to get temperatures
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float T = ds18b02.getTempCByIndex(0);   // 1-wire device index
 
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("AMS,");
  Serial.print(seq);
  Serial.print(",");
  Serial.print(h, 1);
  Serial.print(",");
  Serial.print(t, 1);
  Serial.print(",");
  Serial.print(T);
  Serial.println();
  Serial.flush();

  // set the cursor to column 0, line 0
  lcd.setCursor(0, 0);
  lcd.print("H = ");
  lcd.print(h, 1);
  lcd.print("%");
 
  // set the cursor to column 0, line 1
  lcd.setCursor(0, 1);
  lcd.print("T = ");
  lcd.print(t, 1);
  lcd.print("C");

  u8g.firstPage();
  do 
  {
    dispbuf = "# = ";
    dispbuf.concat(seq);
    u8g.drawStr(0, 0, dispbuf.c_str());

    dispbuf = "";
    dispbuf.concat(h);
    dispbuf += "% ";
    dispbuf.concat(t);
    dispbuf += "C"; 
    u8g.drawStr(0, 14, dispbuf.c_str());

    dispbuf = "T! = ";
    dispbuf.concat(T);
    u8g.drawStr(0, 28, dispbuf.c_str());
  } while(u8g.nextPage());
}

void loop() {
  // Wait a few seconds between measurements.
}

