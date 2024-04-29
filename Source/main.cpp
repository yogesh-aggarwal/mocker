#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/mount.h>
#include <string>
#include <vector>
#include <sys/syscall.h>
#include <sys/stat.h>

#include <Mocker/Namespace.hpp>

int
child_function(void *arg)
{
   Namespace ns { Namespace::Config {
       .mountPoint = "/home/yogesh/Desktop/rootfs",
       .hostname   = "mocker",
   } };

   auto res = ns.Init();
   if (!res)
   {
      res.error->Print("Namespace initialization failed");
      return 1;
   }

   // Retrieve the command and arguments from the passed argument
   auto *data =
       reinterpret_cast<std::pair<std::string, std::vector<std::string>> *>(
           arg);
   const std::string              &command = data->first;
   const std::vector<std::string> &args    = data->second;

   // Prepare the arguments for execv
   const char *execArgs[1 + args.size() + 1] = { command.c_str() };
   for (size_t i = 0; i < args.size(); i++)
      execArgs[i + 1] = args[i].c_str();
   execArgs[1 + args.size()] = nullptr;

   // Execute the new program using execv
   execv(command.c_str(), const_cast<char **>(execArgs));
   perror("execv failed");   // If execv returns, there was an error

   return 1;
}

int
main()
{
   // Define flags for clone: specify namespaces and process options
   int clone_flags = CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID |
                     CLONE_NEWNET | CLONE_NEWUSER | SIGCHLD;

   // Allocate a stack for the child process
   constexpr size_t stack_size = 8 * 1024 * 1024;   // 8 MB stack size
   char            *stack      = new char[stack_size];
   if (!stack)
   {
      std::cerr << "Failed to allocate stack memory" << std::endl;
      return 1;
   }

   // Prepare command and arguments for the child process
   std::string              command = "/bin/sh";
   std::vector<std::string> args    = {};
   auto                     arg     = std::make_pair(command, args);

   // Clone a new process
   int result = clone(child_function, stack + stack_size, clone_flags, &arg);

   if (result == -1)
   {
      std::cerr << "Error while cloning" << std::endl;
      delete[] stack;
      return 1;
   }

   // Optionally, wait for the child process to complete
   waitpid(result, nullptr, 0);

   // Free the allocated stack memory
   delete[] stack;

   return 0;
}
