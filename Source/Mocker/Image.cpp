#include "Image.hpp"

//-----------------------------------------------------------------------------

Image::Image(const Config &config) : m_Config(config) {}

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
