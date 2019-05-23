#include "LCD.h"


namespace dweedee {

    LCD::LCD() : lcd_(LiquidCrystal_I2C(I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS)) {
        // todo
    }

    LCD::LCD(uint8_t i2cAddr, uint8_t rows, uint8_t columns) : lcd_(LiquidCrystal_I2C(i2cAddr, rows, columns)) {
        // todo
    }

}