/*
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
  void SetResource(uint64_t resource);
  void SetName(std::string name);

protected:

private:
  uint64_t index;                  // timestamp at the moment of creation
  uint64_t resource;          // index to resource
  std::string name;
  std::string data;                // formatted text
  uint64_t action_by;              // index to resource
  TicketStatus status;
  uint64_t start;                  // start timestamp
  uint64_t deadline;               // deadline timestamp
  uint64_t end;                    // end timestamp
  std::vector<std::string> attach;
};

#endif
