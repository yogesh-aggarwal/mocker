#include "Syscall.hpp"

#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mount.h>
#include <memory>
#include <fmt/format.h>

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

      const std::string message =
          fmt::format("Error while creating directory {}", path);
      return Result<int> { -1, new Error({ code, message }) };
   }

   return Result<int> { ret };
}

//-----------------------------------------------------------------------------

Result<int>
Syscall::MOUNT(const std::string &source,
               const std::string &target,
               const char        *fsType,
               unsigned long      mountFlags,
               const void        *data)
{
   int ret = mount(source.c_str(), target.c_str(), fsType, mountFlags, data);
   if (ret == -1)
   {
      ErrorCode code = ErrorCode::Unknown;
      switch (errno)
      {
      case EPERM: code = ErrorCode::OPERATION_NOT_PERMITTED; break;
      case EACCES: code = ErrorCode::MOUNT_FAILED; break;
      }

      const std::string message =
          fmt::format("Error while mounting {} to {}", source, target);
      return Result<int> { -1, new Error({ code, message }) };
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
      const std::string message =
          fmt::format("Error while changing directory to {}", path);
      return Result<int> { -1, new Error({ ErrorCode::Unknown, message }) };
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
      const std::string message =
          fmt::format("Error while pivoting root to {}", new_root);
      return Result<int> { -1, new Error({ ErrorCode::Unknown, message }) };
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
      const std::string message =
          fmt::format("Error while unmounting {}", target);
      return Result<int> { -1, new Error({ ErrorCode::Unknown, message }) };
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
      const std::string message =
          fmt::format("Error while removing directory {}", path);
      return Result<int> { -1, new Error({ ErrorCode::Unknown, message }) };
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
      const std::string message =
          fmt::format("Error while duplicating file descriptor {} to {}",
                      oldfd,
                      newfd);
      return Result<int> { -1, new Error({ ErrorCode::Unknown, message }) };
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
      const std::string message =
          fmt::format("Error while setting hostname to {}", name);
      return Result<int> { -1,
                           new Error(
                               { ErrorCode::SETHOSTNAME_FAILED, message }) };
   }

   return Result<int> { ret };
}

//-----------------------------------------------------------------------------
