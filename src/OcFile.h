/*
 * File:   OcFile.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-13
 */

#ifndef OCFILE_H
#define OCFILE_H

#include "MtInclude.h"
#include "OcTicket.h"

class OcFile
{
public:
  OcFile();
  ~OcFile();

  bool ConfirmDataLoc(const char *dir);
  bool SaveTicketToFile(OcTicket *ticket);

protected:
private:
  void _ReadTicketsFromFile();
  bool _WriteTicketsToFile();

  std::string data_dir;
  std::vector<OcTicket> tickets;

};

#endif
