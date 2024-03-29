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
  CMD_FDT,
  CMD_FOT,
  CMD_FHT,
  CMD_FAT,
  CMD_LSG,
  CMD_ADDG,
  CMD_SWG,
  CMD_RENG,
  CMD_DELG,
  CMD_LST,
  CMD_EXG,
  CMD_ADDT,
  CMD_OPT,
  CMD_RENT,
  CMD_CTD,
  CMD_DELT,
  CMD_MOVT,
  CMD_MTO,
  CMD_MTD,
  CMD_MTH,
  CMD_ATA,
  CMD_OTA,
  CMD_NTA,
  CMD_DTA,
  CMD_ATT,
  CMD_ITTP,
  CMD_DTTP,
  CMD_CTTE,
  CMD_CTTR,
  CMD_CTTD,
  CMD_DTT,
  CMD_CLT

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
