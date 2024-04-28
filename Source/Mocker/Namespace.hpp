#pragma once

#include <string>
#include <unistd.h>
#include <sys/mount.h>

#include <Mocker/Core/Error.hpp>
#include <Mocker/Core/Syscall.hpp>

class Namespace
{
public:
	struct Config
	{
		std::string mountPoint;
		std::string oldRootDir = "./old_root";

		std::string hostname = "mocker";

		bool
		Validate() const;
	};

private:
	Config m_Config;

public:
	Namespace(const Config &config);

	Result<void>
	Init() const;

private:
	Result<void>
	SetupLoggingIO() const;

	Result<void>
	SetupUser() const;

	Result<void>
	SetupMounting() const;

	Result<void>
	MountVirtualFileSystem() const;

	Result<void>
	SetupHostname(const std::string &hostname) const;
};
