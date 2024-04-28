#include "Container.hpp"

//-----------------------------------------------------------------------------

Container::Container(const Config &config, const Namespace &ns)
	 : m_Config(config), m_Namespace(ns)
{
}

//-----------------------------------------------------------------------------

void
Container::SetAlias(const std::string &alias)
{
	m_Config.alias = alias;
}

//-----------------------------------------------------------------------------

const std::string &
Container::GetAlias() const
{
	return m_Config.alias;
}

//-----------------------------------------------------------------------------
