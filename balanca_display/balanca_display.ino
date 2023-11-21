#include "HX711.h"
#include <LiquidCrystal.h>
#include <ButtonDebounce.h>

#define BUTTON_DEBOUNCE_DELAY   500   // [ms]

#define DT A1
#define SCK A0

#define START_BUTTON 6
#define INC_ALARM_BUTTON 7
#define DEC_ALARM_BUTTON 8
#define CONFIRM_ALARM_BUTTON 9

HX711 escala;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

ButtonDebounce startButton(START_BUTTON, BUTTON_DEBOUNCE_DELAY);
ButtonDebounce incAlarmButton(INC_ALARM_BUTTON, BUTTON_DEBOUNCE_DELAY);
ButtonDebounce decAlarmButton(DEC_ALARM_BUTTON, BUTTON_DEBOUNCE_DELAY);
ButtonDebounce confirmAlarmButton(CONFIRM_ALARM_BUTTON, BUTTON_DEBOUNCE_DELAY);


int currentScreen = 0;
float upperAlarm = 0;
float lowerAlarm = 0;
float tempAlarm = 0;

void incrementAlarm(const int state) {
  if (incAlarmButton.state() == LOW) {
    if ((tempAlarm + 0.5) <= 5.0) {
       tempAlarm = tempAlarm + 0.5;
       lcd.setCursor(1, 1);
       lcd.print("Valor: ");
       lcd.print(tempAlarm, 1);
       lcd.print(" kg");
    } 
  }
}

void decrementAlarm(const int state) {
  if (decAlarmButton.state() == LOW) {
    if ((tempAlarm - 0.5) >= 0.0) {
       tempAlarm = tempAlarm - 0.5;
       lcd.setCursor(1, 1);
       lcd.print("Valor: ");
       lcd.print(tempAlarm, 1);
       lcd.print(" kg");
    } 
  }
}

void confirmAlarm(const int state) {
  if (currentScreen == 1) {
    Serial.print("Confirming upper alarm");
    upperAlarm = tempAlarm;
  }
  if (currentScreen == 2) {
    Serial.print("Confirming lower alarm");
    lowerAlarm = tempAlarm;
  }
}

void navigateMenu(const int state) {
  if (state == LOW) {
    lcd.clear();

    currentScreen++;

    // Restart from first screen if menu is exceeded
    if (currentScreen > 3) {
      currentScreen = 0;
    }
  }
}

void setup() {
 escala.begin (DT, SCK);

 startButton.setCallback(navigateMenu);
 incAlarmButton.setCallback(incrementAlarm);
 decAlarmButton.setCallback(decrementAlarm);
 confirmAlarmButton.setCallback(confirmAlarm);

 lcd.begin(16, 2);

 Serial.begin(9600);
 Serial.print("Leitura do Valor ADC: ");
 Serial.println(escala.read()); // Aguada até o dispositivo estar pronto
 Serial.println("Nao coloque nada na balanca!");
 Serial.println("Iniciando...");
 
 escala.set_scale(435406.037); // Substituir o valor encontrado para escala
 escala.tare(20); // O peso é chamado de Tare.
 
 lcd.clear();
 lcd.setCursor(4, 0);
 lcd.print("BALANCA");
}

void loop() {
  startButton.update();
  incAlarmButton.update();
  decAlarmButton.update();
  confirmAlarmButton.update();


  switch (currentScreen) {
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Alarme superior");
        lcd.setCursor(1, 1);
        lcd.print("Valor: ");
        lcd.print(tempAlarm, 1);
        lcd.print(" kg");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Alarme inferior");
        lcd.setCursor(1, 1);
        lcd.print("Valor: ");
        lcd.print(tempAlarm, 1);
        lcd.print(" kg");
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("PESO: ");
        lcd.print(escala.get_units(20) , 2);
        lcd.print(" kg");
        lcd.setCursor(0, 1);
        lcd.print("SUP ");
        lcd.print(upperAlarm, 1);
        lcd.setCursor(8, 1);
        lcd.print("INF ");
        lcd.print(lowerAlarm, 1);
        break;
    }
}