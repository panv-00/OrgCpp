/*
 * File:   MtWindow.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "MtWindow.h"

void handle_resize(int sig)
{
  // nothing here for now
}

MtWindow::MtWindow() :
  exit_app{false},
  app_status{AS_OK},
  is_getting_input{false},
  cur_menu{0}
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
  sleep(1);
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
      1, 1,
      w.ws_row, w.ws_col,
      BXT_ULT_, BXT_UDC_, BXT_URT_,
      BXT_RLM_, BXT_RLM_,
      BXT_DLT_, BXT_UDC_, BXT_DRT_
  );

  _MoveTo(w.ws_row, w.ws_col - 1);

  switch (app_status)
  {
    case AS_ER:
    {
      _SetColor(CLR_RED_FG);
      wprintf(L"%lc", ICO_BAD_);
      _SetColor(CLR_DEFAULT);

    } break;

    case AS_OK:
    {
      _SetColor(CLR_GREEN_FG);
      wprintf(L"%lc", ICO_OKE_);
      _SetColor(CLR_DEFAULT);

    } break;

    case AS_WT:
    {
      _SetColor(CLR_YELLOW_FG);
      wprintf(L"%lc", ICO_HGL_);
      _SetColor(CLR_DEFAULT);

    } break;
  }

  _MoveTo(w.ws_row, 1);
  wprintf(L"%s", status_message.c_str());
  _MoveTo(1, 1);
  wprintf(L"\n");
}

void MtWindow::SetMainMenu(MtMenu *menu)
{
  main_menu = menu;
}

void MtWindow::Run()
{
  char c = 0;
  fd_set readfds;

  while (!exit_app)
  {
    status_message = "";

    for (size_t i = 0; i < main_menu->GetSize(); i++)
    {
      if (main_menu->GetOption(i).id == cur_menu)
      {
        status_message += "  ";
        status_message.push_back(main_menu->GetOption(i).accel_key);
        status_message += " -> ";
        status_message += main_menu->GetOption(i).name;
      }
    }

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    ClearScreen();

    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);

    if (result < 0)
    {
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    }

    else if (result > 0)
    {
      if (FD_ISSET(STDIN_FILENO, &readfds))
      {
        if (read(STDIN_FILENO, &c, 1) > 0)
        {
          for (size_t i = 0; i < main_menu->GetSize(); i++)
          {
           if (main_menu->GetOption(i).id == cur_menu)
            {
             if (c == main_menu->GetOption(i).accel_key)
              {
                cur_menu = main_menu->GetOption(i).next_menu_id;
                
                if (main_menu->GetOption(i).f)
                {
                  main_menu->GetOption(i).f(this);
                }
              }
            }
          }
        }
      }
    }
  }
}

void MtWindow::CallOption_Exit()
{
  exit_app = true;
}

void MtWindow::CallOption_Run()
{
  app_status = AS_WT;
  // other stuff
  app_status = AS_OK;
}

std::string MtWindow::GetInputBoxResult(MtInputBox box)
{
  app_status = AS_WT;
  
  char c = 0;
  uint16_t cursor_position = 0;
  std::string string_output = "";

  uint16_t height = 6;
  uint16_t width = box.max_length + 4;

  if (box.prompt.length() > box.max_length)
  {
    width = box.prompt.length() + 4;
  }

  if (width > (w.ws_col - 2))
  {
    width = w.ws_col - 2;
  }

  status_message = (" Getting: " + box.prompt).substr(0, width - 2);
  ClearScreen();
  
  is_getting_input = true;
  
  _draw_rect
  (
      (w.ws_row - height) / 2, (w.ws_col - width) / 2 + 1,
      height + 1, width,
      BOX_ULT_, BOX_UDC_, BOX_URT_,
      BOX_RLM_, BOX_RLM_,
      BOX_DLT_, BOX_UDC_, BOX_DRT_
  );
  
  _MoveTo(((w.ws_row - height) / 2) + 1, ((w.ws_col - width) / 2) + 2);
  _SetColor(CLR_GREEN_BG);
  _SetColor(CLR_BLACK_FG);
  wprintf(L" %s", box.prompt.substr(0, width - 4).c_str());

  for (uint16_t i = box.prompt.substr(0, width - 4).length(); i < width - 3; i++)
  {
    wprintf(L" ");
  }

  _SetColor(CLR_DEFAULT);
  _MoveTo(((w.ws_row - height) / 2) + 3, ((w.ws_col - width) / 2) + 3);
  _ShowCursor();

  while (is_getting_input)
  {
    c = getwchar();

    if (c == '\n')
    {
      is_getting_input = false;
    }

    else if (c == BACKSPACE)
    {
      if (cursor_position > 0)
      {
        cursor_position--;
        string_output.erase(cursor_position, 1);
      }
    }

    else if (c == ESCAPE_CHAR)
    {
      c = getwchar();

      if (c == ARROW_CHAR)
      {
        c = getwchar();

        if (c == ARROW_RT)
        {
          if (cursor_position < string_output.length())
          {
            cursor_position++;
          }
        }

        if (c == ARROW_LT)
        {
          if (cursor_position > 0)
          {
            cursor_position--;
          }
        }
      }
    }

    else if (c >= ' ' && c <= '~')
    {
      if (string_output.length() < box.max_length)
      {
        string_output.insert(cursor_position, 1, c);
        cursor_position++;
      }
    }

    _MoveTo(((w.ws_row - height) / 2) + 3, ((w.ws_col - width) / 2) + 3);

    for (uint16_t i = 0; i < width - 4; i++)
    {
      wprintf(L" ");
    }

    size_t start_at = 0;
    if (string_output.length() + 4 > width)
    {
      start_at = string_output.length() + 4 - width;
    }
    
    _MoveTo(((w.ws_row - height) / 2) + 3, ((w.ws_col - width) / 2) + 3);
    wprintf(L"%s", string_output.substr(start_at, string_output.length()).c_str());
    _MoveTo(((w.ws_row - height) / 2) + 3, ((w.ws_col - width) / 2) + 3);

    if (cursor_position > start_at)
    {
      _MoveRight(cursor_position - start_at);
    }
  }

  app_status = AS_OK;
  return string_output;
}

////////////////////////////////////////////////////////////////////////////////

void MtWindow::_ClrScr()
{
  wprintf(L"\033c");
  _HideCursor();
}
void MtWindow::_SavePosition()         { wprintf(L"\x1b%d", 7);         }
void MtWindow::_RestorePosition()      { wprintf(L"\x1b%d", 8);         }
void MtWindow::_SetColor(ColorCode cc) { wprintf(L"\x1b[%dm", cc);      }
void MtWindow::_MoveUp(int count)      { wprintf(L"\x1b[%dA", count);   }
void MtWindow::_MoveDown(int count)    { wprintf(L"\x1b[%dB", count);   }
void MtWindow::_MoveRight(int count)   { wprintf(L"\x1b[%dC", count);   }
void MtWindow::_MoveLeft(int count)    { wprintf(L"\x1b[%dD", count);   }
void MtWindow::_MoveTo(int r, int c)   { wprintf(L"\x1b[%d;%df", r, c); }
void MtWindow::_HideCursor()           { wprintf(L"\033[?25l");         }
void MtWindow::_ShowCursor()           { wprintf(L"\033[?25h");         }

void MtWindow::_draw_rect
(
    uint16_t start_row, uint16_t start_col,
    uint16_t nrows, uint16_t ncols,
    wchar_t top_left, wchar_t top_center, wchar_t top_right,
    wchar_t mid_left, wchar_t mid_right,
    wchar_t bot_left, wchar_t bot_center, wchar_t bot_right
)
{
  _MoveTo(start_row, start_col);
  wprintf(L"%lc", top_left);

  for (uint16_t i = 1; i < ncols - 1; i++)
  {
    wprintf(L"%lc", top_center);
  }

  wprintf(L"%lc", top_right);

  for (uint16_t i = start_row + 1; i < start_row + nrows - 2; i++)
  {
    _MoveTo(i, start_col);
    wprintf(L"%lc", mid_left);
    _MoveTo(i, start_col + ncols - 1);
    wprintf(L"%lc", mid_right);
  }

  _MoveTo(start_row + nrows - 2, start_col);
  wprintf(L"%lc", bot_left);

  for (uint16_t i = 1; i < ncols - 1; i++)
  {
    wprintf(L"%lc", bot_center);
  }

  wprintf(L"%lc", bot_right);
}
