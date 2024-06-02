#include "Common.hpp"

#include <nanoid/nanoid.h>

//-----------------------------------------------------------------------------

std::string
Common::GenerateNanoID(int length, std::string dictionary)
{
   return nanoid::generate(dictionary, length);
}

//-----------------------------------------------------------------------------
