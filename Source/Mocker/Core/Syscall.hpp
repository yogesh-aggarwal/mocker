#pragma once

#include <Mocker/Core/Result.tcc>

namespace Syscall
{
   Result<int>
   MKDIR(const std::string &path, mode_t mode);

   Result<int>
   MOUNT(const std::string &source,
         const std::string &target,
         const char        *fsType,
         unsigned long      mountFlags,
         const void        *data);

   Result<int>
   CHDIR(const std::string &path);

   Result<int>
   CHROOT(const std::string &path);

   Result<int>
   PIVOT_ROOT(const std::string &new_root, const std::string &put_old);

   Result<int>
   UMOUNT2(const std::string &target, int flags);

   Result<int>
   RMDIR(const std::string &path);

   Result<int>
   DUP2(int oldfd, int newfd);

   Result<int>
   SETHOSTNAME(const std::string &name);
}
