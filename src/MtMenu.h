/*
 * File:   MtMenu.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#ifndef MTMENU_H
#define MTMENU_H

class MtWindow;

#include "MtInclude.h"

class MtMenu
{
public:
  using Function = std::function<void(MtWindow *)>;

  typedef struct
  {
    uint8_t id;
    char accel_key;
    std::string name;
    Function f;
    uint8_t next_menu_id;

  } MenuOption;

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
