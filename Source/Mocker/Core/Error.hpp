#pragma once

#include <string>
#include <vector>
#include <source_location>

enum ErrorCode
{
   /* Generic Error Codes */

   Undefined = -1,
   Unknown   = 0x00,

   /* Syscalls Error Codes */

   FILE_IO             = EIO,
   FILE_NOT_FOUND      = ENOENT,
   FILE_ALREADY_EXISTS = EEXIST,

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

   /* Reset the enum counter */

   MOCKER_NAMESPACE_ERROR = 1 << 24,

   /* Utility Error Codes */

   MOCKER_ARCHIVE_EXTRACT_FAILED,
   MOCKER_ARCHIVE_EXTRACT_FAILED_IO,
   MOCKER_ARCHIVE_EXTRACT_FAILED_UNKNOWN,
   MOCKER_ARCHIVE_EXTRACT_FAILED_WRITE,

   /* Network Error Codes */
   MOCKER_NETWORK_CONNECTION_FAILED,
   MOCKER_NETWORK_FETCH_FAILED,

   /* Namespace Error Codes */

   MOCKER_NAMESPACE_ERROR_UNKNOWN,
   MOCKER_NAMESPACE_ERROR_SETUP_LOGGING_IO,
   MOCKER_NAMESPACE_ERROR_SETUP_MOUNT,
   MOCKER_NAMESPACE_ERROR_SETUP_MOUNT_POINT_DOES_NOT_EXIST,
   MOCKER_NAMESPACE_ERROR_SETUP_USER,
   MOCKER_NAMESPACE_ERROR_SETUP_HOSTNAME,
   MOCKER_NAMESPACE_INVALID_CONFIGURATION,

   /* Container Error Codes */

   MOCKER_CONTAINER_ERROR_UNKNOWN,
   MOCKER_CONTAINER_TERMINATE_ERROR_FAILED,
   MOCKER_CONTAINER_TERMINATE_ERROR_NOT_RUNNING,
   MOCKER_CONTAINER_RUN_FAILED,
   MOCKER_CONTAINER_ERROR_CREATING_MOUNT_POINT,
   MOCKER_CONTAINER_ERROR_POPULATING_MOUNT_POINT,
   MOCKER_CONTAINER_ERROR_STACK_ALLOC,
   MOCKER_CONTAINER_ERROR_CLONE,
   MOCKER_CONTAINER_ERROR_CONFIG_PARSE,
   MOCKER_CONTAINER_ERROR_ALREADY_RUNNING,
   MOCKER_CONTAINER_ERROR_NOT_RUNNING,

   /* Image Error Codes */
   MOCKER_IMAGE_ERROR_UNKNOWN,
   MOCKER_IMAGE_ERROR_PULL_FAILED,
   MOCKER_IMAGE_ERROR_POPULATE_FAILED,
   MOCKER_IMAGE_ERROR_REMOVE_FAILED,
   MOCKER_IMAGE_ERROR_REPLICATE_FAILED,
   MOCKER_IMAGE_ERROR_REPLICATE_FAILED_COPY_FAILED,
   MOCKER_IMAGE_ERROR_REPLICATE_FAILED_DESTINATION_NOT_FOUND,
   MOCKER_IMAGE_ERROR_REPLICATE_FAILED_IMAGE_NOT_FOUND,

   /* Context Error Codes */

   MOCKER_CONTEXT_INITIALIZE_FAILED,
   MOCKER_FILESYSTEM_CONTEXT_INITIALIZE_FAILED,

   /* Context Filesystem Error Codes */

   MOCKER_IMAGE_FILESYSTEM_GENERIC,
   MOCKER_IMAGE_FILESYSTEM_INITIALIZE_FAILED,
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

private:
   void
   PrintAtCenter(int         shellColumns,
                 std::string message,
                 std::string startDecorator   = "",
                 std::string endDecorator     = "",
                 bool        separatorAtStart = false,
                 bool        separatorAtEnd   = false) const;
};
