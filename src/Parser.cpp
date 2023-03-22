/**
 * File:   Parser.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-25
 */

#include "Parser.h"

const char *PERR(TokenStatus st)
{
  switch (st)
  {
    case TKN_OK: return "";
    case TKN_BAD_ARG: return "wrong arguments";
    case TKN_BAD_CMD: return "command not found";
  }
  return "unknown error";
}

Parser::Parser()
{
  commands =
  {
    "quit", "main",  "fdt",  "fot",  "fht", "fat",
     "lsg", "addg",  "swg", "reng", "delg",
     "lst",  "exg", "addt",  "opt", "rent",
     "ctd", "delt", "movt",  "mto",  "mtd",
     "mth",  "ata",  "ota",  "nta",  "dta",
     "att", "ittp", "dttp", "ctte", "cttr",
    "cttd",  "dtt",  "clt"                 
  };

  n_param =
  {
    0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 0,
    0, 0, 2, 1, 1,
    1, 0, 1, 0, 0,
    0, 1, 1, 1, 1,
    3, 1, 1, 2, 2,
    2, 1, 0       
  };
}

Parser::~Parser()
{

}

TokenStatus Parser::Status()
{
  return status;
}

CommandType Parser::Cmd()
{
  return command;
}

std::string Parser::Param(size_t index)
{
  return params[index];
}

void Parser::Parse(std::string &line)
{
  status = TKN_OK;
  tokens.clear();
  params.clear();

  std::istringstream iss(line);
  std::string token;

  while (iss >> std::quoted(token))
  {
    tokens.push_back(token);
  }

  if (tokens.size() < 1)
  {
    status = TKN_BAD_ARG;
    return;
  }

  int cmd = _IsCommand(tokens[0]);
  
  if (cmd < 0)
  {
    status = TKN_BAD_CMD;
    return;
  }

  command = (CommandType) cmd;

  if (tokens.size() != n_param[cmd] + 1)
  {
    status = TKN_BAD_ARG;
    return;
  }

  for (size_t i = 1; i < n_param[cmd] + 1; i++)
  {
    params.push_back(tokens[i]);
  }
}

////////////////////////////////////////////////////////////////////////////////

int Parser::_IsCommand(const std::string &token)
{
  auto it = std::find(commands.begin(), commands.end(), token);

  if (it != commands.end())
  {
    return std::distance(commands.begin(), it);
  }
  
  return -1;
}
