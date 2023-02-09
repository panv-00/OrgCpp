/*
 * File:   MtMenu.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "MtMenu.h"

MtMenu::MtMenu()
{

}

MtMenu::~MtMenu()
{

}

void MtMenu::AddSubMenu
(
  char accel_key,
  const std::string &name,
  Function f,
  MtMenu *submenu
)
{
  options[accel_key] = {accel_key, name, f, submenu};
}

const std::map<char, MtMenu::MenuOption> &MtMenu::getOptions() const
{
  return options;
}
