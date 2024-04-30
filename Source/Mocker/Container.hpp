#pragma once

#include <string>

#include <Mocker/Namespace.hpp>
#include <Mocker/Image.hpp>

class Container
{
public:
   struct Config
   {
      std::string alias;
      Ref<Image>  image;
   };

private:
   Config         m_Config;
   Ref<Namespace> m_Namespace;

public:
   Container(const Config &config);
   Container(const Config &config, const Namespace &ns);
   Container(const Config &config, Ref<Namespace> ns);

   static Result<Ref<Container>>
   FromConfigFile(const std::string &path);

   void
   SetAlias(const std::string &alias);

   const std::string &
   GetAlias() const;

   Ref<Namespace>
   GetNamespace() const;

   Result<bool>
   Run() const;

   void
   Execute(const std::string              &command,
           const std::vector<std::string> &args) const;

private:
   static int
   ProceessFn(void *args);
};
