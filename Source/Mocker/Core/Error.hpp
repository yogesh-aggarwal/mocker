#pragma once

#include <string>
#include <vector>
#include <source_location>

enum ErrorCode
{
   Undefined = -1,
   Unknown   = 0x0,

   FILE_IO        = EIO,
   FILE_NOT_FOUND = ENOENT,

   OPERATION_NOT_PERMITTED = EPERM,

   MKDIR_FAILED  = EACCES,
   MKDIR_EXISTED = EEXIST,

   MOUNT_FAILED      = EACCES,
   MOUNT_BIND_FAILED = EACCES,

   CHDIR_FAILED = EACCES,

   PIVOT_ROOT_FAILED = EACCES,

   UMOUNT2_FAILED = EACCES,

   RMDIR_FAILED = EACCES,

   DUP2_FAILED = EACCES,

   SETHOSTNAME_FAILED = EACCES,

   // Mocker's namespace errors start from 0x1*

   MOCKER_NAMESPACE_ERROR_UNKNOWN          = 0x10,
   MOCKER_NAMESPACE_ERROR_SETUP_LOGGING_IO = 0x11,
   MOCKER_NAMESPACE_ERROR_SETUP_MOUNT      = 0x12,
   MOCKER_NAMESPACE_ERROR_SETUP_USER       = 0x13,
   MOCKER_NAMESPACE_ERROR_SETUP_HOSTNAME   = 0x14,
   MOCKER_NAMESPACE_INVALID_CONFIGURATION  = 0x15,

   // Mocker's container errors start from 0x2*

   MOCKER_CONTAINER_ERROR_STACK_ALLOC  = 0x20,
   MOCKER_CONTAINER_ERROR_CLONE        = 0x21,
   MOCKER_CONTAINER_ERROR_CONFIG_PARSE = 0x22,

   // Mocker's Context errors start from 0x3*
   MOCKER_CONTEXT_INITIALIZE_FAILED            = 0x30,
   MOCKER_FILESYSTEM_CONTEXT_INITIALIZE_FAILED = 0x31,

   // Mocker's Filesystem errors start from 0x4*
   MOCKER_IMAGE_FILESYSTEM_GENERIC           = 0x40,
   MOCKER_IMAGE_FILESYSTEM_INITIALIZE_FAILED = 0x41,
};

class ErrorUnit
{
private:
   ErrorCode            m_Code;
   std::string          m_Message;
   std::source_location m_Location;

public:
   ErrorUnit(ErrorCode            code,
             std::source_location location = std::source_location::current());
   ErrorUnit(ErrorCode            code,
             std::string          message,
             std::source_location location = std::source_location::current());

   ErrorCode
   GetCode() const;

   std::string
   GetMessage() const;

   const std::source_location &
   GetLocation() const;

   operator std::string() const;
};

class Error
{
private:
   std::vector<ErrorUnit> m_Errors;

public:
   Error();
   Error(const ErrorUnit &unit);
   Error(const std::vector<ErrorUnit> &errors);

   void
   Push(const ErrorUnit &unit);

   void
   Clear();

   operator bool() const;

   const ErrorUnit &
   First() const;

   const ErrorUnit &
   Last() const;

   void
   Print(const std::string &title = "") const;

   void
   Raise() const;
};
