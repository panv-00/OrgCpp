/**
 * File:   UI.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-24
 */

#include "UI.h"
#include "Parser.h"

void HandleResize(int sig)
{
  // nothing to do here for now.
}

UI::UI() :
  exit_app{false},
  sc_start_at{0},
  sc_num_of_lines{0},
  num_of_pages{0},
  sc_page{0},
  min_h{25},
  min_w{80},
  cur{0},
  start_at{0}
{
  exit_message = "Application ended in (one) peace.";
  status_line = "Ready..";

  db = new DBase(".orgcpp");
  if (db->Status() == DB_ERR)
  {
    exit_message = "Could not initiate database";
    exit_app = true;
  }

  db->LoadData();
  if (db->Status() == DB_ERR)
  {
    status_line = "Could not load database";
  }

  input_line = "";
  cur_group.id = 0;
  cur_group.name = "";

  _SetupWelcomePage();

  tcgetattr(STDIN_FILENO, &o_term);
  n_term = o_term;
  n_term.c_lflag &= ~ICANON;
  n_term.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &n_term);

  signal(SIGWINCH, HandleResize);
  _RefreshScreen();
}

UI::~UI()
{
  delete db;
  _ClrScr();
  tcsetattr(STDIN_FILENO, TCSANOW, &o_term);
  _ShowCursor();
  _MoveTo(1, 1);
  wprintf(L"%s\n", exit_message.c_str());
}

void UI::Run()
{
  Parser parser;
  char c;

  while (!exit_app)
  {
    c = _Getch();

    switch(c)
    {
      case '\n':
      {
        CleanString(input_line);
        cur = input_line.length();
  
        if (input_line.length() == 0)
        {
          _RefreshScreen();
          break;
        }
  
        parser.Parse(input_line);
        input_line = "";
        cur = 0;
        start_at = 0;
  
        if (parser.Status() != TKN_OK)
        {
          status_line = std::string("Error: ") + PERR(parser.Status());
          _RefreshScreen();
          break;
        }
  
        // QUIT APP ////////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_QUIT)
        {
          exit_app = true;
        }
  
        // MAIN SCREEN /////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_MAIN)
        {
          status_line = "Ready..";
          sc_page = 0;
          _SetupWelcomePage();
        }
  
        // LIST GROUPS /////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_LSG)
        {
          sc_page = 0;
          status_line = "Listing Groups";
          _ListGroups();
        }
  
        // ADD GROUP ///////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_ADDG)
        {
          status_line = std::string("Adding Group ") + parser.Param(0);
          db->AddGroup(parser.Param(0));
  
          if (db->Status() != DB_ERR)
          {
            status_line += ": Success!";
            db->SaveData();
            sc_page = 0;
            _SetupWelcomePage();
          }
  
          else
          {
            status_line += ": DB Error / Failed!";
          }
        }
  
        // RENAME GROUP ////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_RENG)
        {
          status_line = std::string("Modifying Group [index = ") +
                                     parser.Param(0) + "] -> "   +
                                     parser.Param(1);
  
          try
          {
            size_t num = std::stoul(parser.Param(0));
            db->EditGroup(num, parser.Param(1));
  
            if (db->Status() == DB_OK)
            {
              status_line += ": Success!";
              db->SaveData();
              sc_page = 0;
              _SetupWelcomePage();
            }
  
            else
            {
              status_line += ": DB Error / Failed!";
            }
          }
  
          catch (const std::invalid_argument &e)
          {
            status_line += std::string(": Error / Invalid index!");
          }
        }
  
        // DELETE GROUP ////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_DELG)
        {
          status_line = std::string("Deleting Group [index = ") +
                                    parser.Param(0) + "]";
  
          try
          {
            size_t num = std::stoul(parser.Param(0));
            db->DeleteGroup(num);
  
            if (db->Status() == DB_OK)
            {
              status_line += ": Success!";
              db->SaveData();
              sc_page = 0;
              _SetupWelcomePage();
            }
  
            else
            {
              status_line += ": DB Error / Failed!";
            }
          }
  
          catch (const std::invalid_argument &e)
          {
            status_line += std::string(": Error / Invalid index!");
          }
        }
  
        // SWITCH TO GROUP /////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_SWG)
        {
          status_line = std::string("Switching to Group [index = ") +
                                    parser.Param(0) + "]";
  
          try
          {
            size_t num = std::stoul(parser.Param(0));
            cur_group = db->GetGroup(num);
  
            if (db->Status() == DB_OK)
            {
              status_line += std::string(": ") + cur_group.name;
              sc_page = 0;
              _ListTickets(num);
            }
  
            else
            {
              status_line += ": DB Error / Failed!";
            }
          }
  
          catch (const std::invalid_argument &e)
          {
            status_line += std::string(": Error / Invalid index!");
          }
        }
  
        // EXIT GROUP //////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_EXG)
        {
          status_line = "Ready..";
          sc_page = 0;
          cur_group.id = 0;
          cur_group.name = "";
          _SetupWelcomePage();
        }
  
        // LIST TICKETS IN CURRENT GROUP ///////////////////////////////////////
        if (parser.Cmd() == CMD_LST)
        {
          if (cur_group.id == 0)
          {
            status_line = "You need to be in a group to do that.";
          }
  
          else
          {
            status_line = std::string("Group: ") + cur_group.name;
            sc_page = 0;
            _ListTickets(cur_group.id);
          }
        }
  
        // ADD TICKET TO CURRENT GROUP /////////////////////////////////////////
        if (parser.Cmd() == CMD_ADDT)
        {
          if (cur_group.id == 0)
          {
            status_line = "You need to be in a group to do that.";
          }
  
          else
          {
            status_line = std::string("Adding Ticket ") + parser.Param(0);
            db->AddTicket(cur_group.id, parser.Param(0), parser.Param(1));
  
            if (db->Status() != DB_ERR)
            {
              status_line += ": Success!";
              db->SaveData();
              sc_page = 0;
              _ListTickets(cur_group.id);
            }
  
            else
            {
              status_line += ": DB Error / Failed!";
            }
          }
        }

        // RENAME GROUP ////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_RENT)
        {
          status_line = std::string("Modifying Ticket Name [index = ") +
                                     parser.Param(0) + "] -> "   +
                                     parser.Param(1);
  
          try
          {
            size_t num = std::stoul(parser.Param(0));
            db->EditTicket(num, parser.Param(1), db->GetTicket(num).desc);
  
            if (db->Status() == DB_OK)
            {
              status_line += ": Success!";
              db->SaveData();
              sc_page = 0;
              _ListTickets(cur_group.id);
            }
  
            else
            {
              status_line += ": DB Error / Failed!";
            }
          }
  
          catch (const std::invalid_argument &e)
          {
            status_line += std::string(": Error / Invalid index!");
          }
        }

        // CHANGE TICKET DESCRIPTION ///////////////////////////////////////////
        if (parser.Cmd() == CMD_CTD)
        {
          status_line = std::string("Changing Ticket Description [index = ") +
                                     parser.Param(0) + "] -> "   +
                                     parser.Param(1);
  
          try
          {
            size_t num = std::stoul(parser.Param(0));
            db->EditTicket(num, db->GetTicket(num).name, parser.Param(1));
  
            if (db->Status() == DB_OK)
            {
              status_line += ": Success!";
              db->SaveData();
              sc_page = 0;
              _ListTickets(cur_group.id);
            }
  
            else
            {
              status_line += ": DB Error / Failed!";
            }
          }
  
          catch (const std::invalid_argument &e)
          {
            status_line += std::string(": Error / Invalid index!");
          }
        }

////////////////////////////////////////////////////////////////////////////////  
        _RefreshScreen();
      } break;
  
      case BACKSPACE:
      {
        if (cur > 0)
        {
          cur--;
          input_line.erase(cur, 1);
        }
      } break;
  
      case ESCAPE_CHAR:
      {
        c = _Getch();

        if (c == ARROW_CHAR)
        {
          c = _Getch();
  
          if (c == ARROW_UP)
          {
            sc_page = (sc_page - 1) % (num_of_pages + 1);
            _RefreshScreen();
          }
          if (c == ARROW_DN)
          {
            sc_page = (sc_page + 1) % (num_of_pages + 1);
            _RefreshScreen();
          }
          if (c == ARROW_LT)
          {
            if (cur > 0) { cur--; }
          }
          if (c == ARROW_RT)
          {
            if (cur < input_line.length()) { cur++; }
          }  
        }
      } break;
  
      default:
      {
        if (c >= ' ' && c <= '~')
        {
          input_line.insert(cur, 1, c);
          cur++;
        }
      } break;
    }

    start_at = 0;
    if (input_line.length() + 3 > w.ws_col)
    {
      start_at = input_line.length() - w.ws_col + 3;
    }   
    _EchoInputLine();
  }
}

////////////////////////////////////////////////////////////////////////////////

void UI::_SetupWelcomePage()
{
  screen_title = std::string(APPNAME) + " v." + APPVERSION;
  screen_header.clear();
  screen_content.clear();
  _AddHeaderContent("");

  _AddHeaderContent("  .g8\"\"8q.                           .g8\"\"\"bgd");
  _AddHeaderContent(".dP'    `YM.                       .dP'     `M");
  _AddHeaderContent("dM'      `MM `7Mb,od8 .P\"Ybmmm     dM'       ``7MMpdMAo.`7MMpdMAo.");
  _AddHeaderContent("MM        MM   MM' \"':MI  I8       MM           MM   `Wb  MM   `Wb");
  _AddHeaderContent("MM.      ,MP   MM     WmmmP\"       MM.          MM    M8  MM    M8");
  _AddHeaderContent("`Mb.    ,dP'   MM    8M            `Mb.     ,'  MM   ,AP  MM   ,AP");
  _AddHeaderContent("  `\"bmmd\"'   .JMML.   YMMMMMb        `\"bmmmd'   MMbmmd'   MMbmmd'");
  _AddHeaderContent("                    6'     dP                   MM        MM");
  _AddHeaderContent("                    Ybmmmd'                   .JMML.    .JMML.");

  _AddHeaderContent("");
  _AddHeaderContent("┌─────────────┬──────────────────┬────────────────────────────────┐");
  _AddHeaderContent("│ Command     │ Parameters       │ Description                    │");
  _AddHeaderContent("├─────────────┼──────────────────┼────────────────────────────────┤");
  _AddContent      ("│ quit        │                  │ quit the app                   │");
  _AddContent      ("│ main        │                  │ go to main screen              │");
  _AddContent      ("│ lsg         │                  │ list groups                    │");
  _AddContent      ("│ addg        │ name             │ add group                      │");
  _AddContent      ("│ reng        │ id 〔new name〕  │ rename group [id] new_name     │");
  _AddContent      ("│ delg        │ id               │ delete group [id]              │");
  _AddContent      ("│ swg         │ id               │ switch to group [id]           │");
  _AddContent      ("│ exg         │                  │ exit group                     │");
  _AddContent      ("│ lst         │                  │ list tickets in current group  │");
  _AddContent      ("│ addt        │ name description │ list tickets in current group  │");
  _AddContent      ("│ rent        │ id 〔new name〕  │ rename ticket [id] new_name    │");
  _AddContent      ("│ ctd         │ id 〔new desc〕  │ change ticket [id] desc        │");
  _AddContent      ("│             │                  │                                │");
  _AddContent      ("└─────────────┴──────────────────┴────────────────────────────────┘");
}

void UI::_ListGroups()
{
  screen_title = "    ID NAME";
  screen_header.clear();
  screen_content.clear();
  char buf[w.ws_col];

  for (auto &gr : db->GetGroups())
  {
    sprintf(buf, "%6ld %s", gr.id, gr.name.c_str());
    _AddContent(buf);
  }
}

void UI::_ListTickets(uint64_t group_id)
{
  screen_title = "    ID STATE   NAME                 DESCRIPTION";
  screen_header.clear();
  screen_content.clear();
  char buf[w.ws_col];

  for (auto &tkt : db->GetTickets(group_id))
  {
    sprintf(buf, "%6ld %7s %-20s %s", tkt.id, TicketStateToStr(tkt.state), tkt.name.c_str(), tkt.desc.c_str());
    _AddContent(buf);
  }
}

void UI::_AddHeaderContent(std::string content)
{
  screen_header.push_back(content);
}

void UI::_AddContent(std::string content)
{
  screen_content.push_back(content);
}

void UI::_RefreshScreen()
{
  _CheckTermSize();
  _ClrScr();
  
  // Draw Title
  _MoveTo(1, 1);
  _SetColor(CLR_BLUE_BG);
  _SetColor(CLR_BLACK_FG);

  for (uint16_t i = 0; i < w.ws_col; i++)
  {
    if (i < screen_title.length())
    {
      wprintf(L"%lc", screen_title[i]);
    }
    else
    {
      wprintf(L" ");
    }
  }

  _SetColor(CLR_DEFAULT);

  // Draw Header
  for (size_t i = 0; i < screen_header.size(); i++)
  {
    _MoveTo(i + 2, 1);
    wprintf(L"%s", screen_header[i].c_str());
  }
  
  // Draw Screen
  for (size_t i = 0; i < sc_num_of_lines; i++)
  {
    if ((sc_page * sc_num_of_lines) + i < screen_content.size())
    {
      _MoveTo(i + 2 + screen_header.size(), 1);
      wprintf(L"%s", screen_content[(sc_page * sc_num_of_lines) + i].c_str());
    }
  }

  _MoveTo(w.ws_row - 2, 1);
  _SetColor(CLR_BLUE_FG);
  
  for (uint16_t i = 0; i < w.ws_col; i++)
  {
    wprintf(L"─");
  }

  _SetColor(CLR_DEFAULT);
  _MoveTo(w.ws_row - 1, 1);

  if (num_of_pages == 0)
  {
    wprintf(L"%s", status_line.c_str());
  }

  else
  {
    wprintf(L"%s [ Page %ld of %ld ]", status_line.c_str(),
                                       sc_page + 1, num_of_pages + 1);
  }
 
  _EchoInputLine();
}

char UI::_Getch()
{
  char c = 0;
  fd_set readfds;
  struct timeval tv;
  
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  tv.tv_sec = 0;
  tv.tv_usec = 500000;

  int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

  if (result < 0)
  {
    _RefreshScreen();
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

void UI::_CheckTermSize()
{
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  
  if (w.ws_row < min_h)
  {
    exit_message = "App Cannot Continue. Term Height = " +
      Uint64_TToString(w.ws_row) + " < " + Uint64_TToString(min_h);
    exit_app = true;
  }

  if (w.ws_col < min_w)
  {
    exit_message = "App Cannot Continue. Term Width = " +
      Uint64_TToString(w.ws_col) + " < " + Uint64_TToString(min_w);
    exit_app = true;
  }

  sc_num_of_lines = w.ws_row - screen_header.size() - 4;
  num_of_pages = screen_content.size() / sc_num_of_lines;

  if (num_of_pages > 0)
  {
    sc_num_of_lines = (screen_content.size() + num_of_pages) / (num_of_pages + 1);
  }
}

void UI::_EchoInputLine()
{
  _SetColor(CLR_BLUE_FG);
  _ShowCursor();

  _MoveTo(w.ws_row, 1);
  for (size_t i = 0; i < w.ws_col; i++) { wprintf(L"%c", ' '); }
  _MoveTo(w.ws_row, 1);
  wprintf(L"%lc ", ICO_ARR_);
  for (size_t i = start_at; i < input_line.length(); i++)
  {
    wprintf(L"%c", input_line[i]);
  }
  _MoveTo(w.ws_row, 1);
  wprintf(L"%lc ", ICO_ARR_);
  _MoveRight(cur - start_at);
  _SetColor(CLR_DEFAULT);
  fflush(stdout);
}

void UI::_ClrScr()
{
  wprintf(L"\033c");
}

void UI::_HideCursor()
{
  wprintf(L"\033[?25l");
}

void UI::_ShowCursor()
{
  wprintf(L"\033[?25h");
}

void UI::_MoveTo(uint16_t r, uint16_t c)
{
  wprintf(L"\x1b[%d;%df", r, c);
}

void UI::_MoveRight(uint16_t count)
{
  if (count > 0)
  {
    wprintf(L"\x1b[%dC", count);
  }
}

void UI::_SetColor(ColorCode cc)
{
  wprintf(L"\x1b[%dm", cc);
}
