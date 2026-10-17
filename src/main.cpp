#include <Arduino.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include "FS.h"
#include <LittleFS.h>

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

typedef struct{
  int day = 0;
  int month = 0;
  int year = 0; //Datum wann empfangen
  int hour = 0;
  int minute = 0;
  int second = 0;
  char weekday[12] ={};
} zeit_t; //Variable für den Namen mit dem aktuellen Monat und Jahr

int button = 33;
int led = 15;

void print_time();
void save_time();
void print_data();

void setup () {
  pinMode(button, INPUT_PULLDOWN);
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  LittleFS.begin(true); //LittleFS starten
  delay(500);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

Wire.begin(21, 22); // SDA, SCL anpassen!

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  //LittleFS.format(); //Entkommentieren damit der von LittleFS belegte SPeicehr gelöscht wird

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop () {
  bool buttonState = digitalRead(button);
  if(buttonState == 1){
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    save_time();
    delay(50);
    print_data();
    delay(50);
  }
  delay(50);
}

void print_time_now(){
    DateTime now = rtc.now();

    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(" (");
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print(" : ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);

 
    Serial.print(") ");

    Serial.println();

    Serial.println();
}

void save_time(){
  DateTime now = rtc.now(); //aktuelle Zeit abfragen
  zeit_t aktuelle_Zeit; //objekt für Zeit erstellen
  aktuelle_Zeit.day = now.day();
  aktuelle_Zeit.month = now.month();
  aktuelle_Zeit.year = now.year();
  strcpy(aktuelle_Zeit.weekday, daysOfTheWeek[now.dayOfTheWeek()]);
  aktuelle_Zeit.hour = now.hour();
  aktuelle_Zeit.minute = now.minute();
  aktuelle_Zeit.second = now.second();

  //Serial.println("Erstelle Variable für den Namen der Datei /test.txt");
  char filename[20] = "test.txt";  //Name der File
  char file_ausfuehrbar[20] = "/";  //damit File geöffnet werden kann muss / vor dem Namen stehen
  strcat(file_ausfuehrbar, filename);  //Nun beides zsm führen damits klappt
  delay(50);
  
  //Öffnen der Datei
  //Serial.println("Öffne die Datei im Schreibmodus w");
  File file = LittleFS.open(file_ausfuehrbar, "a");  //Datei öffnen/erstellen im schreibmodus
  file.printf("%d.%d.%d.%11s %d:%d:%d\n",
            aktuelle_Zeit.day,
            aktuelle_Zeit.month,
            aktuelle_Zeit.year,
            aktuelle_Zeit.weekday,
            aktuelle_Zeit.hour,
            aktuelle_Zeit.minute,
            aktuelle_Zeit.second);
  delay(50);
  file.close(); 
}

void print_data(){
  File file = LittleFS.open("/test.txt", FILE_READ);

  while(file.available()){
    String line = file.readStringUntil('\n');

    if (line.length() == 0) continue;  // skip leere Zeilen

    zeit_t t;

    sscanf(line.c_str(), "%d.%d.%d.%11s %d:%d:%d", &t.day, &t.month, &t.year, t.weekday, &t.hour, &t.minute, &t.second);

    Serial.printf("%d.%d.%d.%11s %d:%d:%d\n", t.day, t.month, t.year, t.weekday, t.hour, t.minute, t.second);
  }

  file.close();
}



/*void setup(){
  //Initialisieren
  Serial.begin(115200);
  
   //50ms sind zu kurz, 500 nötig sonst gehts nicht
  //richtiges Format für Namen der Datei
  

  //In die Datei schreiben
  char Inhalt_write[50] = "Erste Zeile";
  file.println(Inhalt_write);
  file.close();
  Serial.println("Inhalt Erste Zeile in die Datei geschrieben");
  delay(50);

  //Datei öffnen und etwas an die Datei anhängen
  file = LittleFS.open(file_ausfuehrbar, "a");  // a append sonst wird der Inhalt überschrieben
  char Inhalt_append[50] = "Zweite Zeile";
  file.println(Inhalt_append);
  file.close();
  Serial.println("Zweite Zeile an die Datei gehängt");
  delay(50);

  //Inhalt auslesen
  file = LittleFS.open(file_ausfuehrbar, "r");  // a append sonst wird der Inhalt überschrieben
  String Inhalt_read;
  while(file.available()){
    Inhalt_read = file.readStringUntil('\n');
    Serial.println("Dateiinhalt: " +Inhalt_read);
    delay(50);
  }
  file.close();
  delay(50);

  //Inhalt löschen bzw Speicher freigeben
  LittleFS.format();
}
*/