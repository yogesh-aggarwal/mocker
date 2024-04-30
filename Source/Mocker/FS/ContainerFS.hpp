#pragma once

#include <string>

#include <Mocker/Core/Result.tcc>

class ContainerFS
{
   std::string       basePath;
   const std::string subPath = "containers";

public:
   ContainerFS(std::string basePath);

   std::string
   GetPath() const;

   Result<bool>
   Init() const;
};
