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

char *
allocateStack(size_t size)
{
   char *stack = new char[size];
   if (!stack)
   {
      std::cerr << "Failed to allocate stack memory" << std::endl;
      exit(1);
   }

   return stack;
}

void
runExecv(void *args)
{
   // Retrieve the command and arguments from the passed argument
   auto *data =
       reinterpret_cast<std::pair<std::string, std::vector<std::string>> *>(
           args);
   const std::string              &command       = data->first;
   const std::vector<std::string> &secondaryArgs = data->second;

   // Prepare the arguments for execv
   const char *execArgs[1 + secondaryArgs.size() + 1] = { command.c_str() };
   for (size_t i = 0; i < secondaryArgs.size(); i++)
      execArgs[i + 1] = secondaryArgs[i].c_str();
   execArgs[1 + secondaryArgs.size()] = nullptr;

   // Execute the new program using execv
   execv(command.c_str(), const_cast<char **>(execArgs));
   perror("execv failed");   // If execv returns, there was an error
}

int
child_function(void *args)
{
   Namespace ns { {
       .mountPoint = "/home/yogesh/Desktop/ubuntu",
       .hostname   = "mocker",
   } };

   auto res = ns.Init();
   if (!res)
   {
      res.error->Print("Namespace initialization failed");
      return 1;
   }

   runExecv(args);

   return 1;
}

int
main()
{
   // Allocate a stack for the child process
   size_t stackSize = 1024 * 1024;   // 1 MB
   char  *stack     = allocateStack(stackSize);

   auto args =
       std::make_pair<std::string, std::vector<std::string>>("/bin/sh", {});

   // Clone a new process
   int result = clone(child_function,
                      stack + stackSize,
                      SIGCHLD | CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWIPC |
                          CLONE_NEWPID | CLONE_NEWNET,
                      &args);
   if (result == -1)
   {
      std::cerr << "Error while cloning: " << errno << std::endl;
      goto cleanup;
   }

   wait(nullptr);

cleanup:
   delete[] stack;

   return 0;
}
