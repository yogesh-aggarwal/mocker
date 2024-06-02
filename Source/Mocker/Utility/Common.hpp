#pragma once

#include <string>

namespace Common
{
   std::string
   GenerateNanoID(int length = 10, std::string dictionary = "1234567890abcdef");
}
