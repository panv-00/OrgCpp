/**
 * File:   Parser.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-25
 */

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>

typedef enum
{
  TKN_OK,
  TKN_BAD_ARG,
  TKN_BAD_CMD,

} TokenStatus;

const char *PERR(TokenStatus st);

typedef enum
{
  CMD_QUIT,
  CMD_MAIN,
  CMD_LSG,
  CMD_ADDG,
  CMD_RENG,
  CMD_DELG,
  CMD_SWG,
  CMD_EXG,
  CMD_LST,
  CMD_ADDT,
  CMD_RENT,
  CMD_CTD

} CommandType;

class Parser
{
public:
  Parser();
  ~Parser();

  TokenStatus Status();
  CommandType Cmd();
  std::string Param(size_t index);

  void Parse(std::string &line);

private:
  int _IsCommand(const std::string &token);

  TokenStatus status;
  std::vector<std::string> tokens;

  std::vector<std::string> commands;
  std::vector<size_t> n_param;

  CommandType command;
  std::vector<std::string> params;
};
