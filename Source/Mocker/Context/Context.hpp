#pragma once

#include <Mocker/Context/FSContext.hpp>

class Context
{
private:
   Ref<FSContext> m_FSContext;

public:
   Context(std::string const &root);

   Result<bool>
   Init();

   Ref<FSContext>
   GetFSContext() const;
};
