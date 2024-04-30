#pragma once

#include <string>

#include <Mocker/Core/Result.tcc>

class ImageFS
{
   std::string       basePath;
   const std::string subPath = "images";

public:
   ImageFS(std::string basePath);

   std::string
   GetPath() const;

   Result<bool>
   Init() const;

   Result<bool>
   PopulateImage(const std::string &file, const std::string &dest) const;
};
