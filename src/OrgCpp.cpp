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
  win->Run();

  delete win;

  return 0;
}
