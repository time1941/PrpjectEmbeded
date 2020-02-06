#include "TinyGPS++.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define oled_reset -1
Adafruit_SSD1306 oled(oled_reset);
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10 
#define SEALEVELPRESSURE_HPA (1013.25)
 
Adafruit_BME280 bme; 
 
unsigned long delayTime;
char auth[] = "rvUJzam_7xffwNm82MAGNvPmTOSQAYCU";
char ssid[] = "Korpai_2.4G";
char pass[] = "Timerong2541";
BlynkTimer timer;
TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data

void Line_Notify(String message) ;

//line
String message = "https%3A%2F%2Fwww.google.com%2Fmaps%2Fplace%2F";
//end

String lat_ ;
String lon_ ;
int speed_ = 0 ;
int Index1 = 0;
int temp;
int pres;
int alti;
int hum;


#define LINE_TOKEN "uLc15VK8sJJEG5vhTOysnjE7oThlMRrGAxOMvJMkKCa"
const int sw1 = D5;
int st_sw1 = 0;



void setup()
{
Serial.begin(9600);//This opens up communications to the Serial monitor in the Arduino IDE
Serial.println("GPS Start");//Just show to the monitor that the sketch has started
Blynk.begin(auth, ssid, pass);
message += lat_ + "%2C%2B" + lon_ + "%2F%40" + lat_ + "%40"+ lon_ + "%2C18z  " ;
pinMode(sw1, INPUT);
oled.begin(SSD1306_SWITCHCAPVCC,0x3c); 
unsigned status;
status = bme.begin(0x76);
if (!status) {
Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
while (1);
}
delayTime = 1000; 
timer.setInterval(1000L, sendGPS);
}

void sendGPS()
{
  Blynk.virtualWrite(V1, Index1, lat_, lon_, "value");
  Blynk.virtualWrite(V2, speed_);
  Blynk.virtualWrite(V3, temp);
  Blynk.virtualWrite(V4, pres);
  Blynk.virtualWrite(V5, alti);
  Blynk.virtualWrite(V6, hum);
}

void loop()
{
  Blynk.run();   /* Initiates Blynk */
  timer.run();    /* run BlynkTimer */
st_sw1 = digitalRead(sw1);
printValues();

Serial.println();
Serial.println("-----------------------------------------------");
Serial.println();
gps_sen();

delay(500);  
}


void gps_sen(){
  while(Serial.available())//While there are characters to come from the GPS
{
gps.encode(Serial.read());//This feeds the serial NMEA data into the library one char at a time
}
if(gps.location.isUpdated())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
{
//Get the latest info from the gps object which it derived from the data sent by the GPS unit
lat_ = String(gps.location.lat(),6);
lon_ = String(gps.location.lng(),6);
speed_ =  gps.speed.mph() ;

Serial.println("Latitude:");
Serial.println(lat_);
Serial.println("Longitude:");
Serial.println(gps.location.lng(), 6);
Serial.println("Speed MPH:");
Serial.println(gps.speed.mph());
Serial.println("");
message = "https%3A%2F%2Fwww.google.com%2Fmaps%2Fplace%2F";
message += lat_ + "%2C%2B" + lon_ + "%2F%40" + lat_ + "%40"+ lon_ + "%2C18z  " ;
if (st_sw1 == 0) {
    //while(digitalRead(SW) == HIGH) delay(10);
    Serial.println("Enter !");
    Line_Notify(message); 
    st_sw1 = st_sw1 - 1;  
  }
    
}
}

void printValues() {
temp = bme.readTemperature();
pres = bme.readPressure() / 100.0F;
alti = bme.readAltitude(SEALEVELPRESSURE_HPA);
hum = bme.readHumidity();  
Serial.print("Temperature = ");
Serial.print(bme.readTemperature());
Serial.println(" *C");
 
Serial.print("Pressure = ");
 
Serial.print(bme.readPressure() / 100.0F);
Serial.println(" hPa");
 
Serial.print("Approx. Altitude = ");
Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
Serial.println(" m");
 
Serial.print("Humidity = ");
Serial.print(bme.readHumidity());
Serial.println(" %");

oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.setTextSize(1.8);
  oled.print("Temp : ");
  oled.setCursor(45,0);
  oled.print(bme.readTemperature());
  oled.setCursor(90,0);
  oled.println(" C");
  
  oled.setCursor(0,17);
  oled.setTextSize(1.8);
  oled.print("hPa : ");
  oled.setCursor(45,17);
  oled.print(bme.readPressure() / 100.0F);
  oled.setCursor(90,17);
  oled.println(" hPa");

  oled.setCursor(0,34);
  oled.setTextSize(1.8);
  oled.print("Alti : ");
  oled.setCursor(45,34);
  oled.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  oled.setCursor(90,34);
  oled.println(" m");

  oled.setCursor(0,51);
  oled.setTextSize(1.8);
  oled.print("Hum : ");
  oled.setCursor(45,51);
  oled.print(bme.readHumidity());
  oled.setCursor(90,51);
  oled.println(" %");
  
  oled.display();
delay(1000);
}

void Line_Notify(String message) {

  WiFiClientSecure client;
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;
  }
  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Connection: close\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message; 
  client.print(req);  
  delay(20);
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }    
  }
}
