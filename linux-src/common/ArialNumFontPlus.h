// ArialNumFontPlus.c
// Font type    : Special (SubSet)
// Font size    : 32x50 pixels
// Memory usage : 2204 bytes

#if defined(__AVR__)
        #include <avr/pgmspace.h>
        #define fontdatatype const char
#elif defined(__PIC32MX__)
        #define PROGMEM
        #define fontdatatype const char
#elif defined(__arm__)
        #define PROGMEM
        #define fontdatatype const char
#else
  #define PROGMEM
  #define fontdatatype const char
#endif

fontdatatype ArialNumFontPlus[2204] PROGMEM={
0x20,0x32,0x30,0x0B,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x00,0x1F,0xF8,0x00,0x00,0x7F,0xFE,0x00,0x00,0xFF,0xFF,0x00,0x01,0xFF,0xFF,0x00,0x03,0xF8,0x1F,0x80,0x03,0xF0,0x0F,0xC0,0x07,0xE0,0x07,0xC0,0x07,0xC0,0x03,0xE0,0x0F,0xC0,0x03,0xE0,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x01,0xF0,0x1F,0x80,0x01,0xF0,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x3F,0x00,0x00,0xFC,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3F,0x00,0x00,0x7C,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x01,0xF0,0x07,0xC0,0x03,0xE0,0x07,0xC0,0x03,0xE0,0x03,0xE0,0x07,0xC0,0x03,0xF0,0x0F,0xC0,0x01,0xF8,0x1F,0x80,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFE,0x00,0x00,0x7F,0xFC,0x00,0x00,0x1F,0xF8,0x00,0x00,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 0 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x03,0xF0,0x00,0x00,0x07,0xF0,0x00,0x00,0x0F,0xF0,0x00,0x00,0x1F,0xF0,0x00,0x00,0x3F,0xF0,0x00,0x00,0x7F,0xF0,0x00,0x01,0xFF,0xF0,0x00,0x03,0xFD,0xF0,0x00,0x0F,0xF1,0xF0,0x00,0x0F,0xE1,0xF0,0x00,0x0F,0x81,0xF0,0x00,0x0F,0x01,0xF0,0x00,0x0C,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //1 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x00,0x7F,0xFE,0x00,0x01,0xFF,0xFF,0x00,0x03,0xFF,0xFF,0xC0,0x03,0xFF,0xFF,0xE0,0x07,0xE0,0x0F,0xF0,0x0F,0xC0,0x03,0xF0,0x0F,0x80,0x01,0xF0,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x3E,0x00,0x00,0xF8,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0xF8,0x00,0x00,0x01,0xF8,0x00,0x00,0x03,0xF0,0x00,0x00,0x07,0xF0,0x00,0x00,0x0F,0xE0,0x00,0x00,0x1F,0xC0,0x00,0x00,0x3F,0x80,0x00,0x00,0x7F,0x00,0x00,0x00,0xFE,0x00,0x00,0x01,0xFC,0x00,0x00,0x03,0xF8,0x00,0x00,0x07,0xF0,0x00,0x00,0x0F,0xE0,0x00,0x00,0x1F,0xC0,0x00,0x00,0x3F,0x80,0x00,0x00,0x7F,0x00,0x00,0x00,0xFE,0x00,0x00,0x01,0xFC,0x00,0x00,0x03,0xF8,0x00,0x00,0x07,0xF0,0x00,0x00,0x07,0xE0,0x00,0x00,0x0F,0xC0,0x00,0x00,0x0F,0x80,0x00,0x00,0x1F,0x80,0x00,0x00,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x7F,0xFF,0xFF,0xFC,0x7F,0xFF,0xFF,0xFC,0x7F,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 2 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x00,0x3F,0xFC,0x00,0x00,0xFF,0xFE,0x00,0x01,0xFF,0xFF,0x00,0x03,0xFF,0xFF,0x80,0x03,0xE0,0x1F,0xC0,0x07,0xC0,0x0F,0xC0,0x07,0x80,0x07,0xE0,0x0F,0x80,0x03,0xE0,0x0F,0x00,0x03,0xE0,0x0F,0x00,0x03,0xF0,0x0F,0x00,0x01,0xF0,0x00,0x00,0x03,0xF0,0x00,0x00,0x03,0xE0,0x00,0x00,0x03,0xE0,0x00,0x00,0x07,0xE0,0x00,0x00,0x0F,0xC0,0x00,0x00,0x1F,0xC0,0x00,0x00,0x3F,0x80,0x00,0x07,0xFF,0x00,0x00,0x07,0xFF,0x00,0x00,0x07,0xFF,0x00,0x00,0x07,0xFF,0x80,0x00,0x00,0x1F,0xC0,0x00,0x00,0x0F,0xE0,0x00,0x00,0x03,0xE0,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x01,0xF8,0x0F,0x80,0x01,0xF8,0x0F,0x80,0x03,0xF0,0x0F,0xC0,0x07,0xF0,0x07,0xE0,0x0F,0xE0,0x03,0xFC,0x3F,0xC0,0x01,0xFF,0xFF,0x80,0x00,0xFF,0xFF,0x00,0x00,0x7F,0xFC,0x00,0x00,0x0F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 3 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0xFF,0x00,0x00,0x01,0xFF,0x00,0x00,0x01,0xFF,0x00,0x00,0x03,0xFF,0x00,0x00,0x07,0xDF,0x00,0x00,0x07,0xDF,0x00,0x00,0x0F,0x9F,0x00,0x00,0x0F,0x9F,0x00,0x00,0x1F,0x1F,0x00,0x00,0x3E,0x1F,0x00,0x00,0x7E,0x1F,0x00,0x00,0x7C,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x01,0xF0,0x1F,0x00,0x03,0xE0,0x1F,0x00,0x03,0xE0,0x1F,0x00,0x07,0xC0,0x1F,0x00,0x07,0xC0,0x1F,0x00,0x0F,0x80,0x1F,0x00,0x1F,0x80,0x1F,0x00,0x1F,0x00,0x1F,0x00,0x3E,0x00,0x1F,0x00,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //4 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0xF0,0x01,0xFF,0xFF,0xF0,0x01,0xFF,0xFF,0xF0,0x01,0xFF,0xFF,0xF0,0x01,0xFF,0xFF,0xF0,0x03,0xE0,0x00,0x00,0x03,0xC0,0x00,0x00,0x03,0xC0,0x00,0x00,0x03,0xC0,0x00,0x00,0x07,0xC0,0x00,0x00,0x07,0xC0,0x00,0x00,0x07,0xC0,0x00,0x00,0x07,0xC0,0x00,0x00,0x07,0x80,0x00,0x00,0x0F,0x80,0x00,0x00,0x0F,0x80,0xF0,0x00,0x0F,0x8F,0xFE,0x00,0x0F,0xBF,0xFF,0x80,0x0F,0xFF,0xFF,0xC0,0x0F,0xFF,0xFF,0xE0,0x1F,0xFF,0x07,0xF0,0x1F,0xE0,0x03,0xF0,0x1F,0xC0,0x01,0xF8,0x1F,0x00,0x00,0xF8,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x3F,0x00,0x00,0xF8,0x3F,0x00,0x00,0xF8,0x3F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF0,0x1F,0x00,0x01,0xF0,0x1F,0x80,0x03,0xE0,0x0F,0xC0,0x07,0xE0,0x07,0xE0,0x1F,0xC0,0x03,0xFF,0xFF,0x80,0x03,0xFF,0xFF,0x00,0x01,0xFF,0xFE,0x00,0x00,0x7F,0xF8,0x00,0x00,0x0F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 5 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x00,0x3F,0xFE,0x00,0x00,0xFF,0xFF,0x00,0x01,0xFF,0xFF,0x80,0x03,0xFF,0xFF,0xC0,0x07,0xE0,0x07,0xE0,0x07,0xC0,0x03,0xF0,0x0F,0xC0,0x01,0xF0,0x0F,0x80,0x00,0xF8,0x1F,0x80,0x00,0xF8,0x1F,0x80,0x00,0xF8,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3F,0x03,0xF0,0x00,0x3F,0x1F,0xFC,0x00,0x3F,0x7F,0xFE,0x00,0x3F,0xFF,0xFF,0x80,0x3F,0xFF,0xFF,0xC0,0x3F,0xF8,0x0F,0xE0,0x3F,0xE0,0x03,0xF0,0x3F,0xE0,0x03,0xF0,0x3F,0xC0,0x01,0xF8,0x3F,0x80,0x00,0xF8,0x3F,0x80,0x00,0xF8,0x3F,0x00,0x00,0x7C,0x3F,0x00,0x00,0x7C,0x3F,0x00,0x00,0x7C,0x3F,0x00,0x00,0x7C,0x3F,0x00,0x00,0x7C,0x3F,0x00,0x00,0x7C,0x1F,0x00,0x00,0x7C,0x1F,0x80,0x00,0x78,0x1F,0x80,0x00,0xF8,0x1F,0x80,0x00,0xF8,0x0F,0x80,0x00,0xF8,0x0F,0xC0,0x01,0xF0,0x07,0xF0,0x03,0xF0,0x03,0xF8,0x0F,0xE0,0x01,0xFF,0xFF,0xC0,0x00,0xFF,0xFF,0x80,0x00,0x7F,0xFF,0x00,0x00,0x1F,0xFC,0x00,0x00,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 6 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x3F,0xFF,0xFF,0xFC,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0xFC,0x00,0x00,0x01,0xF8,0x00,0x00,0x01,0xF0,0x00,0x00,0x03,0xE0,0x00,0x00,0x03,0xE0,0x00,0x00,0x07,0xC0,0x00,0x00,0x0F,0x80,0x00,0x00,0x1F,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x01,0xF0,0x00,0x00,0x03,0xE0,0x00,0x00,0x03,0xE0,0x00,0x00,0x03,0xE0,0x00,0x00,0x07,0xC0,0x00,0x00,0x07,0xC0,0x00,0x00,0x0F,0x80,0x00,0x00,0x0F,0x80,0x00,0x00,0x0F,0x80,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 7 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x00,0x7F,0xFE,0x00,0x00,0xFF,0xFF,0x80,0x01,0xFF,0xFF,0x80,0x03,0xF8,0x1F,0xC0,0x07,0xE0,0x07,0xE0,0x07,0xC0,0x03,0xE0,0x0F,0xC0,0x03,0xF0,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x01,0xF0,0x07,0xC0,0x03,0xE0,0x07,0xC0,0x03,0xE0,0x03,0xE0,0x07,0xC0,0x01,0xF0,0x07,0x80,0x00,0xFC,0x3F,0x00,0x00,0x7F,0xFF,0x00,0x00,0x3F,0xFC,0x00,0x00,0x7F,0xFE,0x00,0x00,0xFF,0xFF,0x00,0x03,0xFF,0xFF,0xC0,0x07,0xF0,0x0F,0xE0,0x0F,0xC0,0x03,0xF0,0x0F,0x80,0x01,0xF0,0x1F,0x80,0x01,0xF8,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x3E,0x00,0x00,0x7C,0x1F,0x00,0x00,0xF8,0x1F,0x00,0x00,0xF8,0x0F,0x80,0x01,0xF0,0x0F,0x80,0x03,0xF0,0x07,0xC0,0x07,0xE0,0x03,0xF0,0x0F,0xC0,0x01,0xFF,0xFF,0x80,0x00,0xFF,0xFF,0x00,0x00,0x3F,0xFC,0x00,0x00,0x0F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 8 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xF0,0x00,0x00,0x3F,0xFC,0x00,0x00,0xFF,0xFF,0x00,0x01,0xFF,0xFF,0x80,0x03,0xFF,0xFF,0xC0,0x03,0xF8,0x1F,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xE0,0x07,0xF0,0x0F,0xC0,0x03,0xF0,0x0F,0xC0,0x03,0xF8,0x1F,0x80,0x01,0xF8,0x1F,0x80,0x01,0xF8,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x80,0x01,0xFC,0x1F,0x80,0x01,0xFC,0x0F,0xC0,0x03,0xFC,0x0F,0xC0,0x03,0xFC,0x07,0xE0,0x07,0xFC,0x07,0xE0,0x07,0xFC,0x03,0xF8,0x1F,0xFC,0x01,0xFF,0xFF,0xFC,0x00,0xFF,0xFF,0xFC,0x00,0xFF,0xFE,0xFC,0x00,0x7F,0xF8,0xFC,0x00,0x1F,0xE0,0xFC,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0xFC,0x00,0x00,0x01,0xF8,0x3F,0x00,0x01,0xF8,0x3F,0x00,0x01,0xF0,0x1F,0x00,0x03,0xF0,0x1F,0x80,0x07,0xE0,0x0F,0xC0,0x0F,0xE0,0x0F,0xE0,0x3F,0xC0,0x07,0xFF,0xFF,0x80,0x03,0xFF,0xFF,0x00,0x01,0xFF,0xFE,0x00,0x00,0xFF,0xF8,0x00,0x00,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 9 neu
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x01,0xF0,0x00,0x00,0x03,0xF8,0x00,0x00,0x03,0xF8,0x00,0x00,0x03,0xF8,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x01,0xF0,0x00,0x00,0x03,0xF8,0x00,0x00,0x03,0xF8,0x00,0x00,0x03,0xF8,0x00,0x00,0x01,0xF0,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // :
};
