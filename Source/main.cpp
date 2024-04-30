#include <iostream>
#include <sys/wait.h>

#include <Mocker/Container.hpp>

int
main()
{
   Result<Ref<Container>> res { nullptr };

   res = Container::FromConfigFile("./containers/1.yml");
   if (!res)
   {
      res.error->Print();
      return 1;
   }
   auto c1 = res.value;

   auto _ = c1->Run();

   wait(nullptr);

   return 0;
}
