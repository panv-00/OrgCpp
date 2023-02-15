/**
 * File:   MtMenu.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#ifndef MTMENU_H
#define MTMENU_H

#include "MtInclude.h"

class MtMenu
{
public:
  MtMenu();
  ~MtMenu();

  MenuOption GetOption(uint8_t id) { return options[id]; };
  void AddOption(MenuOption option) { options.push_back(option); };
  size_t GetSize() { return options.size(); };

protected:
private:
  std::vector<MenuOption> options;
};

#endif
