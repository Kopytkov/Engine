#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include "object.h"

Object parseObject(const std::string& file);

#endif