/**
 * File:   OcTicket.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#ifndef OCTICKET_H
#define OCTICKET_H

#include "MtInclude.h"

class OcTicket
{
public:
  OcTicket();
  ~OcTicket();

  void SetIndex(uint64_t index);
  void SetGroup(uint64_t group);
  void SetName(std::string name);

  uint64_t GetId();
  uint64_t GetGroup();
  std::string GetName();

protected:

private:
  uint64_t index;
  uint64_t group;
  std::string name;
  std::vector<std::string> data;
  std::vector<uint64_t> action_by;
  std::vector<TicketStatus> status;
  uint64_t start;
  uint64_t deadline;
  uint64_t end;
  std::vector<std::string> attach;
};

#endif
