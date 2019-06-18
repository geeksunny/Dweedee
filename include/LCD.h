#ifndef DWEEDEE_LCD_H
#define DWEEDEE_LCD_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDRESS     0x27
#define LCD_COLUMNS     16
#define LCD_ROWS        2

namespace dweedee {

class LCD {

  LiquidCrystal_I2C lcd_;

 public:
  LCD();
  LCD(uint8_t i2cAddr, uint8_t rows, uint8_t columns);
};

}

#endif //DWEEDEE_LCD_H
