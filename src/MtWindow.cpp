/**
 * File:   MtWindow.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "MtWindow.h"
#include "MtInclude.h"
#include "MtUtils.h"

void handle_resize(int sig)
{
  // nothing here for now
}

MtWindow::MtWindow() :
  exit_app{false},
  app_status{AS_OK},
  is_getting_input{false},
  term_min_height{25},
  term_min_width{80},
  cur_menu{0}
{
  exit_message = "Application ended peacefully!";
  status_message = std::string(APPNAME) + " v." + APPVERSION;
  screen_content.clear();

  tcgetattr(STDIN_FILENO, &old_term);
  new_term = old_term;
  new_term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

  signal(SIGWINCH, handle_resize);

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

void MtWindow::ClearScreenContent()
{
  this->screen_content.clear();
}

void MtWindow::AppendScreenContent(std::string content)
{
  this->screen_content.push_back(content);
}

void MtWindow::ClearScreen()
{
  _CheckTermSize();
  _ClrScr();
  _draw_rect
  (
      1, 1,
      w.ws_row, w.ws_col,
      BXT_ULT_, BXT_UDC_, BXT_URT_,
      BXT_RLM_, BXT_RLM_,
      BXT_DLT_, BXT_UDC_, BXT_DRT_
  );

  // Print screen_content if not empty
  for (size_t i = 0; i < screen_content.size(); ++i)
  {
    _MoveTo(i + 3, 3);
    wprintf(L"%s", screen_content[i].c_str());
  }

  // Print status icon
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

  // Loop till exit
  while (!exit_app)
  {
    // Put Menu in status_message
    status_message = "";

    for (size_t i = 0; i < main_menu->GetSize(); i++)
    {
      if (main_menu->GetOption(i).id == cur_menu)
      {
        status_message += "(";
        status_message.push_back(main_menu->GetOption(i).accel_key);
        status_message += "->";
        status_message += main_menu->GetOption(i).name;
        status_message += ") ";
      }
    }

    ClearScreen();
    c = _Getch();

    // Save next menu, and execute current menu method
    for (size_t i = 0; i < main_menu->GetSize(); i++)
    {
      if (main_menu->GetOption(i).id == cur_menu)
      {
        if (c == main_menu->GetOption(i).accel_key)
        {
          cur_menu = main_menu->GetOption(i).next_menu_id;

          if (main_menu->GetOption(i).f)
          {
            main_menu->GetOption(i).f(parent);
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
  ClearScreenContent();
  ClearScreen();
  app_status = AS_OK;
}

bool MtWindow::GetInputBoxResult
(
  MtInputBox box,
  InputBoxType type,
  std::vector<MtPair> select_options,
  std::string &result
)
{
  app_status = AS_WT;
  is_getting_input = true;

  char c = 0;
  uint16_t cursor_position = 0;
  result = "";

  status_message = "Input Text";

  if (type == INPUT_DATE)
  {
    status_message += ": YYYYMMDD";
  }

  // Setup preliminary input box dimensions
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

  if (type == INPUT_SLCT)
  {
    status_message = "Select / New";
    height = 15;
  }

  ClearScreen();

  _draw_rect
  (
    (w.ws_row - height) / 2, (w.ws_col - width) / 2 + 1,
    height + 1, width,
    BOX_ULT_, BOX_UDC_, BOX_URT_,
    BOX_RLM_, BOX_RLM_,
    BOX_DLT_, BOX_UDC_, BOX_DRT_
  );

  // Box Title
  _MoveTo(((w.ws_row - height) / 2) + 1, ((w.ws_col - width) / 2) + 2);
  _SetColor(CLR_GREEN_BG);
  _SetColor(CLR_BLACK_FG);
  wprintf(L" %s", box.prompt.substr(0, width - 4).c_str());

  for (uint16_t i = box.prompt.substr(0, width - 4).length(); i < width - 3; i++)
  {
    wprintf(L" ");
  }

  _SetColor(CLR_DEFAULT);

  // Setup Cursor at input location
  _MoveTo(((w.ws_row - height) / 2) + 3, ((w.ws_col - width) / 2) + 3);
  _ShowCursor();

  while (is_getting_input)
  {
    c = getwchar();

    // Exit on Enter
    if (c == '\n') { is_getting_input = false; }

    // Delete char on backspace
    else if (c == BACKSPACE)
    {
      if (cursor_position > 0)
      {
        cursor_position--;
        result.erase(cursor_position, 1);
      }
    }

    // Handle arrow keys
    else if (c == ESCAPE_CHAR)
    {
      c = getwchar();

      if (c == ARROW_CHAR)
      {
        c = getwchar();

        if (c == ARROW_RT)
        {
          if (cursor_position < result.length()) { cursor_position++; }
        }

        if (c == ARROW_LT)
        {
          if (cursor_position > 0) { cursor_position--; }
        }
      }

      // Cancel on escape key (twice)
      else if (c == ESCAPE_CHAR)
      {
        app_status = AS_OK;
        result = "";
        return false;
      }
    }

    // Add chars to result
    else if (
      (type == INPUT_TEXT && c >= ' ' && c <= '~') ||
      (type == INPUT_SLCT && c >= ' ' && c <= '~') ||
      (type == INPUT_DATE && c >= '0' && c <= '9')
    )
    {
      if (result.length() < box.max_length)
      {
        result.insert(cursor_position, 1, c);
        cursor_position++;
      }
    }

    // Display result
    _MoveTo(((w.ws_row - height) / 2) + 3, ((w.ws_col - width) / 2) + 3);

    for (uint16_t i = 0; i < width - 4; i++)
    {
      wprintf(L" ");
    }

    size_t start_at = 0;
    if (result.length() + 4 > width)
    {
      start_at = result.length() + 4 - width;
    }

    _MoveTo(((w.ws_row - height) / 2) + 3, ((w.ws_col - width) / 2) + 3);
    wprintf(L"%s", result.substr(start_at, result.length()).c_str());
    _MoveTo(((w.ws_row - height) / 2) + 3, ((w.ws_col - width) / 2) + 3);

    if (cursor_position > start_at)
    {
      _MoveRight(cursor_position - start_at);
    }
  }

  app_status = AS_OK;
  return true;
}

////////////////////////////////////////////////////////////////////////////////

char MtWindow::_Getch()
{
  char c = 0;
  fd_set readfds;
  
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);

  if (result < 0)
  {
    ClearScreen();
  }

  else if (result > 0)
  {
    if (FD_ISSET(STDIN_FILENO, &readfds))
    {
      if (read(STDIN_FILENO, &c, 1) > 0)
      {
        return c;
      }
    }
  }
  
  return c;
}

void MtWindow::_CheckTermSize()
{
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  
  if (w.ws_col < term_min_width)
  {
    exit_message = "App Cannot Continue. Term Width = " +
      Uint64_TToString(w.ws_col) + " < " + Uint64_TToString(term_min_width);
    exit_app = true;
  }

  if (w.ws_row < term_min_height)
  {
    exit_message = "App Cannot Continue. Term Height = " +
      Uint64_TToString(w.ws_row) + " < " + Uint64_TToString(term_min_height);
    exit_app = true;
  }
}
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
