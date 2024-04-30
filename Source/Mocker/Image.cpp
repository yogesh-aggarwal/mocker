#include "Image.hpp"

#include <iostream>
#include <filesystem>

//-----------------------------------------------------------------------------

Image::Image(Ref<FSContext> fsContext, const Config &config)
    : m_FSContext(fsContext), m_Config(config)
{
}

//-----------------------------------------------------------------------------

void
Image::SetAlias(std::string alias)
{
   m_Config.alias = alias;
}

//-----------------------------------------------------------------------------

const std::string &
Image::GetAlias() const
{
   return m_Config.alias;
}

//-----------------------------------------------------------------------------

void
Image::SetPath(std::string path)
{
   m_Config.path = path;
}

//-----------------------------------------------------------------------------

const std::string &
Image::GetPath() const
{
   return m_Config.path;
}

//-----------------------------------------------------------------------------

Result<bool>
Image::CheckExists() const
{
   return Result<bool> { false };
}

//-----------------------------------------------------------------------------

Result<bool>
Image::Pull(const std::string &repository) const
{
   const std::string imageURL    = repository + "/" + m_Config.path;
   const std::string metadataURL = imageURL + "/metadata.json";

   std::printf("Fetching %s as \"%s\" from %s%s\n",
               m_Config.path.c_str(),
               m_Config.alias.c_str(),
               imageURL.c_str(),
               ".tar.gz");

   std::filesystem::path srcPath { "/home/yogesh/Desktop/mocker_images/" +
                                   m_Config.path + ".tar.gz" };
   std::filesystem::path destPath { m_FSContext->GetImageFS()->GetPath() + "/" +
                                    m_Config.path };

   /* Check existence of destination path. If it already exists then we don't
    * need to do anything */
   try
   {
      bool isAlreadyExists = std::filesystem::exists(destPath);
#ifdef DEBUG
      if (isAlreadyExists) { std::filesystem::remove_all(destPath); }
#else
      if (isAlreadyExists) return Result<bool> { true };
#endif
   }
   catch (const std::filesystem::filesystem_error &e)
   {
      return Result<bool> {
         false,
         new Error({ FILE_IO, "Error while checking image files' existence" })
      };
   }

   /* Extract Image contents to destination dir */
   auto res = m_FSContext->GetImageFS()
                  ->PopulateImage(srcPath.string(), destPath.string())
                  .WithErrorHandler([](Ref<Error> error) {
                     error->Push({ MOCKER_IMAGE_ERROR_POPULATE_FAILED,
                                   "Failed to extract image" });
                  });
   return res;
}

//-----------------------------------------------------------------------------

Result<bool>
Image::Remove() const
{
   return Result<bool> { false };
}

//-----------------------------------------------------------------------------

Result<bool>
Image::Replicate(const std::string &destination) const
{
   return Result<bool> { false };
}

//-----------------------------------------------------------------------------
