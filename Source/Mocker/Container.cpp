#include "Container.hpp"

#include <sys/wait.h>
#include <fmt/core.h>
#include <yaml-cpp/yaml.h>

//-----------------------------------------------------------------------------

Container::Container(Ref<Context> context, const Config &config)
    : m_PID(-1), m_Context(context), m_Config(config)
{
   m_Namespace = CreateRef<Namespace>(Namespace::Config {
       .mountPoint = "/home/yogesh/Desktop/ubuntu",
       .hostname   = "mocker",
   });
}

//-----------------------------------------------------------------------------

Container::Container(Ref<Context>     context,
                     const Config    &config,
                     const Namespace &ns)
    : m_PID(-1), m_Context(context), m_Config(config),
      m_Namespace(CreateRef<Namespace>(ns))
{
}

//-----------------------------------------------------------------------------

Container::Container(Ref<Context>   context,
                     const Config  &config,
                     Ref<Namespace> ns)
    : m_PID(-1), m_Context(context), m_Config(config), m_Namespace(ns)
{
}

//-----------------------------------------------------------------------------

// Result<Ref<Container>>
// Container::FromConfigFile(const std::string &path)
// {
//    try
//    {
//       YAML::Node config = YAML::LoadFile(path);

//       if (!config["alias"] || !config["image"])
//       {
//          return Result<Ref<Container>> {
//             nullptr,
//             new Error { { MOCKER_CONTAINER_ERROR_CONFIG_PARSE,
//                           "Invalid container configuration file" } }
//          };
//       }

//       return Result<Ref<Container>> {
//          CreateRef<Container>(Container::Config {
//              .alias = config["alias"].as<std::string>(),
//          }),
//       };
//    }
//    catch (std::exception e)
//    {
//       return Result<Ref<Container>> {
//          nullptr,
//          new Error { { MOCKER_CONTAINER_ERROR_CONFIG_PARSE, e.what() } }
//       };
//    }
// }

//-----------------------------------------------------------------------------

void
Container::SetAlias(std::string alias)
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

bool
Container::IsRunning() const
{
   return m_PID != -1;
}

//-----------------------------------------------------------------------------

Result<bool>
Container::Terminate()
{
   if (m_PID == -1)
      return Result<bool> { false,
                            new Error(
                                { MOCKER_CONTAINER_TERMINATE_ERROR_NOT_RUNNING,
                                  "Container is not running" }) };

   if (kill(m_PID, SIGKILL) == -1)
      return Result<bool> { false,
                            new Error(
                                { MOCKER_CONTAINER_TERMINATE_ERROR_FAILED,
                                  "Failed to terminate the container" }) };

   int status;
   waitpid(m_PID, &status, 0);

   m_PID = -1;

   return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Container::Run()
{
   if (m_PID != -1)
   {
      const std::string message =
          fmt::format("Container '{}' is already running", m_Config.alias);
      return Result<bool> { false,
                            new Error({ MOCKER_CONTAINER_ERROR_ALREADY_RUNNING,
                                        message }) };
   }

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

   m_PID = result;

   printf("Container is running with PID: %d...\n", result);

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
