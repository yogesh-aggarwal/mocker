#pragma once

#include <string>

#include <Mocker/Core/Result.tcc>

class Archive
{
private:
   std::string m_AbsolutePath;

public:
   Archive(std::string absolutePath);

   std::string
   GetAbsolutePath() const;

   Result<bool>
   ExtractTo(const std::string &destination) const;
};
