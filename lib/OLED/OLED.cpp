#include "OLED.h"
#include "OLED_BITMAP.h"

// MicroOLED oled(PIN_RESET, PIN_DC, PIN_CS);
MicroOLED oled(PIN_RESET, DC_JUMPER); // Example I2C declaration

void oled_setup()
{
  // These three lines of code are all you need to initialize the
  // OLED and print the splash screen.

  // Before you can start using the OLED, call begin() to init
  // all of the pins and configure the OLED.
  oled.begin();
  // clear(ALL) will clear out the OLED's graphic memory.
  // clear(PAGE) will clear the Arduino's display buffer.
  oled.clear(PAGE);//clear the screen before we draw our image
   oled.drawBitmap(OLED_BITMAP_BENDER);//call the drawBitmap function and pass it the array from above
   oled.display();//display the imgae
}

void oled_clear()
{
    oled.clear(PAGE);
}

void oled_print_title(String title)
{
  oled_print_title(title, 0);
}
// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
void oled_print_title(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled_clear();
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (title.length()/2)),
                 middleY - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
}

void oled_print(String text)
{
  oled_clear();
  oled_print(text, 0, 0);
}

void oled_print(String text, int posX, int posY)
{
  oled.setFontType(0);
  oled.setCursor(posX, posY);
  oled.print(text);
  oled.display();
}
