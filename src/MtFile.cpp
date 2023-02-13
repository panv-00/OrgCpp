/*
 * File:   MtFile.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-13
 */

#include "MtFile.h"

MtFile::MtFile(const char *base_dir) :
  base_dir{base_dir}
{

}

MtFile::~MtFile()
{

}

bool MtFile::ConfirmDataLoc()
{
  char *home = getenv("HOME");

  if (home == nullptr)
  {
    wprintf(L"Error: Home env variable not set. Exiting now!\n");
    return false;
  }

  std::string data_dir = std::string(home) + "/" + base_dir;
  struct stat info;

  if (stat(data_dir.c_str(), &info) != 0)
  {
    int status = mkdir(data_dir.c_str(), 0700);

    if (status != 0)
    {
      wprintf(L"Error: Unable to create directory %s. ", data_dir.c_str());
      wprintf(L"Exiting now!\n");
      return false;
    }
  }

  else if (!S_ISDIR(info.st_mode))
  {
    wprintf(L"Error: %s exists, but is not a directory. ", data_dir.c_str());
    wprintf(L"Exiting now!\n");
    return false;
  }

  return true;
}
