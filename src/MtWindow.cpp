/*
 * File:   MtWindow.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "MtWindow.h"

void handle_resize(int sig)
{
  // TODO
}

MtWindow::MtWindow()
{
  exit_message = "Application ended peacefully!";
  status_message = std::string(APPNAME) + " v." + APPVERSION;
  
  tcgetattr(STDIN_FILENO, &old_term);
  new_term = old_term;
  new_term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

  signal(SIGWINCH, handle_resize);

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  ClearScreen();
}

MtWindow::~MtWindow()
{
  _ClrScr();
  tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
  _ShowCursor();
  _MoveTo(1, 1);
  wprintf(L"%s\n", exit_message.c_str());
}

void MtWindow::SetExitMessage(std::string exit_message)
{
  this->exit_message = exit_message;
}

void MtWindow::ClearScreen()
{
  _ClrScr();
  _draw_rect
  (
    1, 1, w.ws_row, w.ws_col,
    BXT_ULT_, BXT_UDC_, BXT_URT_,
    BXT_RLM_, BXT_RLM_,
    BXT_DLT_, BXT_UDC_, BXT_DRT_
  );

 _MoveTo(w.ws_row, 2);
  wprintf(L"%s, Ready..", status_message.c_str());
  _MoveTo(1, 1);
  wprintf(L"\n");
}

void MtWindow::Run()
{
  fd_set readfds;
    
  while (true)
  {
    ClearScreen();
    _MoveTo(2, 2);
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);

    if (result < 0)
    {
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    }

    else if (result > 0)
    {
      if (FD_ISSET(STDIN_FILENO, &readfds))
      {
        char c;

        if (read(STDIN_FILENO, &c, 1) > 0)
        {
          if (c == 'q') { break; }
          status_message = "Pressed: ";
          status_message.push_back(c);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void MtWindow::_ClrScr()
{
  wprintf(L"\033c");
  _HideCursor();
}

void MtWindow::_SavePosition()
{
  wprintf(L"\x1b%d", 7);
}

void MtWindow::_RestorePosition()
{
  wprintf(L"\x1b%d", 8);
}

void MtWindow::_SetColor(ColorCode cc)
{
  wprintf(L"\x1b[%dm", cc);
}

void MtWindow::_MoveUp(int count)
{
  wprintf(L"\x1b[%dA", count);
}

void MtWindow::_MoveDown(int count)
{
  wprintf(L"\x1b[%dB", count);
}

void MtWindow::_MoveRight(int count)
{
  wprintf(L"\x1b[%dC", count);
}

void MtWindow::_MoveLeft(int count)
{
  wprintf(L"\x1b[%dD", count);
}

void MtWindow::_MoveTo(int r, int c)
{
  wprintf(L"\x1b[%d;%df", r, c);
}

void MtWindow::_HideCursor()
{
  wprintf(L"\033[?25l");
}

void MtWindow::_ShowCursor()
{
  wprintf(L"\033[?25h");
}

void MtWindow::_draw_rect
(
  uint16_t start_row,
  uint16_t start_col,
  uint16_t nrows,
  uint16_t ncols,
  wchar_t  top_left,
  wchar_t  top_center,
  wchar_t  top_right,
  wchar_t  mid_left,
  wchar_t  mid_right,
  wchar_t  bot_left,
  wchar_t  bot_center,
  wchar_t  bot_right
)
{
  _MoveTo(start_row, start_col);
  wprintf(L"%lc", top_left);
  
  for(uint16_t i = 1; i < ncols - 1; i++) { wprintf(L"%lc", top_center); }
  
  wprintf(L"%lc", top_right);
  
  for(uint16_t i = 2; i < nrows - 1; i++)
  {
    _MoveTo(i, 1);
    wprintf(L"%lc", mid_left);
    _MoveTo(i, ncols);
    wprintf(L"%lc", mid_right);
  }
  
  _MoveTo(nrows - 1, 1);
  wprintf(L"%lc", bot_left);
  
  for(uint16_t i = 1; i < ncols - 1; i++) { wprintf(L"%lc", bot_center); }
  
  wprintf(L"%lc", bot_right);

}

