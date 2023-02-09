/*
 * File:   MtInclude.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#ifndef MTINCLUDE_H
#define MTINCLUDE_H

#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <termios.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <signal.h>
#include <sys/select.h>

#define APPNAME     "btcpp"
#define APPVERSION  "0.0.1"

#define BTN_LFT_ (wchar_t) L'\u28CF'
#define BTN_MID_ (wchar_t) L'\u28C9'
#define BTN_RGT_ (wchar_t) L'\u28F9'

#define BOX_ULT_ (wchar_t) L'\u250F'
#define BOX_UMD_ (wchar_t) L'\u2533'
#define BOX_URT_ (wchar_t) L'\u2513'
#define BOX_DLT_ (wchar_t) L'\u2517'
#define BOX_DMD_ (wchar_t) L'\u253B'
#define BOX_DRT_ (wchar_t) L'\u251B'
#define BOX_UDC_ (wchar_t) L'\u2501'
#define BOX_RLM_ (wchar_t) L'\u2503'

#define BXT_ULT_ (wchar_t) L'\u250C'
#define BXT_UMD_ (wchar_t) L'\u252C'
#define BXT_URT_ (wchar_t) L'\u2510'
#define BXT_DLT_ (wchar_t) L'\u2514'
#define BXT_DMD_ (wchar_t) L'\u2534'
#define BXT_DRT_ (wchar_t) L'\u2518'
#define BXT_UDC_ (wchar_t) L'\u2500'
#define BXT_RLM_ (wchar_t) L'\u2502'

#define RDIO_TR_ (wchar_t) L'\u25C9'
#define RDIO_FL_ (wchar_t) L'\u25CB'
#define CHCK_TR_ (wchar_t) L'\u25A3'
#define CHCK_FL_ (wchar_t) L'\u25A1'
#define ICO_TAB_ (wchar_t) L'\u2B7E'
#define ICO_RET_ (wchar_t) L'\u2BA0'
#define ICO_OPN_ (wchar_t) L'\u2397'
#define ICO_SAV_ (wchar_t) L'\u2398'

typedef enum
{
  CLR_DEFAULT  =  0    ,
  CLR_BLACK_FG = 30    ,
  CLR_RED_FG           ,
  CLR_GREEN_FG         ,
  CLR_YELLOW_FG        ,
  CLR_BLUE_FG          ,
  CLR_MAGENTA_FG       ,
  CLR_CYAN_FG          ,
  CLR_WHITE_FG         ,

  CLR_BLACK_BG = 40    ,
  CLR_RED_BG           ,
  CLR_GREEN_BG         ,
  CLR_YELLOW_BG        ,
  CLR_BLUE_BG          ,
  CLR_MAGENTA_BG       ,
  CLR_CYAN_BG          ,
  CLR_WHITE_BG

} ColorCode;

#endif
