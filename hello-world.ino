#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Timezone.h>    // https://github.com/JChristensen/Timezone


#define NUM_LEDS 16*16
#define DATA_PIN 5  //It's D1 on LoLin NodeMCU

// Central European Time (Frankfurt, Paris, Warsaw)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);

const char* ssid     = "...";     // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "...";     // The password of the Wi-Fi network

const char* ntpServer = "pool.ntp.org";
WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
    delay(1000);

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

  test();

  Serial.println("HELLO");

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    showTime(i % 24, i % 30, i % 60);
    Serial.print(++i);
    Serial.print(' ');
    delay(10);
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  timeClient.begin();

}

void loop() {
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  printDateTime(CE, timeClient.getEpochTime(), "Warsaw");
  delay(1000);
}


void showTime(int h, int m, int s) {
  clear(CRGB::Black);
  for (int i = 0; i < h; i++) {
    leds[i] = CRGB::Red;
  }
  int row = 4 * 16;
  for (int i = row; i < m + row; i++) {
    leds[i] = CRGB::Green;
  }
  row = 9 * 16;
  for (int i = row; i < s + row; i++) {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();
}


// given a Timezone object, UTC and a string description, convert and print local time with time zone
void printDateTime(Timezone tz, time_t utc, const char *descr)
{
  char buf[40];
  char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
  TimeChangeRule *tcr;        // pointer to the time change rule, use to get the TZ abbrev

  time_t t = tz.toLocal(utc, &tcr);
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
          hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tcr -> abbrev);
  Serial.print(buf);
  Serial.print(' ');
  Serial.println(descr);

  showTime(hour(t), minute(t), second(t));
}


void test() {
  clear(CRGB::Red);
  FastLED.show();
  delay(100);
  clear(CRGB::Green);
  FastLED.show();
  delay(100);
  clear(CRGB::Blue);
  FastLED.show();
  delay(100);
  clear(CRGB::White);
  FastLED.show();
  delay(100);
  clear(CRGB::Black);
  FastLED.show();

}

void clear(CRGB color) {
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = color;
  }
}

