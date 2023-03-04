/**
 * File:   Utils.cpp
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include "Utils.h"

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

std::string TimestampToStringNoTime(uint64_t ts)
{
  struct tm *timeinfo;
  char buffer[11];
  time_t tts = (time_t) ts;

  timeinfo = localtime(&tts);
  strftime(buffer, sizeof(buffer), "%Y/%m/%d", timeinfo);

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

bool Contains(const std::string &str, const std::string &substr)
{
  std::string str_lower, substr_lower;

  std::transform
  (
    str.begin(),
    str.end(),
    std::back_inserter(str_lower),
    [](char c)
    {
      return std::tolower(c);
    }
  );

  std::transform
  (
    substr.begin(),
    substr.end(),
    std::back_inserter(substr_lower),
    [](char c)
    {
      return std::tolower(c);
    }
  );

  return (str_lower.find(substr_lower) != std::string::npos);
}

bool Compare(const std::string &str1, const std::string &str2)
{
  std::string str1_lower, str2_lower;

  std::transform
  (
    str1.begin(),
    str1.end(),
    std::back_inserter(str1_lower),
    [](char c)
    {
      return std::tolower(c);
    }
  );

  std::transform
  (
    str2.begin(),
    str2.end(),
    std::back_inserter(str2_lower),
    [](char c)
    {
      return std::tolower(c);
    }
  );

  return str1_lower == str2_lower;
}

uint64_t StringdateToTimestamp(std::string &sd)
{
  // Expect a string of the format YYYYMMDD
  if (sd.length() != 8)
  {
    return -1;
  }

  struct tm tm = {};
  time_t timestamp;

  if (strptime(sd.c_str(), "%Y%m%d", &tm) == NULL)
  {
    return -1;
  }

  timestamp = mktime(&tm);

  return (uint64_t) timestamp;
}
