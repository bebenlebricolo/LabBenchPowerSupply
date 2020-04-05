//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0); 
}


void loop()
{
    lcd.setCursor(0,0);
    lcd.print("Voltage:");
    lcd.setCursor(0,1);
    lcd.print("Current:");

  for (uint16_t i =0 ; i < 8000 ; i= i + 100)
  {

    char msg[5] = {0};
    lcd.setCursor(8,0);
    lcd.print("16 V");
    lcd.setCursor(8,1);
    memset(msg, ' ', 5);
    snprintf(msg,5,"%d  ",i); 
    lcd.print(msg);
    lcd.setCursor(12,1);
    lcd.print("mA");
    delay(500);  
    //lcd.clear();
  }

}
