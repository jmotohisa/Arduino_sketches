/*
 *  tenkey.cpp - Time-stamp: <Sat Nov 19 12:11:41 JST 2022>
 *
 *   Copyright (c) 2022  jmotohisa (Junichi Motohisa)  <motohisa@ist.hokudai.ac.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: tenkey.cpp 2022-11-19 12:08:30 jmotohisa $
 */

/*! 
  @file tenkey.cpp 
  @brief 
  @author J. Motohisa
  @date
*/


/*!
  @brief
  @param[in]
  @param[out]
  @param[in,out]
  @return
*/

void drawKeyboard10

/********************************************************************//**
 * @brief     draws the keypad
 * @param[in] None
 * @return    None
 *********************************************************************/
void draw_BoxNButtons()
{
  
   //clear screen black
  tft.fillRect(0, 0, 240, 320, ILI9341_BLACK);
  tft.setFont(0);  
  
  //Draw the Result Box
  tft.fillRect(0, 0, 240, 80, ILI9341_CYAN);

  //Draw C and OK field   
  tft.fillRect  (0,260,80,60,ILI9341_RED);
  tft.fillRect  (160,260,80,60,ILI9341_GREEN); 
  
  //Draw Horizontal Lines
  for (int h=80; h<=320; h+=60)
  tft.drawFastHLine(0, h, 240, ILI9341_WHITE);

  //Draw Vertical Lines
  for (int v=80; v<=240; v+=80)
  tft.drawFastVLine(v, 80, 240, ILI9341_WHITE);

  //Display keypad lables 
  for (int j=0;j<4;j++) {
    for (int i=0;i<3;i++) {
      tft.setCursor(32 + (80*i), 100 + (60*j)); 
      if ((j==3) && (i==2)) tft.setCursor(24 + (80*i), 100 + (60*j)); //OK button
      tft.setTextSize(3);
      tft.setTextColor(ILI9341_WHITE);
      tft.println(symbol[j][i]);
    }
  }
}

