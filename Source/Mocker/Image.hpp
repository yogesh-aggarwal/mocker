#pragma once

#include <string>

class Image
{
public:
   struct Config
   {
      std::string alias;
      std::string path;
   };

private:
   Config m_Config;

public:
   Image(const Config &config);

   void
   SetAlias(const std::string &alias);

   const std::string &
   GetAlias() const;

   void
   SetPath(const std::string &path);

   const std::string &
   GetPath() const;
};
