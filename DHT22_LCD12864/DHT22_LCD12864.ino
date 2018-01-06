/*
 * DHT22 -> LCD1602/LCD12864
 * using afafriut DHT22 lib
 * U8glib Arduino
 * DS18B02; with its specific small PCB
 */

#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <U8glib.h>
#include <Adafruit_DHT.h>
#include <Adafruit_BMP085.h>

#include <IRremote.h>

#define FREQ 5

#define DHTPIN 8
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define ONE_WIRE_BUS 7

#define DUST_AOUT 5     // SHARP GP2Y1010
#define DUST_ILED 5     // 

//LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

/*
 * SW SPI Com:
 * SCK/CLK/D0   <---> 9
 * MOSI/DIN/D1  <---> 10
 * CS           <---> 11
 * DC/A0        <---> 12
 * RES/RST      <---> 13
 */
U8GLIB_SH1106_128X64 u8g(9, 10, 11, 12, 13);

String dispbuf;


DHT dht22(DHTPIN, DHTTYPE);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
// Pass our oneWire reference to Dallas Temperature. 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b02(&oneWire);

Adafruit_BMP085 bmp180;    // IIC interface


unsigned long cyc = 0;
unsigned long seq = 0;

const unsigned long CYC_LIMIT = FREQ * (1000L*1000L);

int recvPin = 7;
IRrecv irrecv(recvPin);
IRsend irsend;

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

  pinMode(DUST_ILED, OUTPUT);
  digitalWrite(DUST_ILED, LOW);

  dht22.begin();
  bmp180.begin();
  ds18b02.begin();
  
  // set up the LCD's number of columns and rows:
  //lcd.begin(16, 2);
  u8g.setFont(u8g_font_8x13);    
  u8g.setFontPosTop();

  cyc = 0;
  seq = 0;

  irrecv.enableIRIn();  // Start the receiver

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
  float H22 = dht22.readHumidity();
  // Read temperature as Celsius
  float T22 = dht22.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(H22) || isnan(T22)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
 
  float BMP180_T = bmp180.readTemperature();
  float BMP180_P = bmp180.readPressure();
    
  // Calculate altitude assuming 'standard' barometric pressure of 1013.25 millibar = 101325 Pascal
  float BMP180_A = bmp180.readAltitude();
  
  float BMP180_SLP = bmp180.readSealevelPressure();

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  float BMP180_Ax = bmp180.readAltitude(101500);
   
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  ds18b02.requestTemperatures();            // Send the command to get temperatures
  float T18 = ds18b02.getTempCByIndex(0);   // 1-wire device index

  digitalWrite(DUST_ILED, HIGH);
  delayMicroseconds(280);
  float aout = analogRead(DUST_AOUT);
  delayMicroseconds(40);
  digitalWrite(DUST_ILED, LOW);

  float aoutVo = aout * 5.0 / 1024.0;

  // http://www.howmuchsnow.com/arduino/airquality/
  float dustDensity = (0.172 * aoutVo - 0.0999) * 1000;

  Serial.print("AMS,");
  Serial.print(seq);
  
  Serial.print(",");
  Serial.print(H22, 1);
  Serial.print(",");
  Serial.print(T22, 1);
  
  //Serial.print(",");
  //Serial.print(BMP180_T);
  Serial.print(",");
  Serial.print(BMP180_P);
  Serial.print(",");
  Serial.print(BMP180_SLP);
  //Serial.print(",");
  //Serial.print(BMP180_A);
  //Serial.print(",");
  //Serial.print(BMP180_Ax);
  
  Serial.print(",");
  Serial.print(T18);
  
  //Serial.print(",");
  //Serial.print(aout);
  //Serial.print(",");
  //Serial.print(aoutVo);
  Serial.print(",");
  Serial.print(dustDensity);
  
  Serial.println();
  Serial.flush();

  // set the cursor to column 0, line 0
  //lcd.setCursor(0, 0);
  //lcd.print("H = ");
  //lcd.print(h, 1);
  //lcd.print("%");
 
  // set the cursor to column 0, line 1
  //lcd.setCursor(0, 1);
  //lcd.print("T = ");
  //lcd.print(t, 1);
  //lcd.print("C");

  if (seq % 5 == 3) 
    irsend.sendRC6(0x8010A620, 32);

  u8g.firstPage();
  do 
  {
    dispbuf = "# ";
    dispbuf.concat(seq);
    u8g.drawStr(0, 0, dispbuf.c_str());

    dispbuf = "";
    dispbuf.concat(T22);
    dispbuf += "C "; 
    dispbuf.concat(H22);
    dispbuf += "%";
    u8g.drawStr(0, 16, dispbuf.c_str());

    dispbuf = "";
    dispbuf.concat(BMP180_P / 100);
    dispbuf += "hPa ";
    u8g.drawStr(0, 32, dispbuf.c_str());

    dispbuf = "";
    dispbuf.concat(dustDensity);
    dispbuf += "ug/m3";
    u8g.drawStr(0, 48, dispbuf.c_str());
  } while(u8g.nextPage());
}

//+=============================================================================
// Display IR code
//
void ircode(decode_results *results)
{
  // Panasonic has an Address
  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }

  // Print Code
  Serial.print(results->value, HEX);
}

//+=============================================================================
// Display encoding type
//
void encoding(decode_results *results)
{
  switch (results->decode_type) {
    default:
    case UNKNOWN:      Serial.print("UNKNOWN");       break ;
    case NEC:          Serial.print("NEC");           break ;
    case SONY:         Serial.print("SONY");          break ;
    case RC5:          Serial.print("RC5");           break ;
    case RC6:          Serial.print("RC6");           break ;
    case DISH:         Serial.print("DISH");          break ;
    case SHARP:        Serial.print("SHARP");         break ;
    case JVC:          Serial.print("JVC");           break ;
    case SANYO:        Serial.print("SANYO");         break ;
    case MITSUBISHI:   Serial.print("MITSUBISHI");    break ;
    case SAMSUNG:      Serial.print("SAMSUNG");       break ;
    case LG:           Serial.print("LG");            break ;
    case WHYNTER:      Serial.print("WHYNTER");       break ;
    case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break ;
    case PANASONIC:    Serial.print("PANASONIC");     break ;
    case DENON:        Serial.print("Denon");         break ;
  }
}

//+=============================================================================
// Dump out the decode_results structure.
//
void dumpInfo(decode_results *results)
{
  // Check if the buffer overflowed
  if (results->overflow) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWLEN");
    return;
  }

  Serial.print("# ");
  
  // Show Encoding standard
  Serial.print("Encoding  : ");
  encoding(results);

  // Show Code & length
  Serial.print("Code      : ");
  ircode(results);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
}

void loop() {
  decode_results  results;        // Somewhere to store the results

  if (irrecv.decode(&results)) {  // Grab an IR code
    dumpInfo(&results);           // Output the results
    //dumpRaw(&results);            // Output the results in RAW format
    //dumpCode(&results);           // Output the results as source code
    irrecv.resume();              // Prepare for the next value
  }
  // Gehua remote: P+ 0x8010A620, P- 80102621
}

