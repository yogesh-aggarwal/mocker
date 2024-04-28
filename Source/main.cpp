#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>         // For unshare and namespace flags
#include <sys/mount.h>     // For mount
#include <string>          // For strlen
#include <vector>          // For vector
#include <sys/syscall.h>   // For fopen, fwrite, fclose
#include <sys/stat.h>      // For fopen, fwrite, fclose

namespace Namespace
{
	int
	setup_io()
	{
		// Redirect standard input and output to the child's
		dup2(STDIN_FILENO, STDIN_FILENO);     // Redirect stdin to child's stdin
		dup2(STDOUT_FILENO, STDOUT_FILENO);   // Redirect stdout to child's stdout

		return 0;
	}

	int
	setup_mount(const std::string &target)
	{
		// Define the new root path
		if (mount(target.c_str(), target.c_str(), nullptr, MS_BIND | MS_REC | MS_PRIVATE, nullptr) ==
			 -1)
		{
			perror("mount bind failed");
			return 1;
		}

		if (chdir(target.c_str()) == -1)
		{
			perror("chdir failed");
			return 1;
		}

		// Perform pivot_root to change the root filesystem
		const std::string old_root = "./old_root";
		if (mkdir(old_root.c_str(), 0755) == -1 && errno != EEXIST)
		{
			perror("mkdir failed");
			return 1;
		}

		if (syscall(SYS_pivot_root, ".", old_root.c_str()) == -1)
		{
			perror("pivot_root failed");
			return 1;
		}

		// Change the working directory to the new root
		if (chdir("/") == -1)
		{
			perror("chdir failed");
			return 1;
		}

		// Unmount the old root
		if (umount2(old_root.c_str(), MNT_DETACH) == -1)
		{
			perror("umount2 failed");
			return 1;
		}

		// Remove the old root
		if (rmdir(old_root.c_str()) == -1)
		{
			perror("rmdir failed");
			return 1;
		}

		// Mount /proc
		mount("proc", "/proc", "proc", 0, NULL);

		// Mount /sys
		mount("sysfs", "/sys", "sysfs", 0, NULL);

		// Mount /dev
		mount("udev", "/dev", "devtmpfs", 0, NULL);

		// Mount /dev/pts
		mount("devpts", "/dev/pts", "devpts", 0, NULL);

		// Mount /dev/shm
		mount("shm", "/dev/shm", "tmpfs", 0, NULL);

		// Mount /tmp
		mount("tmpfs", "/tmp", "tmpfs", 0, NULL);

		return 0;
	}

	int
	setup_hostname(const std::string &hostname)
	{
		if (sethostname(hostname.c_str(), hostname.size()) == -1)
		{
			perror("sethostname failed");
			return 1;
		}

		return 0;
	}

	int
	setup_user()
	{
		// Set the user ID mapping
		const std::string uid_map  = "0 1000 1\n";
		const std::string gid_map  = "0 1000 1\n";
		std::string       map_path = "";

		// Set the user ID mapping
		map_path       = "/proc/self/uid_map";
		FILE *map_file = fopen(map_path.c_str(), "w");
		if (map_file == nullptr)
		{
			perror("fopen failed");
			return 1;
		}

		if (fwrite(uid_map.c_str(), 1, uid_map.length(), map_file) != uid_map.length())
		{
			perror("fwrite failed");
			fclose(map_file);
			return 1;
		}
		fclose(map_file);

		// Set the group ID mapping
		map_path = "/proc/self/gid_map";
		map_file = fopen(map_path.c_str(), "w");
		if (map_file == nullptr)
		{
			perror("fopen failed");
			return 1;
		}

		if (fwrite(gid_map.c_str(), 1, gid_map.length(), map_file) != gid_map.length())
		{
			perror("fwrite failed");
			fclose(map_file);
			return 1;
		}

		fclose(map_file);

		return 0;
	}
}

int
child_function(void *arg)
{
	// Retrieve the command and arguments from the passed argument
	auto *data = reinterpret_cast<std::pair<std::string, std::vector<std::string>> *>(arg);
	const std::string              &command = data->first;
	const std::vector<std::string> &args    = data->second;

	// Setup the new namespaces and perform setup functions
	if (Namespace::setup_io())
		return 1;
	if (Namespace::setup_user())
		return 1;
	if (Namespace::setup_mount("/home/yogesh/Desktop/rootfs"))
		return 1;
	if (Namespace::setup_hostname("sandbox"))
		return 1;

	// Prepare the arguments for execv
	const char *execArgs[1 + args.size() + 1] = { command.c_str() };
	for (size_t i = 0; i < args.size(); i++)
		execArgs[i + 1] = args[i].c_str();
	execArgs[1 + args.size()] = nullptr;

	// Execute the new program using execv
	execv(command.c_str(), const_cast<char **>(execArgs));
	perror("execv failed");   // If execv returns, there was an error

	return 1;
}

int
main()
{
	// Define flags for clone: specify namespaces and process options
	int clone_flags = CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNET |
							CLONE_NEWUSER | SIGCHLD;

	// Allocate a stack for the child process
	constexpr size_t stack_size = 8 * 1024 * 1024;   // 8 MB stack size
	char				*stack      = new char[stack_size];
	if (!stack)
	{
		std::cerr << "Failed to allocate stack memory" << std::endl;
		return 1;
	}

	// Prepare command and arguments for the child process
	std::string              command = "/bin/sh";
	std::vector<std::string> args;
	auto                     arg = std::make_pair(command, args);

	// Clone a new process
	int result = clone(child_function, stack + stack_size, clone_flags, &arg);

	if (result == -1)
	{
		std::cerr << "Error while cloning" << std::endl;
		delete[] stack;
		return 1;
	}

	// Optionally, wait for the child process to complete
	waitpid(result, nullptr, 0);

	// Free the allocated stack memory
	delete[] stack;

	return 0;
}
