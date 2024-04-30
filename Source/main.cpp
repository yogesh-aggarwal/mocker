#include <iostream>
#include <sys/wait.h>

#include <Mocker/Core/Constants.hpp>
#include <Mocker/Container.hpp>
#include <Mocker/Context/Context.hpp>

int
main()
{
   Ref<Context> ctx = CreateRef<Context>("/tmp/mocker");

   auto ctxRes = ctx->Init();
   if (!ctxRes)
   {
      ctxRes.error->Print();
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;

   //--------------------------------------------------------------------------

   Ref<Image> image = CreateRef<Image>(ctx->GetFSContext(),
                                       Image::Config {
                                           .alias = "ping-ping-ping",
                                           .path  = "alpine:latest",
                                       });

   auto _ = image->Pull(IMAGE_SERVER);

   return EXIT_SUCCESS;

   Ref<Container> c = CreateRef<Container>(ctx,
                                           Container::Config {
                                               .alias = "ping-ping-ping",
                                               .image = image,
                                           });

   _ = c->Run();

   wait(nullptr);

   return 0;
}
