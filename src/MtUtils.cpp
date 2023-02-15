/**
 * File:   OcUtils.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "MtUtils.h"

void CleanString(std::string &input)
{
  input.erase(0, input.find_first_not_of(' '));
  input.erase(input.find_last_not_of(' ') + 1);
}

uint64_t Now()
{
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);

  return (uint64_t) (now_c);
}

std::string TimestampToString(uint64_t ts)
{
  struct tm *timeinfo;
  char buffer[16];
  time_t tts = (time_t) ts;

  timeinfo = localtime(&tts);
  strftime(buffer, sizeof(buffer), "%Y%m%d-%H%M%S", timeinfo);

  return std::string(buffer);
}

std::string SizeTToString(size_t value)
{
  char buffer[32];
  std::snprintf(buffer, 32, "%zu", value);

  return buffer;
}

std::string Uint64_TToString(uint64_t value)
{
  char buffer[32];
  std::snprintf(buffer, 32, "%lu", value);

  return buffer;
}

