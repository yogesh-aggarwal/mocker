#include "Image.hpp"

#include <iostream>

//-----------------------------------------------------------------------------

Image::Image(Ref<FSContext> fsContext, const Config &config)
    : m_FSContext(fsContext), m_Config(config)
{
}

//-----------------------------------------------------------------------------

void
Image::SetAlias(const std::string &alias)
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
Image::SetPath(const std::string &path)
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

   std::printf("Fetching %s as \"%s\" from %s\n",
               m_Config.path.c_str(),
               m_Config.alias.c_str(),
               imageURL.c_str());

   // copy folder from /home/yogesh/Desktop/mocker_images/alpine:latest to
   // /tmp/mocker/images/alpine:latest

   return Result<bool> { true };
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
