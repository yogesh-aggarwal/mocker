#pragma once

#include <string>

#include <Mocker/Context/Context.hpp>
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
   pid_t          m_PID;
   std::string    m_ID;
   Ref<Context>   m_Context;
   Config         m_Config;
   Ref<Namespace> m_Namespace;

public:
   Container(Ref<Context> context, const Config &config);
   Container(Ref<Context> context, const Config &config, const Namespace &ns);
   Container(Ref<Context> context, const Config &config, Ref<Namespace> ns);

   // static Result<Ref<Container>>
   // FromConfigFile(const std::string &path);

   Result<bool>
   Init();

   std::string
   GetID() const;

   void
   SetAlias(std::string alias);

   const std::string &
   GetAlias() const;

   Ref<Namespace>
   GetNamespace() const;

   Result<bool>
   Run();

   Result<bool>
   Terminate();

   Result<bool>
   Purge();

   bool
   IsRunning() const;

   void
   Execute(const std::string              &command,
           const std::vector<std::string> &args) const;

private:
   static int
   ProceessFn(void *args);
};
