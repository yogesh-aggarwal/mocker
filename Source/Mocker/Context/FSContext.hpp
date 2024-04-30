#pragma once

#include <string>

#include <Mocker/Core/Result.tcc>
#include <Mocker/FS/ImageFS.hpp>

class FSContext
{
private:
   ImageFS m_imageFS;

public:
   FSContext(std::string const &root);

   Result<bool>
   Init();
};
