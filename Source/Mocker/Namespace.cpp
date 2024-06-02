#include "Namespace.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <filesystem>

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

std::string
Namespace::GetMountPoint() const
{
   return m_Config.mountPoint;
}

//-----------------------------------------------------------------------------

std::string
Namespace::GetHostname() const
{
   return m_Config.hostname;
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::Init() const
{
   Result<bool> res { false };

   res = SetupLoggingIO().WithErrorHandler([](Ref<Error> error) {
      error->Push({ MOCKER_NAMESPACE_ERROR_SETUP_LOGGING_IO,
                    "Failed to setup logging IO" });
   });
   if (!res) return { false, res.error };

   res = SetupHostname().WithErrorHandler([](Ref<Error> error) {
      error->Push({ MOCKER_NAMESPACE_ERROR_SETUP_HOSTNAME,
                    "Failed to setup hostname" });
   });
   if (!res) return { false, res.error };

   res = SetupUser().WithErrorHandler([](Ref<Error> error) {
      error->Push(
          { MOCKER_NAMESPACE_ERROR_SETUP_USER, "Failed to setup user" });
   });
   if (!res) return { false, res.error };

   auto mountRes = SetupMounting().WithErrorHandler([](Ref<Error> error) {
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
             .WithErrorHandler([](Ref<Error> error) {
                error->Push({ DUP2_FAILED,
                              "Failed to redirect stdin to child's stdin" });
             });
   if (!res) return { false, res.error };

   // Redirect stdout to child's stdout
   res = Syscall::DUP2(STDOUT_FILENO, STDOUT_FILENO)
             .WithErrorHandler([](Ref<Error> error) {
                error->Push({ DUP2_FAILED,
                              "Failed to redirect stdout to child's stdout" });
             });
   if (!res) return { false, res.error };

   // Redirect stdout to child's stderr
   res = Syscall::DUP2(STDERR_FILENO, STDERR_FILENO)
             .WithErrorHandler([](Ref<Error> error) {
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

   // Check if the mount point already exists.
   if (!std::filesystem::exists(m_Config.mountPoint))
   {
      return { false,
               new Error {
                   { MOCKER_NAMESPACE_ERROR_SETUP_MOUNT_POINT_DOES_NOT_EXIST,
                     "Mount point does not exist" } } };
   }

   // Define the new root path
   res = Syscall::MOUNT("none", "/", NULL, MS_REC | MS_PRIVATE, NULL)
             .WithErrorHandler([](Ref<Error> error) {
                error->Push({ MOUNT_FAILED, "chroot has failed" });
             });
   if (!res) return { false, res.error };

   // Define the new root path
   res = Syscall::CHROOT(m_Config.mountPoint.c_str())
             .WithErrorHandler([](Ref<Error> error) {
                error->Push({ MOUNT_FAILED, "chroot has failed" });
             });
   if (!res) return { false, res.error };

   // Define the new root path
   res = Syscall::CHDIR("/").WithErrorHandler([](Ref<Error> error) {
      error->Push({ MOUNT_FAILED, "chdir to / has failed" });
   });
   if (!res) return { false, res.error };

   auto mountRes =
       MountVirtualFileSystem().WithErrorHandler([](Ref<Error> error) {
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

   res = Syscall::MKDIR("proc", 0755).WithErrorHandler([](Ref<Error> error) {
      if (error->Last().GetCode() == ErrorCode::MKDIR_EXISTED)
      {
         error->Clear();
         return;
      }
      error->Push({ MKDIR_FAILED, "Failed to create /proc directory" });
   });
   if (!res) return { false, res.error };

   const std::vector<std::tuple<const char *, const char *, const char *>>
       mounts = {
          { "proc", "/proc", "proc" },
          //  { "sys", "/sys", "sysfs" },
          //  { "devpts", "/dev/pts", "devpts" },
          //  { "tmpfs", "/run", "tmpfs" },
       };

   for (const auto &[source, target, fsType] : mounts)
   {
      res = Syscall::MOUNT(source, target, fsType, 0, NULL)
                .WithErrorHandler([](Ref<Error> error) {
                   if (error->Last().GetCode() ==
                       ErrorCode::OPERATION_NOT_PERMITTED)
                   {
                      error->Clear();
                      return;
                   }
                   error->Push({ MOUNT_FAILED, "Failed to mount /proc" });
                });
      if (!res) return { false, res.error };
   }

   // ------------------------------------------------------------------------

   return { true };
}

//-----------------------------------------------------------------------------

Result<bool>
Namespace::SetupHostname() const
{
   Result<int> res =
       Syscall::SETHOSTNAME(m_Config.hostname)
           .WithErrorHandler([](Ref<Error> error) {
              error->Push({ SETHOSTNAME_FAILED, "Failed to set hostname" });
           });
   if (!res) return { false, res.error };

   return { true };
}

//-----------------------------------------------------------------------------
