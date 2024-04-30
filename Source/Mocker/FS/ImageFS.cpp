#include "ImageFS.hpp"

#include <Mocker/Core/Syscall.hpp>

ImageFS::ImageFS(const std::string &basePath) : basePath(basePath) {}

std::string
ImageFS::GetPath() const
{
   return basePath + "/" + subPath;
}

Result<bool>
ImageFS::Init() const
{
   Result<int> res { false };

   res = Syscall::MKDIR(basePath, 0755)
             .WithErrorHandler([this](Ref<Error> error) {
                if (error->Last().GetCode() == ErrorCode::MKDIR_EXISTED)
                {
                   error->Clear();
                   return;
                }

                error->Push({ ErrorCode::MKDIR_FAILED,
                              "Failed to create base directory for mocker " +
                                  basePath });
             });
   if (!res) return { false, res.error };

   res = Syscall::MKDIR(GetPath(), 0755)
             .WithErrorHandler([this](Ref<Error> error) {
                if (error->Last().GetCode() == ErrorCode::MKDIR_EXISTED)
                {
                   error->Clear();
                   return;
                }

                error->Push({ ErrorCode::MKDIR_FAILED,
                              "Failed to create image directory for mocker " +
                                  GetPath() });
             });
   if (!res) return { false, res.error };

   return { true };
}
