//<File !Start!>
// FILE: [ESPKeyboard_GSLC.h]
// Created by GUIslice Builder version: [0.17.b34]
//
// GUIslice Builder Generated GUI Framework File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>

#ifndef _GUISLICE_GEN_H
#define _GUISLICE_GEN_H

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XGraph.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#if defined(DRV_DISP_TFT_ESPI)
  #error E_PROJECT_OPTIONS tab->Graphics Library should be TFT_eSPI
#endif
#include <Adafruit_GFX.h>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_PG2};
enum {E_ELEM_BOX1,E_ELEM_BTN0,E_ELEM_BTN1,E_ELEM_BTN11,E_ELEM_BTN12
      ,E_ELEM_BTN13,E_ELEM_BTN14,E_ELEM_BTN15,E_ELEM_BTN16,E_ELEM_BTN2
      ,E_ELEM_BTN3,E_ELEM_BTN4,E_ELEM_BTN5,E_ELEM_BTN6,E_ELEM_BTN7
      ,E_ELEM_BTN8,E_ELEM_BTN9,E_ELEM_DATE,E_ELEM_FNAME,E_ELEM_GRAPH1
      ,E_ELEM_TEMPERATURE,E_ELEM_TEMPMAX,E_ELEM_TEMPMIN,E_ELEM_TEXT2
      ,E_ELEM_TIME};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_BUILTIN10X16,E_BUILTIN5X8,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                2

#define MAX_ELEM_PG_MAIN 15 // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG2 10 // # Elems total on page
#define MAX_ELEM_PG2_RAM MAX_ELEM_PG2 // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_PG2_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_PG2];
gslc_tsXGraph                   m_sGraph1;
int16_t                         m_anGraphBuf1[0]; // NRows=0

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Element References for direct access
//<Extern_References !Start!>
extern gslc_tsElemRef* m_pElemGraph1;
//<Extern_References !End!>

// Define debug message function
static int16_t DebugOut(char ch);

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY);
bool CbCheckbox(void* pvGui, void* pvElemRef, int16_t nSelId, bool bState);
bool CbDrawScanner(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw);
bool CbKeypad(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId);
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos);
bool CbSpinner(void* pvGui, void *pvElemRef, int16_t nState, void* pvData);
bool CbTickScanner(void* pvGui,void* pvScope);

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
void InitGUIslice_gen()
{
  gslc_tsElemRef* pElemRef = NULL;

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_BUILTIN10X16,GSLC_FONTREF_PTR,NULL,2)) { return; }
    if (!gslc_FontSet(&m_gui,E_BUILTIN5X8,GSLC_FONTREF_PTR,NULL,1)) { return; }
//<Load_Fonts !End!>

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG2,m_asPage2Elem,MAX_ELEM_PG2_RAM,m_asPage2ElemRef,MAX_ELEM_PG2);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
  
  // create E_ELEM_BTN1 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN1,E_PG_MAIN,
    (gslc_tsRect){60,20,40,40},(char*)"1",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN2 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN2,E_PG_MAIN,
    (gslc_tsRect){100,20,40,40},(char*)"2",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN3 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN3,E_PG_MAIN,
    (gslc_tsRect){140,20,40,40},(char*)"3",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN4 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN4,E_PG_MAIN,
    (gslc_tsRect){180,20,40,40},(char*)"4",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN5 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN5,E_PG_MAIN,
    (gslc_tsRect){220,20,40,40},(char*)"5",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN6 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN6,E_PG_MAIN,
    (gslc_tsRect){60,60,40,40},(char*)"6",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN7 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN7,E_PG_MAIN,
    (gslc_tsRect){100,60,40,40},(char*)"7",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN8 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN8,E_PG_MAIN,
    (gslc_tsRect){140,60,40,40},(char*)"8",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN9 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN9,E_PG_MAIN,
    (gslc_tsRect){180,60,40,40},(char*)"9",0,E_BUILTIN10X16,&CbBtnCommon);
  
  // create E_ELEM_BTN0 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN0,E_PG_MAIN,
    (gslc_tsRect){220,60,40,40},(char*)"0",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN11 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN11,E_PG_MAIN,
    (gslc_tsRect){42,101,80,40},(char*)"Clear",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT4,GSLC_COL_BLUE_LT3,GSLC_COL_BLUE_LT2);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN12 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN12,E_PG_MAIN,
    (gslc_tsRect){208,101,80,40},(char*)"OK",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT4,GSLC_COL_BLUE_LT3,GSLC_COL_BLUE_LT2);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_BTN13 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN13,E_PG_MAIN,
    (gslc_tsRect){126,101,80,40},(char*)"Cancel",0,E_BUILTIN10X16,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT4,GSLC_COL_BLUE_LT3,GSLC_COL_BLUE_LT2);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // Create E_ELEM_TEXT2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT2,E_PG_MAIN,(gslc_tsRect){80,190,12,16},
    (char*)"0",0,E_BUILTIN10X16);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){132,180,50,50});

  // -----------------------------------
  // PAGE: E_PG2
  

  // Create graph E_ELEM_GRAPH1
  pElemRef = gslc_ElemXGraphCreate(&m_gui,E_ELEM_GRAPH1,E_PG2,
    &m_sGraph1,(gslc_tsRect){24,24,200,190},E_BUILTIN5X8,(int16_t*)&m_anGraphBuf1,
        0,((gslc_tsColor){255,200,0}));
  gslc_ElemXGraphSetStyle(&m_gui,pElemRef, GSLCX_GRAPH_STYLE_DOT, 5);
  m_pElemGraph1 = pElemRef;
  
  // Create E_ELEM_DATE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATE,E_PG2,(gslc_tsRect){1,1,120,16},
    (char*)"2024/10/10",0,E_BUILTIN10X16);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // Create E_ELEM_TIME text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TIME,E_PG2,(gslc_tsRect){220,1,96,16},
    (char*)"12:00:00",0,E_BUILTIN10X16);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // Create E_ELEM_TEMPERATURE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEMPERATURE,E_PG2,(gslc_tsRect){241,19,72,16},
    (char*)"25.00C",0,E_BUILTIN10X16);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GREEN_LT4);
  
  // create E_ELEM_BTN14 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN14,E_PG2,
    (gslc_tsRect){233,109,80,40},(char*)"refresh",0,E_BUILTIN5X8,&CbBtnCommon);
  
  // create E_ELEM_BTN15 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN15,E_PG2,
    (gslc_tsRect){232,151,80,40},(char*)"range",0,E_BUILTIN10X16,&CbBtnCommon);
  
  // Create E_ELEM_FNAME text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_FNAME,E_PG2,(gslc_tsRect){238,57,48,8},
    (char*)"filename",0,E_BUILTIN5X8);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_ORANGE);
  
  // Create E_ELEM_TEMPMAX text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEMPMAX,E_PG2,(gslc_tsRect){4,20,18,8},
    (char*)"200",0,E_BUILTIN5X8);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_RIGHT);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // Create E_ELEM_TEMPMIN text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEMPMIN,E_PG2,(gslc_tsRect){4,210,6,8},
    (char*)"0",0,E_BUILTIN5X8);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_RIGHT);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  
  // create E_ELEM_BTN16 button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN16,E_PG2,
    (gslc_tsRect){231,193,80,40},(char*)"interval",0,E_BUILTIN5X8,&CbBtnCommon);
//<InitGUI !End!>

//<Startup !Start!>
//<Startup !End!>

}

#endif // end _GUISLICE_GEN_H
