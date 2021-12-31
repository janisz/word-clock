#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Timezone.h>    // https://github.com/JChristensen/Timezone

#include <ThreeWire.h>
#include <RtcDS1302.h>
// CONNECTIONS:



#define NUM_LEDS 16*16
#define DATA_PIN 2        // D4

#define DS1302_RST_PIN 16 // D0
#define DS1302_CLK_PIN 5  // D1
#define DS1302_DAT_PIN 4  // D2

// https://github.com/Makuna/Rtc/blob/master/examples/DS1302_Simple/DS1302_Simple.ino
ThreeWire myWire(DS1302_CLK_PIN, DS1302_DAT_PIN, DS1302_RST_PIN);
RtcDS1302<ThreeWire> Rtc(myWire);

// Central European Time (Frankfurt, Paris, Warsaw)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);

const char* ssid     = "clock";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "WhatTimeIsIt?";     // The password of the Wi-Fi network

const char* ntpServer = "pool.ntp.org";


CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

  Serial.print("HELLO");

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();
  Serial.println();

  RtcDateTime compiled = RtcDateTime("Dec 31 2020", "21:31:24");
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled || !Rtc.IsDateTimeValid())
  {
    if (now < compiled) {
      Serial.println(now.Epoch32Time());
      Serial.println("Now is earlier than compiled");
    }
    if (!Rtc.IsDateTimeValid()) {
      Serial.println("RTC lost confidence in the DateTime!");
    }
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing



    WiFi.begin(ssid, password);             // Connect to the network
    Serial.print("Connecting to ");
    Serial.print(ssid); Serial.println(" ...");

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
      clear(CHSV(i++, 255, 255));
      FastLED.show();
      delay(10);
    }

    WiFiUDP ntpUDP;

    // By default 'pool.ntp.org' is used with 60 seconds update interval and
    // no offset
    NTPClient timeClient(ntpUDP);

    Serial.println('\n');
    Serial.println("WiFi: Connection established!");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
    timeClient.begin();
    while (!timeClient.forceUpdate()) {
      clear(CHSV(i++, 255, 255));
      FastLED.show();
      delay(10);
    }
    Serial.println("Got time from NTP server: ");
    Serial.println(timeClient.getFormattedTime());
    RtcDateTime ntpTime = RtcDateTime();
    ntpTime.InitWithEpoch32Time(timeClient.getEpochTime());
    Serial.println(timeClient.getEpochTime());
    Serial.println(ntpTime.Epoch32Time());

    if (Rtc.GetIsWriteProtected())
    {
      Serial.println("RTC was write protected, enabling writing now");
      Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
    }
    Rtc.SetDateTime(ntpTime);
    while (!Rtc.IsDateTimeValid()) {
      clear(CHSV(i++, 255, 255));
      FastLED.show();
      delay(10);
    }

    Serial.println(Rtc.GetDateTime().Epoch32Time());
    Serial.println("Reset..");
    ESP.restart();
  }
}


void loop() {
  RtcDateTime now = Rtc.GetDateTime();
  if (!now.IsValid())
  {
    // Common Causes:
    //    1) the battery on the device is low or even missing and the power line was disconnected
    Serial.println("RTC lost confidence in the DateTime!");
    Serial.println("Reset..");
    ESP.restart();
  }

  clear(CRGB::Black);
  printDateTime(CE, now.Epoch32Time(), "Warsaw");
  FastLED.show();
  delay(1000);
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
  showTime(hour(t), minute(t), CHSV(t / 10, 255, getBrightness()));

}


void clear(CRGB color) {
  iter(0, NUM_LEDS - 1, color);
}

void za(CRGB color) {
  iter(14, 15, color);
}


void dziesiec(CRGB color) {
  iter(5, 12, color);
}


void trzy(CRGB color) {
  iter(0, 3, color);
}


void dwadziescia(CRGB color) {
  iter(17, 27, color);
}


void kwadrans(CRGB color) {
  iter(38, 45, color);
}


void kwadranse(CRGB color) {
  iter(37, 45, color);
}

void piec(CRGB color) {
  iter(48, 51, color);
}

void wpol(CRGB color) {
  iter(54, 57, color);
}

void _do(CRGB color) {
  iter(60, 61, color);
}

void po(CRGB color) {
  iter(62, 63, color);
}

void jedenastej(CRGB color) {
  iter(70, 79, color);
}

void druga(CRGB color) {
  iter(64, 68, color);
}

void piata(CRGB color) {
  iter(81, 85, color);
}

void dziesiatej(CRGB color) {
  iter(86, 95, color);
}

void dziesiata(CRGB color) {
  iter(102, 110, color);
}

void osma(CRGB color) {
  iter(96, 99, color);
}

void trzecia(CRGB color) {
  iter(113, 119, color);
}

void trzeciej(CRGB color) {
  iter(120, 127, color);
}

void siodma(CRGB color) {
  iter(138, 143, color);
}

void pierwszej(CRGB color) {
  iter(129, 137, color);
}

void jedenasta(CRGB color) {
  iter(144, 152, color);
}

void szostej(CRGB color) {
  iter(153, 159, color);
}

void siodmej(CRGB color) {
  iter(169, 175, color);
}

void dziewiata(CRGB color) {
  iter(160, 168, color);
}

void pierwsza(CRGB color) {
  iter(176, 183, color);
}

void czwartej(CRGB color) {
  iter(184, 191, color);
}

void piatej(CRGB color) {
  iter(201, 206, color);
}

void dwunasta(CRGB color) {
  iter(193, 200, color);
}

void dwunastej(CRGB color) {
  iter(208, 216, color);
}

void szosta(CRGB color) {
  iter(218, 223, color);
}

void czwarta(CRGB color) {
  iter(233, 239, color);
}

void drugiej(CRGB color) {
  iter(224, 230, color);
}

void dziewiatej(CRGB color) {
  iter(240, 249, color);
}

void osmej(CRGB color) {
  iter(251, 255, color);
}


void (*beforeHour[])(CRGB) = {
  dwunasta, pierwsza, druga, trzecia, czwarta, piata, szosta, siodma, osma, dziewiata, dziesiata, jedenasta, dwunasta
};


void (*afterHour[])(CRGB) = {
  dwunastej, pierwszej, drugiej, trzeciej, czwartej, piatej, szostej, siodmej, osmej, dziewiatej, dziesiatej, jedenastej, dwunastej
};

void iter(int from, int to, CRGB color) {
  for (int i = from; i <= to; i++) {
    leds[i] = color;
  }
}

void test() {
  while (true) {
    for (int i = 0; i <= 12; i++) {
      for (int m = 0; m <= 60; m++) {
        clear(CRGB::Black);
        showTime(i, m, CRGB::Red);
        FastLED.show();
        delay(200);
      }
    }
  }
}

void showTime(int h, int m, CRGB color) {

  h = h % 12;
  int next = (h + 1) % 12;
  if (m > 58) {
    beforeHour[next](color);
    return;
  }
  if (m > 56) {
    za(color);
    trzy(color);
    beforeHour[next](color);
    return;
  }
  if (m > 52) {
    za(color);
    piec(color);
    beforeHour[next](color);
    return;
  }
  if (m > 47) {
    za(color);
    dziesiec(color);
    beforeHour[next](color);
    return;
  }
  if (m > 42) {
    za(color);
    kwadrans(color);
    beforeHour[next](color);
    return;
  }
  if (m > 37) {
    za(color);
    dwadziescia(color);
    beforeHour[next](color);
    return;
  }
  if (m > 32) {
    za(color);
    dwadziescia(color);
    piec(color);
    beforeHour[next](color);
    return;
  }
  if (m > 27) {
    wpol(color);
    _do(color);
    afterHour[next](color);
    return;
  }
  if (m > 22) {
    dwadziescia(color);
    piec(color);
    po(color);
    afterHour[h](color);
    return;
  }
  if (m > 16) {
    dwadziescia(color);
    po(color);
    afterHour[h](color);
    return;
  }
  if (m > 12) {
    kwadrans(color);
    po(color);
    afterHour[h](color);
    return;
  }
  if (m > 7) {
    dziesiec(color);
    po(color);
    afterHour[h](color);
    return;
  }
  if (m > 3) {
    piec(color);
    po(color);
    afterHour[h](color);
    return;
  }
  if (m > 1) {
    trzy(color);
    po(color);
    afterHour[h](color);
    return;
  }
  beforeHour[h](color);
}

int getBrightness() {
  int b = (1024 - analogRead(A0)) / 4;
  b += 10;
  if (b > 256) {
    b = 255;
  }
  return b;
}
