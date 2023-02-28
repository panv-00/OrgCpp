/**
 * File:   UI.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-24
 */

#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <string>
#include <vector>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <wchar.h>
#include <stdexcept>

#include "Utils.h"
#include "DBase.h"

#define APPNAME "OrgCpp"
#define APPVERSION "0.0.1"

#define ICO_ARR_ (wchar_t) L'\u21c9' // Arrow Select

#define TAB          9
#define ESCAPE_CHAR 27
#define PAGE_UP     53
#define PAGE_DN     54
#define ARROW_CHAR  91
#define ARROW_UP    65
#define ARROW_DN    66
#define ARROW_RT    67
#define ARROW_LT    68
#define PAGE_UP_DN 126
#define BACKSPACE  127

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

class UI
{
public:
  UI();
  ~UI();

  void Run();

private:
  void _SetupWelcomePage();
  void _ListGroups();
  void _ListTickets(uint64_t group_id);
  void _AddHeaderContent(std::string content);
  void _AddContent(std::string content);
  void _RefreshScreen();
  char _Getch();
  void _CheckTermSize();
  void _EchoInputLine();
  void _ClrScr();
  void _HideCursor();
  void _ShowCursor();
  void _MoveTo(uint16_t r, uint16_t c);
  void _MoveRight(uint16_t count);
  void _SetColor(ColorCode cc);

  bool exit_app;

  std::string exit_message;
  std::string status_line;
  std::string input_line;

  std::string screen_title;
  std::vector<std::string> screen_header;
  std::vector<std::string> screen_content;

  size_t sc_start_at;
  uint16_t sc_num_of_lines;
  size_t num_of_pages;
  size_t sc_page;

  struct termios o_term, n_term;
  struct winsize w;
  uint16_t min_h;
  uint16_t min_w;

  uint16_t cur;
  uint16_t start_at;

  Group cur_group;

  DBase *db;
};

#endif /* ifndef UI_H */
