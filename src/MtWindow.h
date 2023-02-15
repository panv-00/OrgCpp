/**
 * File:   MtWindow.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#ifndef MTWINDOW_H
#define MTWINDOW_H

#include "MtMenu.h"

class OrgCpp;

class MtWindow
{
public:
  MtWindow();
  ~MtWindow();

  void SetParent(OrgCpp *app) { parent = app; };
  OrgCpp *GetParent() { return parent; };

  void SetExitMessage(std::string exit_message);
  void ClearScreenContent();
  void AppendScreenContent(std::string content);
  
  void ClearScreen();
  void SetMainMenu(MtMenu *menu);
  void Run();

  void CallOption_Exit();
  void CallOption_Run();
  bool GetInputBoxResult
  (
    MtInputBox box,
    InputBoxType type,
    std::vector<MtPair> select_options,
    std::string &result
  );

protected:
private:
  OrgCpp *parent;

  char _Getch();
  void _CheckTermSize();
  void _ClrScr();
  void _SavePosition();
  void _RestorePosition();
  void _SetColor(ColorCode cc);
  void _MoveUp(int count);
  void _MoveDown(int count);
  void _MoveRight(int count);
  void _MoveLeft(int count);
  void _MoveTo(int r, int c);
  void _HideCursor();
  void _ShowCursor();
  void _draw_rect
  (
      uint16_t start_row, uint16_t start_col,
      uint16_t nrows, uint16_t ncols,
      wchar_t top_left, wchar_t top_center, wchar_t top_right,
      wchar_t mid_left, wchar_t mid_right,
      wchar_t bot_left, wchar_t bot_center, wchar_t bot_right
  );

  bool exit_app;
  AppStatus app_status;
  bool is_getting_input;

  struct termios old_term, new_term;
  struct winsize w;
  std::string exit_message;
  std::string status_message;
  uint16_t term_min_height;
  uint16_t term_min_width;

  MtMenu *main_menu;
  size_t cur_menu;

  std::vector<std::string> screen_content;
};

#endif
