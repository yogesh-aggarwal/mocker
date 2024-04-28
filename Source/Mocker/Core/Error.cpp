#include "Error.hpp"

//-----------------------------------------------------------------------------

Error::Error(ErrorCode code)
	 : m_Code(code), m_Message(""), m_SubCode(ErrorCode::Undefined)
{
}

//-----------------------------------------------------------------------------

Error::Error(ErrorCode code, ErrorMessage message)
	 : m_Code(code), m_Message(message), m_SubCode(ErrorCode::Undefined)
{
}

//-----------------------------------------------------------------------------

Error::Error(ErrorCode code, ErrorCode subCode, ErrorMessage message)
	 : m_Code(code), m_SubCode(subCode), m_Message(message)
{
}

//-----------------------------------------------------------------------------

ErrorCode
Error::GetCode() const
{
	return m_Code;
}

//-----------------------------------------------------------------------------

ErrorMessage
Error::GetMessage() const
{
	return m_Message;
}

//-----------------------------------------------------------------------------

void
Error::Raise() const
{
	throw *this;
}

//-----------------------------------------------------------------------------
