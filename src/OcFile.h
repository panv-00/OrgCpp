/*
 * File:   OcFile.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-13
 */

#ifndef OCFILE_H
#define OCFILE_H

#include "MtInclude.h"

class OcFile
{
public:
  OcFile();
  ~OcFile();

  bool ConfirmDataLoc(const char *dir);

protected:
private:
  std::string data_dir;

};

#endif
