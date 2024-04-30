#include "FSContext.hpp"

FSContext::FSContext(std::string const &root) : m_imageFS { ImageFS { root } }
{
}

Result<bool>
FSContext::Init()
{
   Result<bool> res { false };

   res = m_imageFS.Init().WithErrorHandler([](Ref<Error> error) {
      error->Push(ErrorUnit(MOCKER_IMAGE_FILESYSTEM_INITIALIZE_FAILED,
                            "Failed to initialize ImageFS"));
   });
   if (!res) return res;

   return { true };
}
