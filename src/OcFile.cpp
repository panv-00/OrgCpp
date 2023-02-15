/**
 * File:   OcFile.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-13
 */

#include "OcFile.h"

OcFile::OcFile()
{

}

OcFile::~OcFile()
{

}

bool OcFile::ConfirmDataLoc(const char *dir)
{
  char *home = getenv("HOME");

  if (home == nullptr)
  {
    wprintf(L"Error: Home env variable not set. Exiting now!\n");
    return false;
  }

  data_dir = std::string(home) + "/" + dir;
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

bool OcFile::SaveTicketToFile(OcTicket *ticket)
{
  _ReadTicketsFromFile();
  
  bool ticketExists = false;
  
  for (size_t i = 0; i < tickets.size(); i++)
  {
    if (tickets[i].GetId() == ticket->GetId())
    {
      tickets[i].SetGroup(ticket->GetGroup());
      tickets[i].SetName(ticket->GetName());
      ticketExists = true;
      break;
    }
  }

  if (!ticketExists)
  {
    tickets.push_back(*ticket);
  }

  return _WriteTicketsToFile();
}

std::vector<MtPair> OcFile::GetGroups()
{
  std::vector<MtPair> rv;
  return rv;
}

///////////////////////////////////////////////////////////////////////////////

void OcFile::_ReadMtPairsFromFile(const char *file_name)
{
/*
  mt_pairs.clear();
  std::ifstream file(data_dir + "/" + file_name, std::ios::binary);

  if (file.is_open())
  {
    uint64_t index;
    MtWindow::MtPair mt_pair;
    size_t size;

    file.read(reinterpret_cast<char *>(&size), sizeof(std::size_t));

    for (size_t i = 0; i < size; ++i)
    {
      file.read(reinterpret_cast<char *>(&index), sizeof(uint64_t));
      file.read(reinterpret_cast<char *>(&group), sizeof(uint64_t));
      size_t name_size;
      file.read(reinterpret_cast<char *>(&name_size), sizeof(size_t));
      char *name_buf = new char[name_size];
      file.read(name_buf, name_size);

      name.assign(name_buf, name_size);
      delete[] name_buf;

      tkt.SetIndex(index);
      tkt.SetGroup(group);
      tkt.SetName(name);
      tickets.push_back(tkt);
    }

    file.close();
  }
*/
}

bool OcFile::_WriteMtPairsToFile(const char *file_name)
{
  std::ofstream file(data_dir + "/" + file_name, std::ios::binary);

  if (file.is_open())
  {
    uint64_t index;
    uint64_t group;
    std::string name;
    size_t size = tickets.size();

    file.write(reinterpret_cast<char *>(&size), sizeof(size_t));

    for (auto &ticket : tickets)
    {
      index = ticket.GetId();
      group = ticket.GetGroup();
      name  = ticket.GetName();
      size_t name_size = name.size();

      file.write(reinterpret_cast<char *>(&index), sizeof(uint64_t));
      file.write(reinterpret_cast<char *>(&group), sizeof(uint64_t));
      file.write(reinterpret_cast<char *>(&name_size), sizeof(size_t));
      file.write(name.c_str(), name_size);
    }

    file.close();
    return true;
  }

  return false;
}

void OcFile::_ReadTicketsFromFile()
{
  tickets.clear();
  std::ifstream file(data_dir + "/tickets.bin", std::ios::binary);

  if (file.is_open())
  {
    OcTicket tkt;
    uint64_t index;
    uint64_t group;
    std::string name;
    size_t size;

    file.read(reinterpret_cast<char *>(&size), sizeof(std::size_t));

    for (size_t i = 0; i < size; ++i)
    {
      file.read(reinterpret_cast<char *>(&index), sizeof(uint64_t));
      file.read(reinterpret_cast<char *>(&group), sizeof(uint64_t));
      size_t name_size;
      file.read(reinterpret_cast<char *>(&name_size), sizeof(size_t));
      char *name_buf = new char[name_size];
      file.read(name_buf, name_size);

      name.assign(name_buf, name_size);
      delete[] name_buf;

      tkt.SetIndex(index);
      tkt.SetGroup(group);
      tkt.SetName(name);
      tickets.push_back(tkt);
    }

    file.close();
  }
}

bool OcFile::_WriteTicketsToFile()
{
  std::ofstream file(data_dir + "/tickets.bin", std::ios::binary);

  if (file.is_open())
  {
    uint64_t index;
    uint64_t group;
    std::string name;
    size_t size = tickets.size();

    file.write(reinterpret_cast<char *>(&size), sizeof(size_t));

    for (auto &ticket : tickets)
    {
      index = ticket.GetId();
      group = ticket.GetGroup();
      name  = ticket.GetName();
      size_t name_size = name.size();

      file.write(reinterpret_cast<char *>(&index), sizeof(uint64_t));
      file.write(reinterpret_cast<char *>(&group), sizeof(uint64_t));
      file.write(reinterpret_cast<char *>(&name_size), sizeof(size_t));
      file.write(name.c_str(), name_size);
    }

    file.close();
    return true;
  }

  return false;
}
