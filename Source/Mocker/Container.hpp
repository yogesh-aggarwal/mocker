#pragma once

#include <string>

#include <Mocker/Namespace.hpp>

class Container
{
public:
	struct Config
	{
		std::string alias;
	};

private:
	Config    m_Config;
	Namespace m_Namespace;

public:
	Container(const Config &config, const Namespace &ns);

	void
	SetAlias(const std::string &alias);

	const std::string &
	GetAlias() const;
};
