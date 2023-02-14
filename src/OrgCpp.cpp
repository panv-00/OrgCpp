/*
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
}

OrgCpp::~OrgCpp()
{
  delete ticket;
  delete ocfile;
  delete menu;
  delete win;
}

bool OrgCpp::SaveTicketToFile()
{
  return ocfile->SaveTicketToFile(ticket);
}

void CallMenuExit(MtWindow *win) { win->CallOption_Exit(); }

void CallMenuRun(MtWindow *win)  { win->CallOption_Run();  }

void CallMenuNewTicket(MtWindow *win)
{
  OcTicket *ticket = win->GetParent()->GetTicket();
  MtWindow::MtInputBox box;
  std::string temp_string = "";
  bool retry_loop = true;

  ticket->SetIndex(Now());
  ticket->SetGroup(Now());
  
  box = {"Ticket Name", 60, INPUT_TEXT};

  while (retry_loop)
  {
    retry_loop = !win->GetInputBoxResult(box, INPUT_TEXT, temp_string);
  }

  ticket->SetName(temp_string);
  win->ClearScreenContent();
  win->AppendScreenContent("Group:      " + Uint64_TToString(ticket->GetGroup()));
  win->AppendScreenContent("TicketID:   " + Uint64_TToString(ticket->GetId()));
  win->AppendScreenContent("TicketName: " + ticket->GetName());
}

void CallMenuConfSaveTicket(MtWindow *win)
{
  win->ClearScreenContent();
  
  if (win->GetParent()->SaveTicketToFile())
  {
    win->AppendScreenContent("Record Saved Successfully.");
  }

  else
  {
    win->AppendScreenContent("IO Error: Could not save file.");
  }
}

void CallMenuConfirmFalse(MtWindow *win)
{
  win->ClearScreenContent();
  win->AppendScreenContent("Record Discarded.");
}

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  OrgCpp *app = new OrgCpp();

  if (app->ConfirmFileDataLoc(".orgcpp"))
  {
    app->AddMenuOption({0,  ':', "Run",         CallMenuRun,            1});
    app->AddMenuOption({1,  'n', "New Ticket",  CallMenuNewTicket,     11});
    app->AddMenuOption({1,  'q', "Quit",        CallMenuExit,           0});
    app->AddMenuOption({11, 's', "Save Ticket", CallMenuConfSaveTicket, 0});
    app->AddMenuOption({11, 'c', "Cancel",      CallMenuConfirmFalse,   0});

    app->SetAppMainMenu(app->GetMenu());

    app->RunApp();
  }

  delete app;
  return 0;
}
