/*
 * File:   OrgCpp.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "MtWindow.h"

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");

  MtWindow *win = new MtWindow();
  win->Run();

  delete win;

  return 0;
}
