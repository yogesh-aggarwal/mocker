#include "Namespace.hpp"

//-----------------------------------------------------------------------------

bool
Namespace::Config::Validate() const
{
	if (mountPoint.empty() || oldRootDir.empty() || hostname.empty())
		return false;

	return true;
}

//-----------------------------------------------------------------------------

Namespace::Namespace(const Config &config) : m_Config(config)
{
	if (!m_Config.Validate())
	{
		Error error {
			MOCKER_NAMESPACE_INVALID_CONFIGURATION,
			"Invalid namespace configuration",
		};
		error.Raise();
	}
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::Init() const
{
	Result<bool> res { false };

	res = SetupLoggingIO();
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOCKER_NAMESPACE_ERROR_SETUP_LOGGING_IO,
						 "Failed to setup logging IO"),
		};
	}

	res = SetupHostname();
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOCKER_NAMESPACE_ERROR_SETUP_HOSTNAME,
						 "Failed to setup hostname"),
		};
	}

	res = SetupUser();
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOCKER_NAMESPACE_ERROR_SETUP_USER, "Failed to setup user"),
		};
	}

	res = SetupMounting();
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOCKER_NAMESPACE_ERROR_SETUP_MOUNT,
						 "Failed to setup mounting"),
		};
	}

	return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::SetupLoggingIO() const
{
	Result<int> res { false };

	// Redirect stdin to child's stdin
	res = Syscall::DUP2(STDIN_FILENO, STDIN_FILENO);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(DUP2_FAILED, "Failed to redirect stdin to child's stdin"),
		};
	}

	// Redirect stdout to child's stdout
	res = Syscall::DUP2(STDOUT_FILENO, STDOUT_FILENO);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(DUP2_FAILED, "Failed to redirect stdout to child's stdout"),
		};
	}

	return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::SetupUser() const
{
	Error *res = nullptr;

	// Set the user ID mapping
	const std::string uid_map = "0 1000 1\n";
	const std::string gid_map = "0 1000 1\n";

	std::string map_path = "";

	// Set the user ID mapping
	map_path = "/proc/self/uid_map";
	{
		FILE *map_file = fopen(map_path.c_str(), "w");
		if (map_file == nullptr)
		{
			return Result<bool> {
				new Error(MOCKER_NAMESPACE_ERROR_SETUP_USER,
							 FILE_IO,
							 "Failed to open uid_map"),
			};
		}

		if (fwrite(uid_map.c_str(), 1, uid_map.length(), map_file) !=
			 uid_map.length())
		{
			return Result<bool> {
				new Error(MOCKER_NAMESPACE_ERROR_SETUP_USER,
							 FILE_IO,
							 "Failed to write uid_map"),
			};
		}
		fclose(map_file);
	}

	// Set the group ID mapping
	map_path = "/proc/self/gid_map";
	{
		FILE *map_file = fopen(map_path.c_str(), "w");
		if (map_file == nullptr)
		{
			return Result<bool> {
				new Error(MOCKER_NAMESPACE_ERROR_SETUP_USER,
							 FILE_IO,
							 "Failed to open gid_map"),
			};
		}

		if (fwrite(gid_map.c_str(), 1, gid_map.length(), map_file) !=
			 gid_map.length())
		{
			return Result<bool> {
				new Error(MOCKER_NAMESPACE_ERROR_SETUP_USER,
							 FILE_IO,
							 "Failed to write gid_map"),
			};
		}

		fclose(map_file);
	}

	return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::SetupMounting() const
{
	Result<int> res { false };

	// Define the new root path
	res = Syscall::MOUNT("none", "/", nullptr, MS_REC | MS_PRIVATE, nullptr);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOUNT_FAILED, "mount MS_PRIVATE on / has failed"),
		};
	}

	// Define the new root path
	res = Syscall::MOUNT(m_Config.mountPoint.c_str(),
								m_Config.mountPoint.c_str(),
								nullptr,
								MS_BIND | MS_REC | MS_PRIVATE,
								nullptr);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOUNT_FAILED, "Failed to mount bind"),
		};
	}

	// Change the working directory to the new root
	res = Syscall::CHDIR(m_Config.mountPoint.c_str());
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(CHDIR_FAILED, "Failed to change directory"),
		};
	}

	// Perform pivot_root to change the root filesystem
	res = Syscall::MKDIR(m_Config.oldRootDir.c_str(), 0755);
	if (!res && res.error->GetCode() != ErrorCode::MKDIR_EXISTED)
	{
		return Result<bool> {
			false,
			new Error(MKDIR_FAILED, "Failed to create old root directory"),
		};
	}

	res = Syscall::PIVOT_ROOT(".", m_Config.oldRootDir.c_str());
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(PIVOT_ROOT_FAILED, "Failed to pivot root"),
		};
	}

	// Change the working directory to the new root
	res = Syscall::CHDIR("/");
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(CHDIR_FAILED, "Failed to change directory"),
		};
	}

	// Unmount the old root
	res = Syscall::UMOUNT2(m_Config.oldRootDir.c_str(), MNT_DETACH);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(UMOUNT2_FAILED, "Failed to unmount old root"),
		};
	}

	// Remove the old root
	res = Syscall::RMDIR(m_Config.oldRootDir.c_str());
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(RMDIR_FAILED, "Failed to remove old root"),
		};
	}

	if (!MountVirtualFileSystem())
	{
		return Result<bool> {
			false,
			new Error(MOUNT_FAILED, "Failed to mount virtual file system"),
		};
	}

	return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::MountVirtualFileSystem() const
{
	Result<int> res { false };

	// ------------------------------------------------------------------------
	// Mount /proc
	// ------------------------------------------------------------------------

	res = Syscall::MKDIR("proc", 0755);
	if (!res && res.error->GetCode() != ErrorCode::MKDIR_EXISTED)
	{
		return Result<bool> {
			false,
			new Error(MKDIR_FAILED, "Failed to create /proc directory"),
		};
	}
	res = Syscall::MOUNT("proc", "/proc", "proc", 0, NULL);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOUNT_FAILED, "Failed to mount /proc"),
		};
	}

	// ------------------------------------------------------------------------
	// Mount /sys
	// ------------------------------------------------------------------------

	res = Syscall::MOUNT("sysfs", "/sys", "sysfs", 0, NULL);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOUNT_FAILED, "Failed to mount /sys"),
		};
	}

	// ------------------------------------------------------------------------
	// Mount /dev
	// ------------------------------------------------------------------------

	res = Syscall::MOUNT("udev", "/dev", "devtmpfs", 0, NULL);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOUNT_FAILED, "Failed to mount /dev"),
		};
	}

	// ------------------------------------------------------------------------
	// Mount /dev/pts
	// ------------------------------------------------------------------------

	res = Syscall::MOUNT("devpts", "/dev/pts", "devpts", 0, NULL);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOUNT_FAILED, "Failed to mount /dev/pts"),
		};
	}

	// ------------------------------------------------------------------------
	// Mount /dev/shm
	// ------------------------------------------------------------------------

	res = Syscall::MOUNT("tmpfs", "/dev/shm", "tmpfs", 0, NULL);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(MOUNT_FAILED, "Failed to mount /dev/shm"),
		};
	}

	// ------------------------------------------------------------------------

	return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::SetupHostname() const
{
	Result<int> res = Syscall::SETHOSTNAME(m_Config.hostname);
	if (!res)
	{
		return Result<bool> {
			false,
			new Error(SETHOSTNAME_FAILED, "Failed to set hostname"),
		};
	}

	return { true };
}

//-----------------------------------------------------------------------------
