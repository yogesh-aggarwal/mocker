#include "Context.hpp"

Context::Context(std::string const &root)
{
   m_FSContext = CreateRef<FSContext>(root);
}

Result<bool>
Context::Init()
{
   Result<bool> res { false };

   res = m_FSContext->Init().WithErrorHandler([](Ref<Error> error) {
      error->Push(ErrorUnit(MOCKER_FILESYSTEM_CONTEXT_INITIALIZE_FAILED,
                            "Failed to initialize FSContext"));
   });
   if (!res) return res;

   return { true };
}

Ref<FSContext>
Context::GetFSContext() const
{
   return m_FSContext;
}
