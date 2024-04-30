#include "FSContext.hpp"

FSContext::FSContext(std::string const &root)
    : m_ImageFS { CreateRef<ImageFS>(root) }, m_ContainerFS {
         CreateRef<ContainerFS>(root)
      }
{
}

Result<bool>
FSContext::Init()
{
   Result<bool> res { false };

   res = m_ImageFS->Init().WithErrorHandler([](Ref<Error> error) {
      error->Push(ErrorUnit(MOCKER_IMAGE_FILESYSTEM_INITIALIZE_FAILED,
                            "Failed to initialize ImageFS"));
   });
   if (!res) return res;

   res = m_ContainerFS->Init().WithErrorHandler([](Ref<Error> error) {
      error->Push(ErrorUnit(MOCKER_IMAGE_FILESYSTEM_INITIALIZE_FAILED,
                            "Failed to initialize ContainerFS"));
   });
   if (!res) return res;

   return { true };
}

Ref<ImageFS>
FSContext::GetImageFS() const
{
   return m_ImageFS;
}

Ref<ContainerFS>
FSContext::GetContainerFS() const
{
   return m_ContainerFS;
}
