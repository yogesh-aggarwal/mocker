#include "Archive.hpp"

Archive::Archive(std::string absolutePath)
    : m_AbsolutePath(std::move(absolutePath))
{
}

std::string
Archive::GetAbsolutePath() const
{
   return m_AbsolutePath;
}

Result<bool>
Archive::ExtractTo(const std::string &destination) const
{
   return Result<bool>(true,
                       new Error({ ErrorCode::Unknown, "Not implemented" }));
}
