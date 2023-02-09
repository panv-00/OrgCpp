/*
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
  using Function = std::function<void()>;

  typedef struct
  {
    char accel_key;
    std::string name;
    Function f;
    MtMenu *submenu;

  } MenuOption;

  MtMenu();
  ~MtMenu();

  void AddSubMenu
  (
    char accel_key,
    const std::string &name,
    Function f,
    MtMenu *submenu
  );

  const std::map<char, MenuOption> &getOptions() const;

protected:

private:

  std::map<char, MenuOption> options;

};

#endif
