#include <iostream>
#include <sys/wait.h>

#include <Mocker/Core/Constants.hpp>
#include <Mocker/Container.hpp>
#include <Mocker/Context/Context.hpp>

int
main()
{
   system("clear");

   Ref<Context> ctx = CreateRef<Context>("/tmp/mocker");

   auto ctxRes = ctx->Init();
   if (!ctxRes)
   {
      ctxRes.error->Print();
      return EXIT_FAILURE;
   }

   Ref<Image> image = CreateRef<Image>(ctx->GetFSContext(),
                                       Image::Config {
                                           .alias = "ping-ping-ping",
                                           .path  = "alpine",
                                       });

   auto _ = image->Pull(IMAGE_SERVER);
   if (!_)
   {
      _.error->Print();
      return EXIT_FAILURE;
   }

   Ref<Container> c = CreateRef<Container>(ctx,
                                           Container::Config {
                                               .alias = "ping-ping-ping",
                                               .image = image,
                                           });

   _ = c->Run();
   if (!_)
   {
      _.error->Print();
      return EXIT_FAILURE;
   }

   wait(nullptr);

   return 0;
}
