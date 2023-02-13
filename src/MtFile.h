/*
 * File:   MtFile.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-13
 */

#ifndef MTFILE_H
#define MTFILE_H

#include "MtInclude.h"

class MtFile
{
public:
  MtFile(const char *base_dir);
  ~MtFile();

  bool ConfirmDataLoc();

protected:
private:
  const char *base_dir;

};

#endif
