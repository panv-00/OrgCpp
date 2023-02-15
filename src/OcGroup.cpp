/**
 * File:   OcGroup.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-14
 */

#include "OcGroup.h"

OcGroup::OcGroup()
{

}

OcGroup::~OcGroup()
{

}

void OcGroup::SetIndex(uint64_t index)
{
  this->index = index;
}

void OcGroup::SetName(std::string name)
{
  this->name = name;
}

void OcGroup::Echo()
{
  wprintf(L"\n\tGroup ID:   %ld", index);
  wprintf(L"\n\tGroup Name: %s", name.c_str());
}
