/**
 * File:   UI.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-24
 */

#include "UI.h"
#include "Parser.h"

std::vector<std::string> wrap_string(const std::string &str, uint16_t width)
{
  std::vector<std::string> wrapped_string;
  std::string current_line;

  for (size_t i = 0; i < str.length(); i++) {
    char c = str[i];
    if (c == ' ')
    {
      // Check if adding the next word to the current line will make it too long
      if (current_line.length() + 1 + (str.find_first_not_of(' ', i + 1) - i) > width)
      {
        wrapped_string.push_back(current_line);
        current_line.clear();
      }

      current_line += ' ';
    }
    
    else
    {
      current_line += c;
    }

    // If the current line is too long and the last character was not a space,
    // split the current word onto the next line
    if (current_line.length() == width && str[i + 1] != ' ')
    {
      int j = current_line.length() - 1;

      while (j > 0 && current_line[j] != ' ')
      {
        j--;
      }

      wrapped_string.push_back(current_line.substr(0, j));
      current_line = current_line.substr(j + 1);
    }
  }

  if (!current_line.empty())
  {
    wrapped_string.push_back(current_line);
  }

  return wrapped_string;
}

void HandleResize(int sig)
{
  // nothing to do here for now.
}

UI::UI() :
  screen_status{SCR_MAIN},
  app_status{APP_WT},
  sc_start_at{0},
  sc_num_of_lines{0},
  num_of_pages{0},
  sc_page{0},
  min_h{25},
  min_w{80},
  cur{0},
  start_at{0}
{
  app_status = APP_OK;
  exit_message = "Application ended in (one) peace.";
  status_line = "quit | main | lsg | addg | swg";

  db = new DBase(".orgcpp");
  if (db->Status() == DB_ERR)
  {
    exit_message = "Could not initiate database";
    screen_status = SCR_QUIT;
    app_status = APP_ER;
  }

  db->LoadData();
  if (db->Status() == DB_ERR)
  {
    status_line = "Could not load database";
    app_status = APP_WR;
  }

  input_line = "";
  cur_group.id = 0;
  cur_group.name = "";

  cur_ticket.id = 0;
  cur_ticket.group_id = 0;
  cur_ticket.state = TKT_HID;
  cur_ticket.creation_date = 0;
  cur_ticket.name = "";
  cur_ticket.desc = "";
  std::vector<Attachment> empty_attachment;
  std::vector<TicketTask> empty_task;
  cur_ticket.attachments = empty_attachment;
  cur_ticket.tasks = empty_task;


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

  while (screen_status != SCR_QUIT)
  {
    c = _Getch();

    switch(c)
    {
      case '\n':
      {
        CleanString(input_line);
        cur = input_line.length();
        app_status = APP_OK;
  
        if (input_line.length() == 0)
        {
          break;
        }
  
        parser.Parse(input_line);
        input_line = "";
        cur = 0;
        start_at = 0;
  
        if (parser.Status() != TKN_OK)
        {
          status_line = std::string("Error: ") + PERR(parser.Status());
          app_status = APP_ER;
          _RefreshScreen();
          break;
        }
  
        // QUIT APP ////////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_QUIT)
        {
          screen_status = SCR_QUIT;
        }
  
        // MAIN SCREEN /////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_MAIN)
        {
          sc_page = 0;
          screen_status = SCR_MAIN;
        }
  
        // LIST GROUPS /////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_LSG)
        {
          sc_page = 0;
          screen_status = SCR_LSG;
        }
  
        // ADD GROUP ///////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_ADDG)
        {
          db->AddGroup(parser.Param(0));
  
          if (db->Status() != DB_ERR)
          {
            db->SaveData();
            screen_status = SCR_LSG;
          }
  
          else
          {
            app_status = APP_ER;
            status_line = "Adding Group " + parser.Param(0) + ": DB Error / Failed!";
          }
        }

        // SWITCH TO GROUP /////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_SWG)
        {
          try
          {
            size_t num = std::stoul(parser.Param(0));
            cur_group = db->GetGroup(num);
  
            if (db->Status() == DB_OK)
            {
              sc_page = 0;
              screen_status = SCR_LST;
            }
  
            else
            {
              app_status = APP_ER;
              status_line = "Switching to Group [index = " + parser.Param(0) +
                            "] : DB Error / Failed!";
            }
          }
  
          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Switching to Group [index = " + parser.Param(0) +
                          "] : Error / Invalid index!";
          }
        }
 
        // RENAME GROUP ////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_RENG)
        {
          if (cur_group.id == 0)
          {
            status_line = "You must be in a group to do that!";
            app_status = APP_ER;
            _RefreshScreen();
            break;
          }

 
          db->EditGroup(cur_group.id, parser.Param(0));
 
          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_group = db->GetGroup(cur_group.id);
          }
  
          else
          {
            app_status = APP_ER;
            status_line = std::string("Modifying Group [index = ") +
                                     Uint64_TToString(cur_group.id) + "] -> " +
                                     parser.Param(0) + ": DB Error / Failed!";
          }
        }
  
        // DELETE GROUP ////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_DELG)
        {
          if (cur_group.id == 0)
          {
            status_line = "You need to be in a group to do that!";
            app_status = APP_ER;
            _RefreshScreen();
            break;
          }
 
          db->DeleteGroup(cur_group.id);

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_group.id = 0;
            cur_group.name = "";
            screen_status = SCR_LSG;
          }

          else
          {
            app_status = APP_ER;
            status_line = "Deleting Group [index = " +
                          Uint64_TToString(cur_group.id) +
                          "] : DB Error / Failed!";
          }
        }

        // LIST TICKETS IN CURRENT GROUP ///////////////////////////////////////
        if (parser.Cmd() == CMD_LST)
        {
          if (cur_group.id == 0)
          {
            status_line = "You need to be in a group to do that!";
            app_status = APP_ER;
            _RefreshScreen();
            break;
          }
  
          sc_page = 0;
          screen_status = SCR_LST;
        }

        // EXIT GROUP //////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_EXG)
        {
          sc_page = 0;
          cur_group.id = 0;
          cur_group.name = "";
          cur_ticket.id = 0;
          cur_ticket.group_id = 0;
          cur_ticket.state = TKT_HID;
          cur_ticket.creation_date = 0;
          cur_ticket.name = "";
          cur_ticket.desc = "";
          std::vector<Attachment> empty_attachment;
          std::vector<TicketTask> empty_task;
          cur_ticket.attachments = empty_attachment;
          cur_ticket.tasks = empty_task;
          screen_status = SCR_LSG;
        }
  
        // ADD TICKET TO CURRENT GROUP /////////////////////////////////////////
        if (parser.Cmd() == CMD_ADDT)
        {
          if (cur_group.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a group to do that!";
            _RefreshScreen();
            break;
          }
  
          db->AddTicket(cur_group.id, parser.Param(0), parser.Param(1));

          if (db->Status() != DB_ERR)
          {
            db->SaveData();
            sc_page = 0;
            screen_status = SCR_LST;
          }

          else
          {
            app_status = APP_ER;
            status_line = "Adding Ticket " + parser.Param(0) +
                          ": DB Error / Failed!";
          }
        }

        // OPEN TICKET /////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_OPT)
        {
          try
          {
            size_t tnum = std::stoul(parser.Param(0));
            
            cur_ticket = db->GetTicket(tnum);

            if (db->Status() == DB_OK && cur_ticket.id > 0)
            {
              cur_group = db->GetGroup(cur_ticket.group_id);
              screen_status = SCR_TKV;
            }

            else
            {
              app_status = APP_ER;
              status_line = "Opening Ticket [index = " + parser.Param(0) +
                            "] : DB Error / Failed!";
            }
          }
  
          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Opening Ticket [index = " + parser.Param(0) +
                          "] : Error / Invalid index!";
          }
        }

        // RENAME TICKET ///////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_RENT)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }
 
          db->EditTicket(cur_ticket.id, parser.Param(0), cur_ticket.desc);

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket = db->GetTicket(cur_ticket.id);
          }

          else
          {
            app_status = APP_ER;
            status_line = "Modifying Ticket Name [index = " +
                          Uint64_TToString(cur_ticket.id) + "] -> " +
                          parser.Param(0) + ": DB Error / Failed!";
          }
        }

        // CHANGE TICKET DESCRIPTION ///////////////////////////////////////////
        if (parser.Cmd() == CMD_CTD)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          db->EditTicket(cur_ticket.id, cur_ticket.name, parser.Param(0));

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket = db->GetTicket(cur_ticket.id);
          }

          else
          {
            app_status = APP_ER;
            status_line = "Changing Ticket Description [index = " +
                          Uint64_TToString(cur_ticket.id) + "] -> " +
                          parser.Param(0) + ": DB Error / Failed!";
          }
        }

       // DELETE TICKET ///////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_DELT)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          db->DeleteTicket(cur_ticket.id);

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket.id = 0;
            cur_ticket.group_id = 0;
            cur_ticket.state = TKT_HID;
            cur_ticket.creation_date = 0;
            cur_ticket.name = "";
            cur_ticket.desc = "";
            std::vector<Attachment> empty_attachment;
            std::vector<TicketTask> empty_task;
            cur_ticket.attachments = empty_attachment;
            cur_ticket.tasks = empty_task;
            sc_page = 0;
            screen_status = SCR_LST;
          }

          else
          {
            app_status = APP_ER;
            status_line = "Deleting Ticket [index = " +
                          Uint64_TToString(cur_ticket.id) + "] : DB Error / Failed!";
          }
        }

        // MOVE TICKET ID TO GROUP ID //////////////////////////////////////////
        if (parser.Cmd() == CMD_MOVT)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t gnum = std::stoul(parser.Param(0));
            
            db->GetGroup(gnum);

            if (db->Status() == DB_OK)
            {
              db->MoveTicketToGroup(cur_ticket.id, gnum);

              if (db->Status() == DB_OK)
              {
                db->SaveData();
                cur_group = db->GetGroup(gnum);
                cur_ticket = db->GetTicket(cur_ticket.id);
              }

              else
              {
                app_status = APP_ER;
                status_line = "Moving Ticket [index = " +
                              Uint64_TToString(cur_ticket.id) +
                              "] -> To Group [index = " + parser.Param(0) +
                              "] : DB Error / Failed!";
              }
            }

            else
            {
              app_status = APP_ER;
              status_line = "Moving Ticket [index = " +
                            Uint64_TToString(cur_ticket.id) +
                            "] -> To Group [index = " + parser.Param(0) +
                            "] : DB Error / No group found at index " +
                            SizeTToString(gnum);
            }
          }
  
          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Moving Ticket [index = " +
                          Uint64_TToString(cur_ticket.id) +
                          "] -> To Group [index = " + parser.Param(0) +
                          "] : Error / Invalid index!";
          }
        }

        // MARK TICKET AS ONGOING //////////////////////////////////////////////
        if (parser.Cmd() == CMD_MTO)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          db->ChangeTicketState(cur_ticket.id, TKT_ONG);

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket = db->GetTicket(cur_ticket.id);
          }

          else
          {
            app_status = APP_ER;
            status_line = "Marking Ticket [index = " +
                          Uint64_TToString(cur_ticket.id) +
                          "] as ONGOING: DB Error / Failed!";
          }
        }

        // MARK TICKET AS DONE /////////////////////////////////////////////////
        if (parser.Cmd() == CMD_MTD)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          db->ChangeTicketState(cur_ticket.id, TKT_DON);

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket = db->GetTicket(cur_ticket.id);
          }

          else
          {
            app_status = APP_ER;
            status_line = "Marking Ticket [index = " +
                          Uint64_TToString(cur_ticket.id) +
                          "] as DONE: DB Error / Failed!";
          }
        }

        // MARK TICKET AS HIDDEN ///////////////////////////////////////////////
        if (parser.Cmd() == CMD_MTH)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          db->ChangeTicketState(cur_ticket.id, TKT_HID);

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket = db->GetTicket(cur_ticket.id);
          }

          else
          {
            app_status = APP_ER;
            status_line = "Marking Ticket [index = " +
                          Uint64_TToString(cur_ticket.id) +
                          "] as HIDDEN: DB Error / Failed!";
          }
        }
 
        // ADD TICKET ATTACHMENT ///////////////////////////////////////////////
        if (parser.Cmd() == CMD_ATA)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          db->AddTicketAttachment(cur_ticket.id, parser.Param(0));

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket = db->GetTicket(cur_ticket.id);
          }

          else
          {
            app_status = APP_ER;
            status_line = "Adding Attachment to Ticket [index = " +
                          Uint64_TToString(cur_ticket.id) + "] >> " +
                          parser.Param(0) + " : Failed!";
          }
        }

        // OPEN TICKET ATTACHMENT //////////////////////////////////////////////
        if (parser.Cmd() == CMD_OTA)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t anum = std::stoul(parser.Param(0));
 
            db->OpenTicketAttachment(cur_ticket.id, anum);

            if (db->Status() != DB_OK)
            {
              app_status = APP_ER;
              status_line = "Opening Attachment [index = " + parser.Param(0) +
                            "] : Failed!";
            }
          }

          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Opening Attachment [index = " + parser.Param(0) +
                          "] : Error / Invalid index!";
          }
        }

        // NEW TICKET ATTACHMENT ///////////////////////////////////////////////
        if (parser.Cmd() == CMD_NTA)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          db->NewTextTicketAttachment(cur_ticket.id, parser.Param(0));

          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket = db->GetTicket(cur_ticket.id);
          }

          else
          {
            app_status = APP_ER;
            status_line = "New Text Attachment to Ticket [index = " +
                          Uint64_TToString(cur_ticket.id) + "] >> " +
                          parser.Param(0) + " : Failed!";
          }
        }

        // DELETE TICKET ATTACHMENT ////////////////////////////////////////////
        if (parser.Cmd() == CMD_DTA)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t anum = std::stoul(parser.Param(0));
 
            db->DeleteTicketAttachment(cur_ticket.id, anum);

            if (db->Status() == DB_OK)
            {
              cur_ticket = db->GetTicket(cur_ticket.id);
            }

            else
            {
              app_status = APP_ER;
              status_line = "Deleting Attachment [index = " +
                            parser.Param(0) + "] : Failed!";
            }
          }

          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Deleting Attachment [index = " +
                          parser.Param(0) + "] : Error / Invalid index!";
          }
        }

        // ADD TICKET TASK /////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_ATT)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          std::string eta_str = parser.Param(0);
          uint64_t eta = StringdateToTimestamp(eta_str);

          if (eta == (uint64_t)-1)
          {
              app_status = APP_ER;
              status_line = std::string("Adding Task to Ticket [index = ") +
                                     Uint64_TToString(cur_ticket.id) + "] >> " +
                                     parser.Param(0) + ", " +
                                     parser.Param(1) + ", " +
                                     parser.Param(2) + " : wrong date!";
              _RefreshScreen();
              break;
          }

          db->AddTicketTask
          (
            cur_ticket.id,
            eta,
            parser.Param(1),
            parser.Param(2)
          );
  
          if (db->Status() == DB_OK)
          {
            db->SaveData();
            cur_ticket = db->GetTicket(cur_ticket.id);
          }
  
          else
          {
            app_status = APP_ER;
            status_line = std::string("Adding Task to Ticket [index = ") +
                                     Uint64_TToString(cur_ticket.id) + "] >> " +
                                     parser.Param(0) + ", " +
                                     parser.Param(1) + ", " +
                                     parser.Param(2) + " : Failed!";
          }
        }

        // INCREMENT TICKET TASK PROGRESS //////////////////////////////////////
        if (parser.Cmd() == CMD_ITTP)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t tnum = std::stoul(parser.Param(0));
 
            db->IncTicketTaskProgress(cur_ticket.id, tnum);

            if (db->Status() == DB_OK)
            {
              cur_ticket = db->GetTicket(cur_ticket.id);
            }

            else
            {
              app_status = APP_ER;
              status_line = "Incr. Task Progress [index = " +
                            parser.Param(0) + "] : Failed!";
            }
          }

          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Incr. Task Progress [index = " +
                          parser.Param(0) + "] : Error / Invalid index!";
          }
        }

        // DECREMENT TICKET TASK PROGRESS //////////////////////////////////////
        if (parser.Cmd() == CMD_DTTP)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t tnum = std::stoul(parser.Param(0));
 
            db->DecTicketTaskProgress(cur_ticket.id, tnum);

            if (db->Status() == DB_OK)
            {
              cur_ticket = db->GetTicket(cur_ticket.id);
            }

            else
            {
              app_status = APP_ER;
              status_line = "Decr. Task Progress [index = " +
                            parser.Param(0) + "] : Failed!";
            }
          }

          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Decr. Task Progress [index = " +
                          parser.Param(0) + "] : Error / Invalid index!";
          }
        }

        // CHANGE TICKET TASK ETA //////////////////////////////////////////////
        if (parser.Cmd() == CMD_CTTE)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t tnum = std::stoul(parser.Param(0));

            std::string eta_str = parser.Param(1);
            uint64_t eta = StringdateToTimestamp(eta_str);

            if (eta == (uint64_t)-1)
            {
              app_status = APP_ER;
              status_line = "Changing Task ETA [index = " +
                            parser.Param(0) + "] -> " +
                            parser.Param(1) + " : wrong date!";
              _RefreshScreen();
              break;
            }
 
            db->EditTicketTaskETA(cur_ticket.id, tnum, eta);

            if (db->Status() == DB_OK)
            {
              cur_ticket = db->GetTicket(cur_ticket.id);
            }

            else
            {
              app_status = APP_ER;
              status_line = "Changing Task ETA [index = " +
                            parser.Param(0) + "] -> " +
                            parser.Param(1) + " : Failed!";
            }
          }

          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Changing Task ETA [index = " +
                          parser.Param(0) + "] -> " +
                          parser.Param(1) + ": Error / Invalid index!";
          }
        }

        // CHANGE TICKET TASK RESOURCE /////////////////////////////////////////
        if (parser.Cmd() == CMD_CTTR)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t tnum = std::stoul(parser.Param(0));

            db->EditTicketTaskResource(cur_ticket.id, tnum, parser.Param(1));

            if (db->Status() == DB_OK)
            {
              cur_ticket = db->GetTicket(cur_ticket.id);
            }

            else
            {
              app_status = APP_ER;
              status_line = "Changing Task ETA [index = " +
                            parser.Param(0) + "] -> " +
                            parser.Param(1) + " : Failed!";
            }
          }

          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Changing Task ETA [index = " +
                          parser.Param(0) + "] -> " +
                          parser.Param(1) + ": Error / Invalid index!";
          }
        }

        // CHANGE TICKET TASK DESC /////////////////////////////////////////////
        if (parser.Cmd() == CMD_CTTD)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t tnum = std::stoul(parser.Param(0));

            db->EditTicketTaskDesc(cur_ticket.id, tnum, parser.Param(1));

            if (db->Status() == DB_OK)
            {
              cur_ticket = db->GetTicket(cur_ticket.id);
            }

            else
            {
              app_status = APP_ER;
              status_line = "Changing Task ETA [index = " +
                            parser.Param(0) + "] -> " +
                            parser.Param(1) + " : Failed!";
            }
          }

          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Changing Task ETA [index = " +
                          parser.Param(0) + "] -> " +
                          parser.Param(1) + ": Error / Invalid index!";
          }
        }

        // DELETE TICKET TASK //////////////////////////////////////////////////
        if (parser.Cmd() == CMD_DTT)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          try
          {
            size_t tnum = std::stoul(parser.Param(0));
 
            db->DeleteTicketTask(cur_ticket.id, tnum);

            if (db->Status() == DB_OK)
            {
              cur_ticket = db->GetTicket(cur_ticket.id);
            }

            else
            {
              app_status = APP_ER;
              status_line = "Deleting Task [index = " +
                            parser.Param(0) + "] : Failed!";
            }
          }

          catch (const std::invalid_argument &e)
          {
            app_status = APP_ER;
            status_line = "Deleting Task [index = " +
                          parser.Param(0) + "] : Error / Invalid index!";
          }
        }

        // CLOSE TICKET ////////////////////////////////////////////////////////
        if (parser.Cmd() == CMD_CLT)
        {
          if (cur_ticket.id == 0)
          {
            app_status = APP_ER;
            status_line = "You need to be in a ticket to do that!";
            _RefreshScreen();
            break;
          }

          cur_ticket.id = 0;
          cur_ticket.group_id = 0;
          cur_ticket.state = TKT_HID;
          cur_ticket.creation_date = 0;
          cur_ticket.name = "";
          cur_ticket.desc = "";
          std::vector<Attachment> empty_attachment;
          std::vector<TicketTask> empty_task;
          cur_ticket.attachments = empty_attachment;
          cur_ticket.tasks = empty_task;
          screen_status = SCR_LST;
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
        if (c >= ' ' && c <= '~' && input_line.length() < (LARGEBUF - 1))
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

  _AddHeaderContent("┌─────────────┬───────────────────┬─────────────────────────────────┐");
  _AddContent      ("│ quit        │                   │ quit the app                    │");
  _AddContent      ("│ main        │                   │ go to main screen               │");
  _AddContent      ("│ lsg         │                   │ list groups                     │");
  _AddContent      ("│ addg        │ name              │ add group                       │");
  _AddContent      ("│ swg         │ id                │ switch to group [id]            │");
  _AddContent      ("│ reng        │ new name          │ rename cur group new_name       │");
  _AddContent      ("│ delg        │                   │ delete cur group                │");
  _AddContent      ("│ lst         │                   │ list tickets in current group   │");
  _AddContent      ("│ exg         │                   │ exit group                      │");
  _AddContent      ("│ addt        │ name description  │ add ticket                      │");
  _AddContent      ("│ opt         │ id                │ open ticket [id]                │");
  _AddContent      ("│ rent        │ new name          │ rename cur ticket to new_name   │");
  _AddContent      ("│ ctd         │ new desc          │ change cur ticket desc          │");
  _AddContent      ("│ delt        │                   │ delete cur ticket               │");
  _AddContent      ("│ movt        │ group_id          │ move ticket to group [gid]      │");
  _AddContent      ("│ mtd         │                   │ mark cur ticket as DONE         │");
  _AddContent      ("│ mto         │                   │ mark cur ticket as ONGOING      │");
  _AddContent      ("│ mth         │                   │ mark cur ticket as HIDDEN       │");
  _AddContent      ("│ ata         │                   │ add ticket attachment           │");
  _AddContent      ("│ ota         │ id                │ open ticket attachment [id]     │");
  _AddContent      ("│ nta         │ name              │ new ticket attachment           │");
  _AddContent      ("│ dta         │ id                │ delete ticket attachment [id]   │");
  _AddContent      ("│ att         │ eta resource desc │ add ticket task                 │");
  _AddContent      ("│ ittp        │ id                │ Increment Ticket Task progress  │");
  _AddContent      ("│ dttp        │ id                │ Decrement Ticket Task progress  │");
  _AddContent      ("│ ctte        │ id new_eta        │ Change Ticket Task ETA YYYYMMDD │");
  _AddContent      ("│ cttr        │ id new_resource   │ Change Ticket Task Resource     │");
  _AddContent      ("│ cttd        │ id new_desc       │ Change Ticket Task Description  │");
  _AddContent      ("│ dtt         │ id                │ Delete Ticket Task [id]         │");
  _AddContent      ("│ clt         │                   │ close ticket                    │");
  _AddContent      ("└─────────────┴───────────────────┴─────────────────────────────────┘");
}

void UI::_ListGroups()
{
  screen_title = "    ID NAME";
  screen_header.clear();
  screen_content.clear();
  char buf[w.ws_col + 1];

  for (auto &gr : db->GetGroups())
  {
    sprintf(buf, "%6ld %s", gr.id, gr.name.substr(0, w.ws_col - 7).c_str());
    _AddContent(buf);
  }
}

void UI::_ListTickets()
{
  screen_title = "    ID   STATE NAME                 DESCRIPTION";
  screen_header.clear();
  screen_content.clear();
  char buf[w.ws_col + 1];

  for (auto &tkt : db->GetTickets(cur_group.id))
  {
    if (tkt.name.length() <= 20)
    {
      sprintf(buf, "%6ld %7s %-20s %s", tkt.id, TicketStateToStr(tkt.state), tkt.name.c_str(), tkt.desc.substr(0, w.ws_col - 36).c_str());
    }

    else
    {  
      sprintf(buf, "%6ld %7s %-19s┄ %s", tkt.id, TicketStateToStr(tkt.state), tkt.name.substr(0, 19).c_str(), tkt.desc.substr(0, w.ws_col - 36).c_str());
    }

    _AddContent(buf);
  }
}

void UI::_SetupTicketView()
{
  if (cur_ticket.id == 0)
  {
    return;
  }

  screen_header.clear();
  screen_content.clear();

  char buf[w.ws_col + 1];

  sprintf
  (
    buf,
    "[id: %6ld] [Created: %s] [State: %s]",
    cur_ticket.id,
    TimestampToString(cur_ticket.creation_date).c_str(),
    TicketStateToStr(cur_ticket.state)
  );

  screen_title = buf;

  _AddHeaderContent("");

  for (auto &line : wrap_string(cur_ticket.desc, w.ws_col))
  {
    _AddHeaderContent(line.c_str());
  }

  _AddHeaderContent("");

  sprintf(buf, "-- Attachments ");

  for (size_t i = 15; i < w.ws_col; i++)
  {
    buf[i] = '-';
  }

  buf[w.ws_col] = '\0';
  _AddHeaderContent(buf);

  for (auto &att : cur_ticket.attachments)
  {
    sprintf(buf, "[%2ld: %s]", att.id, att.name.substr(0, w.ws_col - 6).c_str());
    _AddHeaderContent(buf);
  }

  _AddHeaderContent("");
  sprintf(buf, "-- TASKS ------");

  for (size_t i = 15; i < w.ws_col; i++)
  {
    buf[i] = '-';
  }

  buf[w.ws_col] = '\0';
  _AddHeaderContent(buf);
  _AddHeaderContent(" ID   \%        ETA Resource             Description");

  for (auto &tsk : cur_ticket.tasks)
  {
    sprintf
    (
      buf,
      "%3ld %3d %s %-20s %s",
      tsk.id,
      tsk.progress,
      TimestampToStringNoTime(tsk.eta).c_str(),
      tsk.resource.substr(0, 20).c_str(),
      tsk.desc.substr(0, w.ws_col - 40).c_str()
    );
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

  switch (screen_status)
  {
    case SCR_QUIT:
    {

    } break;

    case SCR_MAIN:
    {
      _SetupWelcomePage();

      if (app_status == APP_OK)
      {
        status_line = "quit | main | lsg | addg | swg";
      }

    } break;

    case SCR_LSG:
    {
      _ListGroups();

      if (app_status == APP_OK)
      {
        status_line = "quit | main | lsg | addg | swg";
      }

    } break;

    case SCR_LST:
    {
      _ListTickets();

      if (app_status == APP_OK)
      {
        status_line = "reng | delg | lst | addt | opt | exg";
      }

    } break;

    case SCR_TKV:
    {
      _SetupTicketView();

      if (app_status == APP_OK)
      {
        status_line = "rent | ctd | delt | movt | mtd | mto | mth | ata | ota | nta | dta | att | ittp | dttp | ctte | cttr | cttd | dtt | clt";
      }

    } break;
  }
  
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

  // Draw Footer
  _MoveTo(w.ws_row - 3, 1);
  _SetColor(CLR_BLUE_FG);
  
  for (uint16_t i = 0; i < w.ws_col; i++)
  {
    wprintf(L"─");
  }

  _SetColor(CLR_BLACK_BG);
  _MoveTo(w.ws_row - 2, 1);
  
  for (uint16_t i = 0; i < w.ws_col; i++)
  {
    wprintf(L" ");
  }

  _MoveTo(w.ws_row - 2, 1);
  wprintf(L"◇ ");
  
  if (cur_group.id > 0)
  {
    wprintf(L"%s", cur_group.name.c_str());
  }

  if (cur_ticket.id > 0)
  {
    _SetColor(CLR_BOLD);
    wprintf(L" ▷ %s", cur_ticket.name.c_str());
  }

  _MoveTo(w.ws_row - 1, 1);
  
  for (uint16_t i = 0; i < w.ws_col; i++)
  {
    wprintf(L" ");
  }

  _MoveTo(w.ws_row - 1, 1);
  _SetColor(CLR_DEFAULT);
  _SetColor(CLR_BLACK_BG);

  switch (app_status)
  {
    case APP_ER:
    {
      _SetColor(CLR_RED_FG);

    } break;
    case APP_WR:
    {
      _SetColor(CLR_YELLOW_FG);

    } break;
    case APP_WT:
    {
      _SetColor(CLR_YELLOW_FG);

    } break;
    case APP_OK:
    {
      _SetColor(CLR_GREEN_FG);

    } break;

    _SetColor(CLR_WHITE_FG);
  }

  if (num_of_pages == 0)
  {
    wprintf(L"%s", status_line.substr(0, w.ws_col).c_str());
  }

  else
  {
    wprintf(L"%s [ Page %2ld of %2ld ]", status_line.substr(0, w.ws_col - 18).c_str(),
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

  tv.tv_sec = 1;
  tv.tv_usec = 0;

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
    screen_status = SCR_QUIT;
  }

  if (w.ws_col < min_w)
  {
    exit_message = "App Cannot Continue. Term Width = " +
      Uint64_TToString(w.ws_col) + " < " + Uint64_TToString(min_w);
    screen_status = SCR_QUIT;
  }

  sc_num_of_lines = w.ws_row - screen_header.size() - 5;
  num_of_pages = screen_content.size() / sc_num_of_lines;

  if (num_of_pages > 0)
  {
    sc_num_of_lines = (screen_content.size() + num_of_pages) / (num_of_pages + 1);
  }
}

void UI::_EchoInputLine()
{
  _SetColor(CLR_BLUE_FG);
  _SetColor(CLR_BLACK_BG);
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
