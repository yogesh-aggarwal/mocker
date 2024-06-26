#pragma once

#include <string>

#include <Mocker/Context/FSContext.hpp>
#include <Mocker/Core/Result.tcc>

class Image
{
public:
   struct Config
   {
      std::string alias;
      std::string path;
   };

private:
   Config         m_Config;
   Ref<FSContext> m_FSContext;

public:
   Image(Ref<FSContext> fsContext, const Config &config);

   void
   SetAlias(std::string alias);

   const std::string &
   GetAlias() const;

   void
   SetPath(std::string path);

   const std::string &
   GetPath() const;

   Result<bool>
   CheckExists() const;

   Result<bool>
   Pull(const std::string &repository) const;

   Result<bool>
   Remove() const;

   Result<bool>
   Replicate(const std::string &destination) const;
};
