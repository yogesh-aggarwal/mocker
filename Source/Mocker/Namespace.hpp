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

	Result<bool>
	Init() const;

private:
	Result<bool>
	SetupLoggingIO() const;

	Result<bool>
	SetupUser() const;

	Result<bool>
	SetupMounting() const;

	Result<bool>
	MountVirtualFileSystem() const;

	Result<bool>
	SetupHostname() const;
};
