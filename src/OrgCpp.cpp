/**
 * File:   OrgCpp.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-24
 */

#include <stdio.h>
#include "UI.h"
#include "Parser.h"

int main(int argc, char *argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");
  setbuf(stdout, NULL);

  UI *ui = new UI();
  ui->Run();

  delete ui;
  return 0;
}
