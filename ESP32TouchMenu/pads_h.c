/*
   Pads
*/

void showAlphaPad(int x, int y) {
  GLOBAL_PAD_XPOS = x;
  GLOBAL_PAD_YPOS = y;
  GLOBAL_TOUCH_TYPE = ALPHA;
  //ClearScreen();
  tft.fillRect(0, 0, 319, 239, ILI9341_BLACK);
  tft.drawRoundRect(x - 5, y - 5, 255, 105, 10, ILI9341_YELLOW);
  //  first row
  int keyx = x;
  int keyy = y;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("Q");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("W");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("E");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("R");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("T");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("Y");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("U");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("I");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("O");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("P");
  //  second row of keys
  keyx = x;
  keyy = y + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("A");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("S");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("D");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("F");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("G");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("H");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("J");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("K");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("L");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 4, keyy + 6);
  tft.print("cr");
  //  third row of keys
  keyx = x;
  keyy = y + 50;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("Z");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("X");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("C");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("V");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("B");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("N");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 7, keyy + 6);
  tft.print("M");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 1, keyy + 6);
  tft.print("ctrl");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 2, keyy + 6);
  tft.print("ALT");
  keyx = keyx + 25;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 2, keyy + 6);
  tft.print("esc");
  //  fourth row of keys - will be adding more toggle buttons
  keyx = x + 75;
  keyy = y + 75;
  tft.fillRoundRect(keyx, keyy, 70, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 22, keyy + 6);
  tft.print("SPACE");
  keyx = keyx + 85;
  tft.fillRoundRect(keyx, keyy, 20, 20, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setCursor(keyx + 2, keyy + 6);
  tft.print("bsp");
}
void showNumPad(int x, int y) {
  GLOBAL_PAD_XPOS = x;
  GLOBAL_PAD_YPOS = y;
  GLOBAL_TOUCH_TYPE = NUMERIC;
  tft.drawRoundRect(x - 5, y - 5, 90, 105, 10, border_colour);
  tft.fillRoundRect(x, y, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x + 30, y, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x + 60, y, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x, y + 25, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x + 30, y + 25, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x + 60, y + 25, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x, y + 50, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x + 30, y + 50, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x + 60, y + 50, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x, y + 75, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x + 30, y + 75, 20, 20, 5, num_button_colour);
  tft.fillRoundRect(x + 60, y + 75, 20, 20, 5, num_button_colour);
  tft.setTextColor(num_text_colour,  num_button_colour);
  tft.setCursor(x + 7, y + 6);
  tft.print("1");
  tft.setCursor(x + 37, y + 6);
  tft.print("2");
  tft.setCursor(x + 67, y + 6);
  tft.print("3");
  tft.setCursor(x + 7, y + 31);
  tft.print("4");
  tft.setCursor(x + 37, y + 31);
  tft.print("5");
  tft.setCursor(x + 67, y + 31);
  tft.print("6");
  tft.setCursor(x + 7, y + 56);
  tft.print("7");
  tft.setCursor(x + 37, y + 56);
  tft.print("8");
  tft.setCursor(x + 67, y + 56);
  tft.print("9");
  tft.setCursor(x + 7, y + 81);
  tft.print(".");
  tft.setCursor(x + 37, y + 81);
  tft.print("0");
  tft.setCursor(x + 65, y + 81);
  tft.print("OK");
}
void showYesNo(int x, int y) {
  GLOBAL_PAD_XPOS = x;
  GLOBAL_PAD_YPOS = y;
  GLOBAL_TOUCH_TYPE = YESNO;
  tft.drawRoundRect( x, y, 80, 30, 10, border_colour);
  tft.fillRoundRect(x + 5, y + 5, 30, 20, 5, yes_no_button_colour);
  tft.fillRoundRect(x + 45, y + 5, 30, 20, 5, yes_no_button_colour);
  tft.setTextColor(ILI9341_BLACK, yes_no_button_colour);
  tft.setCursor(x + 10, y + 12);
  tft.print("YES");
  tft.setCursor(x + 55, y + 12);
  tft.print("NO");
}
/*
   this can be added after ShowYesNo(x,y) is used
*/
bool checkYesNo(int x, int y) {
  GLOBAL_TOUCH_TYPE = YESNO;
  showYesNo(100, 100);
  bool YES = false;
  // simply split the x axis
  // TODO:  return with response
  // just check for true else...
  if (YES)return true;
  return false;
}
int getKeyboardNumeric() {
  int val = 0;
  int x = 75;
  int y = 75;
  showNumPad( x, y);
  bool ENTRY_COMPLETE = false;

  return val;
}
String getKeyboardString() {
  GLOBAL_TOUCH_TYPE = ALPHA;
  String str;
  //show the keyboard
  showAlphaPad(5, 110);
  //scan for keys pressed and append to @str
  // this will override the button test and scan the whole display for keys
  // if key is the return key then finish up
  // return control to TouchType.Menu
  GLOBAL_TOUCH_TYPE = MENU;
  return str;
}