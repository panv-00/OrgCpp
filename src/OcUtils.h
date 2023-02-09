/*
 * File:   OcUtils.h
 * Author: Michel Alwan
 *
 * Created on 2023-02-09
 */

#ifndef OCUTILS_H
#define OCUTILS_H

#include "MtInclude.h"

uint64_t Now();
std::string TimestampToString(uint64_t ts);
std::string SizeTToString(size_t value);
std::string Uint64_TToString(uint64_t value);

#endif
