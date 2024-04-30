#pragma once

#include <string>

#include <Mocker/Core/Result.tcc>
#include <Mocker/FS/ImageFS.hpp>
#include <Mocker/FS/ContainerFS.hpp>

class FSContext
{
private:
   Ref<ImageFS>     m_ImageFS;
   Ref<ContainerFS> m_ContainerFS;

public:
   FSContext(std::string const &root);

   Result<bool>
   Init();

   Ref<ImageFS>
   GetImageFS() const;

   Ref<ContainerFS>
   GetContainerFS() const;
};
