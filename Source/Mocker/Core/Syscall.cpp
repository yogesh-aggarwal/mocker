#include "Syscall.hpp"

#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mount.h>
#include <memory>

#include <Mocker/Core/Error.hpp>
#include <Mocker/Core/Result.tcc>

//-----------------------------------------------------------------------------

Result<int>
Syscall::MKDIR(const std::string &path, mode_t mode)
{
	int ret = mkdir(path.c_str(), mode);
	if (ret == -1)
	{
		ErrorCode code = ErrorCode::Unknown;
		switch (errno)
		{
		case EACCES: code = ErrorCode::MKDIR_FAILED; break;
		case EEXIST: code = ErrorCode::MKDIR_EXISTED; break;
		}

		return Result<int> { -1, new Error(code) };
	}

	return Result<int> { ret };
}

//-----------------------------------------------------------------------------

Result<int>
Syscall::MOUNT(const std::string &source,
					const std::string &target,
					const std::string &filesystemtype,
					unsigned long      mountflags,
					const void        *data)
{
	int ret = mount(source.c_str(),
						 target.c_str(),
						 filesystemtype.c_str(),
						 mountflags,
						 data);
	if (ret == -1)
	{
		return Result<int> { -1, new Error(ErrorCode::Unknown) };
	}

	return Result<int> { ret };
}

//-----------------------------------------------------------------------------

Result<int>
Syscall::CHDIR(const std::string &path)
{
	int ret = chdir(path.c_str());
	if (ret == -1)
	{
		return Result<int> { -1, new Error(ErrorCode::Unknown) };
	}

	return Result<int> { ret };
}

//-----------------------------------------------------------------------------

Result<int>
Syscall::PIVOT_ROOT(const std::string &new_root, const std::string &put_old)
{
	int ret = syscall(SYS_pivot_root, new_root.c_str(), put_old.c_str());
	if (ret == -1)
	{
		return Result<int> { -1, new Error(ErrorCode::Unknown) };
	}

	return Result<int> { ret };
}

//-----------------------------------------------------------------------------

Result<int>
Syscall::UMOUNT2(const std::string &target, int flags)
{
	int ret = umount2(target.c_str(), flags);
	if (ret == -1)
	{
		return Result<int> { -1, new Error(ErrorCode::Unknown) };
	}

	return Result<int> { ret };
}

//-----------------------------------------------------------------------------

Result<int>
Syscall::RMDIR(const std::string &path)
{
	int ret = rmdir(path.c_str());
	if (ret == -1)
	{
		return Result<int> { -1, new Error(ErrorCode::Unknown) };
	}

	return Result<int> { ret };
}

//-----------------------------------------------------------------------------

Result<int>
Syscall::DUP2(int oldfd, int newfd)
{
	int ret = dup2(oldfd, newfd);
	if (ret == -1)
	{
		return Result<int> { -1, new Error(ErrorCode::Unknown) };
	}

	return Result<int> { ret };
}

//-----------------------------------------------------------------------------

Result<int>
Syscall::SETHOSTNAME(const std::string &name)
{
	int ret = sethostname(name.c_str(), name.size());
	if (ret == -1)
	{
		return Result<int> { -1, new Error(ErrorCode::SETHOSTNAME_FAILED) };
	}

	return Result<int> { ret };
}

//-----------------------------------------------------------------------------
