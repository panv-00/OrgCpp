/*
 * File:   MtWindow.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#ifndef MTWINDOW_H
#define MTWINDOW_H

#include "MtInclude.h"
#include "MtMenu.h"

class MtWindow
{
public:
  typedef struct
  {
    std::string prompt;
    uint16_t max_length;
    InputBoxType type;

  } MtInputBox;

  MtWindow();
  ~MtWindow();

  void SetExitMessage(std::string exit_message);
  void ClearScreen();
  void SetMainMenu(MtMenu *menu);
  void Run();

  void CallOption_Exit();
  void CallOption_Run();
  std::string GetInputBoxResult(MtWindow::MtInputBox box);

protected:
private:

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

  MtMenu *main_menu;
  size_t cur_menu;
};

#endif
