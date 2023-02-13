/*
 * File:   OrgCpp.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "MtWindow.h"
#include "MtFile.h"
#include "MtUtils.h"
#include "OcTicket.h"

void CallMenuExit(MtWindow *win) { win->CallOption_Exit(); }
void CallMenuRun(MtWindow *win)  { win->CallOption_Run();  }
void CallMenuNewTicket(MtWindow *win)
{
  OcTicket ticket;

  MtWindow::MtInputBox box;

  ticket.SetIndex(Now());
  
  box = {"Ticket Name", 60, INPUT_TEXT};
  ticket.SetName(win->GetInputBoxResult(box));
}

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  MtFile *file = new MtFile(".orgcpp");
  
  if (file->ConfirmDataLoc())
  {
    MtWindow *win = new MtWindow();
    MtMenu *menu = new MtMenu();

    menu->AddOption({0, ':', "Run",        CallMenuRun,       1});
    menu->AddOption({1, 'n', "New Ticket", CallMenuNewTicket, 0});
    menu->AddOption({1, 'q', "Quit",       CallMenuExit,      0});

    win->SetMainMenu(menu);
    win->Run();

    delete menu;
    delete win;
  }

  delete file;
  return 0;
}
