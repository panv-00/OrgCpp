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

class OrgCpp
{
public:

  OrgCpp();
  ~OrgCpp();

  void CallMenuRun();
  void CallMenuNewTicket();
  void CallMenuExit();
  void CallMenuConfSaveTicket();
  void CallMenuConfirmFalse();

  bool ConfirmFileDataLoc(const char *d);
  void AddMenuOption(MenuOption option);
  void RunApp();

private:
  bool _SaveTicketToFile();
  
  OcTicket *ticket;
  OcFile *ocfile;
  MtMenu *menu;
  MtWindow *win;
};

#endif
