#include "ContainerFS.hpp"

#include <Mocker/Core/Syscall.hpp>

ContainerFS::ContainerFS(std::string basePath) : basePath(std::move(basePath))
{
}

std::string
ContainerFS::GetPath() const
{
   return basePath + "/" + subPath;
}

Result<bool>
ContainerFS::Init() const
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

                error->Push(
                    { ErrorCode::MKDIR_FAILED,
                      "Failed to create directory for mocker containers: " +
                          GetPath() });
             });
   if (!res) return { false, res.error };

   return { true };
}
