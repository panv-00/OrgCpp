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
#include <map>
#include <functional>
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

#define BXT_ULT_ (wchar_t) L'\u250C' // Thin Box Border
#define BXT_UMD_ (wchar_t) L'\u252C' // Thin Box Border
#define BXT_URT_ (wchar_t) L'\u2510' // Thin Box Border
#define BXT_DLT_ (wchar_t) L'\u2514' // Thin Box Border
#define BXT_DMD_ (wchar_t) L'\u2534' // Thin Box Border
#define BXT_DRT_ (wchar_t) L'\u2518' // Thin Box Border
#define BXT_UDC_ (wchar_t) L'\u2500' // Thin Box Border
#define BXT_RLM_ (wchar_t) L'\u2502' // Thin Box Border

#define RDIO_TR_ (wchar_t) L'\u25C9' // Radiobutton, on
#define RDIO_FL_ (wchar_t) L'\u25CB' // Radiobutton, off
#define CHCK_TR_ (wchar_t) L'\u25A3' // Checkbox, checked
#define CHCK_FL_ (wchar_t) L'\u25A1' // Checkbox, Unchekced
#define ICO_TAB_ (wchar_t) L'\u2B7E' // [Tab] key
#define ICO_RET_ (wchar_t) L'\u2BA0' // [Return] key
#define ICO_OPN_ (wchar_t) L'\u2397' // Open
#define ICO_SAV_ (wchar_t) L'\u2398' // Save
#define ICO_HGL_ (wchar_t) L'\u231B' // Hourglass
#define ICO_BAD_ (wchar_t) L'\u2BBF' // Error X
#define ICO_OKE_ (wchar_t) L'\u2714' // Okey

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
