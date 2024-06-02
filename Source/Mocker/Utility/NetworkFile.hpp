#pragma once

#include <Mocker/Core/Result.tcc>

class NetworkFile
{
private:
   std::string m_URL;
   std::string m_DesinationPath;

public:
   NetworkFile(std::string url, std::string destinationPath);

   Result<bool>
   Fetch();
};
