#include "Namespace.hpp"

#include <fcntl.h>
#include <unistd.h>

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
      Error error { { MOCKER_NAMESPACE_INVALID_CONFIGURATION,
                      "Invalid namespace configuration" } };
      error.Raise();
   }
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::Init() const
{
   Result<bool> res { false };

   res = SetupLoggingIO().WithErrorHandler([](auto error) {
      error->Push({ MOCKER_NAMESPACE_ERROR_SETUP_LOGGING_IO,
                    "Failed to setup logging IO" });
   });
   if (!res) return { false, res.error };

   res = SetupHostname().WithErrorHandler([](auto error) {
      error->Push({ MOCKER_NAMESPACE_ERROR_SETUP_HOSTNAME,
                    "Failed to setup hostname" });
   });
   if (!res) return { false, res.error };

   res = SetupUser().WithErrorHandler([](auto error) {
      error->Push(
          { MOCKER_NAMESPACE_ERROR_SETUP_USER, "Failed to setup user" });
   });
   if (!res) return { false, res.error };

   auto mountRes = SetupMounting().WithErrorHandler([](auto error) {
      error->Push(
          { MOCKER_NAMESPACE_ERROR_SETUP_MOUNT, "Failed to setup mounting" });
   });
   if (!mountRes) return { false, mountRes.error };

   return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::SetupLoggingIO() const
{
   Result<int> res { false };

   // int f = open("./output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);

   // Redirect stdin to child's stdin
   res = Syscall::DUP2(STDIN_FILENO, STDIN_FILENO)
             .WithErrorHandler([](auto error) {
                error->Push({ DUP2_FAILED,
                              "Failed to redirect stdin to child's stdin" });
             });
   if (!res) return { false, res.error };

   // Redirect stdout to child's stdout
   res = Syscall::DUP2(STDOUT_FILENO, STDOUT_FILENO)
             .WithErrorHandler([](auto error) {
                error->Push({ DUP2_FAILED,
                              "Failed to redirect stdout to child's stdout" });
             });
   if (!res) return { false, res.error };

   // Redirect stdout to child's stderr
   res = Syscall::DUP2(STDERR_FILENO, STDERR_FILENO)
             .WithErrorHandler([](auto error) {
                error->Push({ DUP2_FAILED,
                              "Failed to redirect stderr to child's stderr" });
             });
   if (!res) return { false, res.error };

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
         Error *err = new Error();
         err->Push(
             { MOCKER_NAMESPACE_ERROR_SETUP_USER, "Failed to open uid_map" });
         err->Push({ FILE_IO, "Failed to open uid_map" });
         return Result<bool> { false, err };
      }

      if (fwrite(uid_map.c_str(), 1, uid_map.length(), map_file) !=
          uid_map.length())
      {
         Error *err = new Error();
         err->Push(
             { MOCKER_NAMESPACE_ERROR_SETUP_USER, "Failed to open uid_map" });
         err->Push({ FILE_IO, "Failed to write uid_map" });
         return Result<bool> { false, err };
      }

      fclose(map_file);
   }

   // Set the group ID mapping
   map_path = "/proc/self/gid_map";
   {
      FILE *map_file = fopen(map_path.c_str(), "w");
      if (map_file == nullptr)
      {
         Error *err = new Error();
         err->Push(
             { MOCKER_NAMESPACE_ERROR_SETUP_USER, "Failed to open gid_map" });
         err->Push({ FILE_IO, "Failed to open gid_map" });
         return Result<bool> { false, err };
      }

      if (fwrite(gid_map.c_str(), 1, gid_map.length(), map_file) !=
          gid_map.length())
      {
         Error *err = new Error();
         err->Push(
             { MOCKER_NAMESPACE_ERROR_SETUP_USER, "Failed to open gid_map" });
         err->Push({ FILE_IO, "Failed to write gid_map" });
         return Result<bool> { false, err };
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
   res =
       Syscall::MOUNT("none", "/", nullptr, MS_REC | MS_PRIVATE, nullptr)
           .WithErrorHandler([](auto error) {
              error->Push({ MOUNT_FAILED, "mount MS_PRIVATE on / has failed" });
           });
   if (!res) return { false, res.error };

   // Define the new root path
   res = Syscall::MOUNT(m_Config.mountPoint.c_str(),
                        m_Config.mountPoint.c_str(),
                        nullptr,
                        MS_BIND | MS_REC | MS_PRIVATE,
                        nullptr)
             .WithErrorHandler([](auto error) {
                error->Push({ MOUNT_FAILED, "mount MS_BIND on / has failed" });
             });
   if (!res) return { false, res.error };

   // Change the working directory to the new root
   res = Syscall::CHDIR(m_Config.mountPoint.c_str())
             .WithErrorHandler([](auto error) {
                error->Push({ CHDIR_FAILED, "Failed to change directory" });
             });
   if (!res) return { false, res.error };

   // Perform pivot_root to change the root filesystem
   res = Syscall::MKDIR(m_Config.oldRootDir.c_str(), 0755)
             .WithErrorHandler([](auto error) {
                if (error->Last().GetCode() == ErrorCode::MKDIR_EXISTED)
                {
                   error->Clear();
                   return;
                }
                error->Push(
                    { MKDIR_FAILED, "Failed to create old root directory" });
             });
   if (!res) return { false, res.error };

   res = Syscall::PIVOT_ROOT(".", m_Config.oldRootDir.c_str())
             .WithErrorHandler([](auto error) {
                error->Push({ PIVOT_ROOT_FAILED, "Failed to pivot root" });
             });
   if (!res) return { false, res.error };

   // Change the working directory to the new root
   res = Syscall::CHDIR("/").WithErrorHandler([](auto error) {
      error->Push({ CHDIR_FAILED, "Failed to change directory" });
   });
   if (!res) return { false, res.error };

   // Unmount the old root
   res = Syscall::UMOUNT2(m_Config.oldRootDir.c_str(), MNT_DETACH)
             .WithErrorHandler([](auto error) {
                error->Push({ UMOUNT2_FAILED, "Failed to unmount old root" });
             });
   if (!res) return { false, res.error };

   // Remove the old root
   res = Syscall::RMDIR(m_Config.oldRootDir.c_str())
             .WithErrorHandler([](auto error) {
                error->Push({ RMDIR_FAILED, "Failed to remove old root" });
             });
   if (!res) return { false, res.error };

   auto mountRes = MountVirtualFileSystem().WithErrorHandler([](auto error) {
      error->Push({ MOUNT_FAILED, "Failed to mount virtual file system" });
   });
   if (!mountRes) return mountRes;

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

   res = Syscall::MKDIR("proc", 0755).WithErrorHandler([](auto error) {
      if (error->Last().GetCode() == ErrorCode::MKDIR_EXISTED)
      {
         error->Clear();
         return;
      }
      error->Push({ MKDIR_FAILED, "Failed to create /proc directory" });
   });
   if (!res) return { false, res.error };

   res =
       Syscall::MOUNT("proc", "/proc", "proc", 0, NULL)
           .WithErrorHandler([](auto error) {
              if (error->Last().GetCode() == ErrorCode::OPERATION_NOT_PERMITTED)
              {
                 error->Clear();
                 return;
              }
              error->Push({ MOUNT_FAILED, "Failed to mount /proc" });
           });
   if (!res) return { false, res.error };

   // ------------------------------------------------------------------------
   // Mount /sys
   // ------------------------------------------------------------------------

   res =
       Syscall::MOUNT("sysfs", "/sys", "sysfs", 0, NULL)
           .WithErrorHandler([](auto error) {
              if (error->Last().GetCode() == ErrorCode::OPERATION_NOT_PERMITTED)
              {
                 error->Clear();
                 return;
              }
              error->Push({ MOUNT_FAILED, "Failed to mount /sys" });
           });
   if (!res) return { false, res.error };

   // ------------------------------------------------------------------------
   // Mount /dev
   // ------------------------------------------------------------------------

   res =
       Syscall::MOUNT("udev", "/dev", "devtmpfs", 0, NULL)
           .WithErrorHandler([](auto error) {
              if (error->Last().GetCode() == ErrorCode::OPERATION_NOT_PERMITTED)
              {
                 error->Clear();
                 return;
              }
              error->Push({ MOUNT_FAILED, "Failed to mount /dev" });
           });
   if (!res) return { false, res.error };

   // ------------------------------------------------------------------------

   return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::SetupHostname() const
{
   Result<int> res =
       Syscall::SETHOSTNAME(m_Config.hostname).WithErrorHandler([](auto error) {
          error->Push({ SETHOSTNAME_FAILED, "Failed to set hostname" });
       });
   if (!res) return { false, res.error };

   return { true };
}

//-----------------------------------------------------------------------------
