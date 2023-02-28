/**
 * File:   Utils.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#include <string>
#include <chrono>
#include <algorithm>

void CleanString(std::string &input);
uint64_t Now();
std::string TimestampToString(uint64_t ts);
std::string SizeTToString(size_t value);
std::string Uint64_TToString(uint64_t value);
bool Contains(const std::string &str, const std::string &substr);
bool Compare(const std::string &str1, const std::string &str2);
