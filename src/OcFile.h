/**
 * File:   OcFile.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-13
 */

#ifndef OCFILE_H
#define OCFILE_H

#include "MtInclude.h"
#include "MtWindow.h"
#include "OcTicket.h"

class OcFile
{
public:
  OcFile();
  ~OcFile();

  bool ConfirmDataLoc(const char *dir);
  bool SaveTicketToFile(OcTicket *ticket);

  std::vector<MtPair> GetGroups();

protected:
private:
  void _ReadMtPairsFromFile(const char *file_name);
  bool _WriteMtPairsToFile(const char *file_name);

  void _ReadTicketsFromFile();
  bool _WriteTicketsToFile();

  std::string data_dir;
  std::vector<OcTicket> tickets;
  std::vector<MtPair> mt_pairs;

};

#endif
