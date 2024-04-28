#pragma once

#include <string>

enum ErrorCode
{
	Undefined = -1,
	Unknown   = 0x0,

	FILE_IO = EIO,

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
};

using ErrorMessage = const std::string &;

class Error
{
private:
	const ErrorCode m_Code;
	const ErrorCode m_SubCode;
	ErrorMessage    m_Message;

public:
	Error(ErrorCode code);

	Error(ErrorCode code, ErrorMessage message);
	Error(ErrorCode code, const char *message);

	Error(ErrorCode code, ErrorCode subCode, ErrorMessage message);
	Error(ErrorCode code, ErrorCode subCode, const char *message);

	ErrorCode
	GetCode() const;

	ErrorMessage
	GetMessage() const;

	void
	Raise() const;
};
