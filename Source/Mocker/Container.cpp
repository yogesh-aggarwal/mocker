#include "Container.hpp"

#include <sys/wait.h>
#include <yaml-cpp/yaml.h>

//-----------------------------------------------------------------------------

Container::Container(const Config &config) : m_Config(config)
{
   m_Namespace = CreateRef<Namespace>(Namespace::Config {
       .mountPoint = "/home/yogesh/Desktop/ubuntu",
       .hostname   = "mocker",
   });
}

//-----------------------------------------------------------------------------

Container::Container(const Config &config, const Namespace &ns)
    : m_Config(config), m_Namespace(CreateRef<Namespace>(ns))
{
}

//-----------------------------------------------------------------------------

Container::Container(const Config &config, Ref<Namespace> ns)
    : m_Config(config), m_Namespace(ns)
{
}

//-----------------------------------------------------------------------------

Result<Ref<Container>>
Container::FromConfigFile(const std::string &path)
{
   try
   {
      YAML::Node config = YAML::LoadFile(path);

      if (!config["alias"] || !config["image"])
      {
         return Result<Ref<Container>> {
            nullptr,
            new Error { { MOCKER_CONTAINER_ERROR_CONFIG_PARSE,
                          "Invalid container configuration file" } }
         };
      }

      return Result<Ref<Container>> {
         CreateRef<Container>(Container::Config {
             .alias = config["alias"].as<std::string>(),
         }),
      };
   }
   catch (std::exception e)
   {
      return Result<Ref<Container>> {
         nullptr,
         new Error { { MOCKER_CONTAINER_ERROR_CONFIG_PARSE, e.what() } }
      };
   }
}

//-----------------------------------------------------------------------------

void
Container::SetAlias(const std::string &alias)
{
   m_Config.alias = alias;
}

//-----------------------------------------------------------------------------

const std::string &
Container::GetAlias() const
{
   return m_Config.alias;
}

//-----------------------------------------------------------------------------

Ref<Namespace>
Container::GetNamespace() const
{
   return m_Namespace;
}

//-----------------------------------------------------------------------------

Result<bool>
Container::Run() const
{
   Result<bool> res { false };

   size_t stackSize = 1024 * 1024;   // 1 MB
   char  *stack     = new char[stackSize];
   if (!stack)
   {
      return Result<bool> { false,
                            new Error {
                                { MOCKER_CONTAINER_ERROR_STACK_ALLOC,
                                  "Failed to allocate stack memory" } } };
   }

   auto args = std::make_tuple<const Container *,
                               std::string,
                               std::vector<std::string>>(this, "/bin/sh", {});

   // Clone a new process
   int result = clone(ProceessFn,
                      stack + stackSize,
                      SIGCHLD | CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWIPC |
                          CLONE_NEWPID | CLONE_NEWNET,
                      &args);
   if (result == -1)
   {
      return Result<bool> { false,
                            new Error { { MOCKER_CONTAINER_ERROR_CLONE,
                                          "Failed to clone a new process" } } };
   }

   return res;
}

//-----------------------------------------------------------------------------

int
Container::ProceessFn(void *args)
{
   Result<bool> res { false };

   // Retrieve the command and arguments from the passed argument
   auto *data = reinterpret_cast<
       std::tuple<const Container *, std::string, std::vector<std::string>> *>(
       args);

   const Container                *container     = std::get<0>(*data);
   const std::string              &command       = std::get<1>(*data);
   const std::vector<std::string> &secondaryArgs = std::get<2>(*data);

   res = container->GetNamespace()->Init().WithErrorHandler([](auto error) {
      error->Print("Namespace initialization failed");
   });
   if (!res) return EXIT_FAILURE;

   container->Execute(command, secondaryArgs);

   return EXIT_FAILURE;
}

//-----------------------------------------------------------------------------

void
Container::Execute(const std::string              &command,
                   const std::vector<std::string> &args) const
{
   // Prepare the arguments for execv
   const char *execArgs[1 + args.size() + 1] = { command.c_str() };
   for (size_t i = 0; i < args.size(); i++)
      execArgs[i + 1] = args[i].c_str();
   execArgs[1 + args.size()] = nullptr;

   // Execute the new program using execv
   execv(command.c_str(), const_cast<char **>(execArgs));
   perror("execv failed");   // If execv returns, there was an error
}

//-----------------------------------------------------------------------------
