/*
 * File:   OrgCpp.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "MtWindow.h"
#include "OcTicket.h"
#include "OcUtils.h"

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

  char *home = getenv("HOME");

  if (home == nullptr)
  {
    wprintf(L"Error: Home env variable not set. Exiting now!\n");
    return 1;
  }

  std::string data_dir = std::string(home) + "/.orgcpp";
  struct stat info;

  if (stat(data_dir.c_str(), &info) != 0)
  {
    int status = mkdir(data_dir.c_str(), 0700);

    if (status != 0)
    {
      wprintf(L"Error: Unable to create directory %s. ", data_dir.c_str());
      wprintf(L"Exiting now!\n");
      return 1;
    }
  }

  else if (!S_ISDIR(info.st_mode))
  {
    wprintf(L"Error: %s exists, but is not a directory. ", data_dir.c_str());
    wprintf(L"Exiting now!\n");
    return 1;
  }

  MtWindow *win = new MtWindow();
  MtMenu *menu = new MtMenu();

  menu->AddOption({0, ':', "Run",        CallMenuRun,       1});
  menu->AddOption({1, 'n', "New Ticket", CallMenuNewTicket, 0});
  menu->AddOption({1, 'q', "Quit",       CallMenuExit,      0});

  win->SetMainMenu(menu);
  win->Run();

  delete menu;
  delete win;

  return 0;
}
