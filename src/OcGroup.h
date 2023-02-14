/*
 * File:   OcGroup.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-14
 */

#ifndef OCGROUP_H
#define OCGROUP_H

#include "MtInclude.h"

class OcGroup
{
public:
  OcGroup();
  ~OcGroup();

  void SetIndex(uint64_t index);
  void SetName(std::string name);

  void Echo();

protected:

private:
  uint64_t index;
  std::string name;
};

#endif
