#include "ImageFS.hpp"

#include <filesystem>

#include <Mocker/Core/Syscall.hpp>

ImageFS::ImageFS(std::string basePath) : basePath(std::move(basePath)) {}

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
                              "Failed to create directory for mocker images: " +
                                  GetPath() });
             });
   if (!res) return { false, res.error };

   return { true };
}

Result<bool>
ImageFS::DecompressToPath(const std::string &file,
                          const std::string &dest) const
{
   Result<int> res { false };

   /* Check existence of source path. If it already exists then won't be able to
    * do anything as files are not available */
   try
   {
      bool isAlreadyExists = std::filesystem::exists(file);
      if (!isAlreadyExists)
      {
         const std::string message = "File not found: " + file;
         return Result<bool> { false, new Error({ FILE_IO, message }) };
      }
   }
   catch (const std::filesystem::filesystem_error &e)
   {
      const std::string message =
          "Error while checking files' existence: " + file;
      return Result<bool> { false, new Error({ FILE_IO, message }) };
   }

   res = Syscall::MKDIR(dest, 0755).WithErrorHandler([dest](Ref<Error> error) {
      error->Push({ ErrorCode::MKDIR_FAILED,
                    "Failed to create directory for image: " + dest });
   });
   if (!res) return { false, res.error };

   return { true };
}
