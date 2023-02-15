/**
 * File:   OrgCpp.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "OrgCpp.h"

OrgCpp::OrgCpp()
{
  ticket = new OcTicket();
  ocfile = new OcFile();
  menu = new MtMenu();
  win = new MtWindow();
  
  win->SetParent(this);
  win->SetMainMenu(menu);
}

OrgCpp::~OrgCpp()
{
  delete ticket;
  delete ocfile;
  delete menu;
  delete win;
}

// menu functions /////////////////////////////////////////////////////////////

void OrgCpp::CallMenuRun()
{
  win->CallOption_Run();
}

void OrgCpp::CallMenuNewTicket()
{
  std::vector<MtPair> select_options;
  MtInputBox box;
  std::string temp_string = "";
  bool retry_loop = true;

  ticket->SetIndex(Now());
  ticket->SetGroup(Now());
  
  box = {"Ticket Name", 60, INPUT_TEXT};

  while (retry_loop)
  {
    retry_loop = !win->GetInputBoxResult
                 (
                   box, INPUT_TEXT, select_options, temp_string
                 );
  }

  CleanString(temp_string);

  ticket->SetName(temp_string);
  win->ClearScreenContent();
  win->AppendScreenContent
    ("Group:      " + Uint64_TToString(ticket->GetGroup()));
  win->AppendScreenContent
    ("TicketID:   " + Uint64_TToString(ticket->GetId()));
  win->AppendScreenContent
    ("TicketName: " + ticket->GetName());
}

void OrgCpp::CallMenuExit()
{
  win->CallOption_Exit();
}

void OrgCpp::CallMenuConfSaveTicket()
{
  win->ClearScreenContent();
  
  if (_SaveTicketToFile())
  {
    win->AppendScreenContent("Record Saved Successfully.");
  }

  else
  {
    win->AppendScreenContent("IO Error: Could not save file.");
  }
}

void OrgCpp::CallMenuConfirmFalse()
{
  win->ClearScreenContent();
  win->AppendScreenContent("Record Discarded.");
}

// Public  Functions //////////////////////////////////////////////////////////

bool OrgCpp::ConfirmFileDataLoc(const char *d)
{
  return ocfile->ConfirmDataLoc(d);
}

void OrgCpp::AddMenuOption(MenuOption option)
{
  menu->AddOption(option);
}

void OrgCpp::RunApp()
{
  win->Run();
}

// Private Functions //////////////////////////////////////////////////////////

bool OrgCpp::_SaveTicketToFile()
{
  return ocfile->SaveTicketToFile(ticket);
}

// INT MAIN ///////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  OrgCpp *app = new OrgCpp();

  if (app->ConfirmFileDataLoc(".orgcpp"))
  {
    app->AddMenuOption
    (
      {
        0, ':', "Run", 1,
        [](void *instance)
          { static_cast<OrgCpp *>(instance)->CallMenuRun(); }, app
      }
    );

    app->AddMenuOption
    (
      {
        1, 'n', "New Ticket", 11,
        [](void *instance)
          { static_cast<OrgCpp *>(instance)->CallMenuNewTicket(); }, app
      }
    );

    app->AddMenuOption
    (
      {
        1, 'q', "Quit", 0,
        [](void *instance)
          { static_cast<OrgCpp *>(instance)->CallMenuExit(); }, app
      }
    );

    app->AddMenuOption
    (
      {
        11, 's', "Save Ticket", 0,
        [](void *instance)
          { static_cast<OrgCpp *>(instance)->CallMenuConfSaveTicket(); }, app
      }
    );

    app->AddMenuOption
    (
      {
        11, 'c', "Cancel", 0,
        [](void *instance)
          { static_cast<OrgCpp *>(instance)->CallMenuConfirmFalse(); }, app
      }
    );

    app->RunApp();
  }

  delete app;
  return 0;
}
