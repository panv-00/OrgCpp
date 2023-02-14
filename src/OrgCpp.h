/**
 * File:   OrgCpp.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-14
 */

#ifndef ORGCPP
#define ORGCPP

#include "MtWindow.h"
#include "MtUtils.h"
#include "OcFile.h"
#include "OcTicket.h"
#include <vector>

void CallMenuExit(MtWindow *win);
void CallMenuRun(MtWindow *win);
void CallMenuNewTicket(MtWindow *win);
void CallMenuConfirmTrue(MtWindow *win);
void CallMenuConfirmFalse(MtWindow *win);

class OrgCpp
{
public:
  OrgCpp();
  ~OrgCpp();

  bool SaveTicketToFile();

  bool ConfirmFileDataLoc(const char *d) { return ocfile->ConfirmDataLoc(d); };
  void AddMenuOption(MtMenu::MenuOption option) { menu->AddOption(option); };
  void SetAppMainMenu(MtMenu *menu) { win->SetMainMenu(menu); };
  void RunApp() { win->Run(); };

  OcTicket *GetTicket() { return ticket;}
  MtMenu *GetMenu() { return menu; };
  MtWindow *GetWin() { return win; };


private:
  OcTicket *ticket;
  OcFile *ocfile;
  MtMenu *menu;
  MtWindow *win;
};

#endif
