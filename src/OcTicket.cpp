/*
 * File:   OcTicket.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "OcTicket.h"

OcTicket::OcTicket()
{

}

OcTicket::~OcTicket()
{

}

void OcTicket::SetIndex(uint64_t index)
{
  this->index = index;
}

void OcTicket::SetGroup(uint64_t group)
{
  this->group = group;
}

void OcTicket::SetName(std::string name)
{
  this->name = name;
}

uint64_t OcTicket::GetId()
{
  return index;
}

uint64_t OcTicket::GetGroup()
{
  return group;
}

std::string OcTicket::GetName()
{
  return name;
}
