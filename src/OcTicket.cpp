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

void OcTicket::SetResource(uint64_t resource)
{
  this->resource = resource;
}

void OcTicket::SetName(std::string name)
{
  this->name = name;
}


