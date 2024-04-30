#include <iostream>
#include <sys/wait.h>

#include <Mocker/Container.hpp>

int
main()
{
   Container container {
      Container::Config {
          .alias = "ubuntu",
      },
      Namespace { Namespace::Config {
          .mountPoint = "/home/yogesh/Desktop/ubuntu",
          .hostname   = "mocker",
      } },
   };

   auto res = container.Run();

   std::cout << container.GetAlias() << std::endl;

   wait(nullptr);

   return 0;
}
