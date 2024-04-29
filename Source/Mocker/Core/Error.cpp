#include "Error.hpp"

#include <cmath>
#include <iostream>
#include <string>

#include <Mocker/Core/Helpers.hpp>

//-----------------------------------------------------------------------------

ErrorUnit::ErrorUnit(ErrorCode code, std::source_location location)
    : m_Code(code), m_Message(""), m_Location(location)
{
}

//-----------------------------------------------------------------------------

ErrorUnit::ErrorUnit(ErrorCode            code,
                     std::string          message,
                     std::source_location location)
    : m_Code(code), m_Message(std::move(message)), m_Location(location)
{
}

//-----------------------------------------------------------------------------

ErrorCode
ErrorUnit::GetCode() const
{
   return m_Code;
}

//-----------------------------------------------------------------------------

std::string
ErrorUnit::GetMessage() const
{
   return m_Message;
}

//-----------------------------------------------------------------------------

const std::source_location &
ErrorUnit::GetLocation() const
{
   return m_Location;
}

//-----------------------------------------------------------------------------

ErrorUnit::operator std::string() const { return m_Message; }

//-----------------------------------------------------------------------------

Error::Error() : m_Errors() {}

//-----------------------------------------------------------------------------

Error::Error(const ErrorUnit &unit) { m_Errors.push_back(unit); }

//-----------------------------------------------------------------------------

Error::Error(const std::vector<ErrorUnit> &errors) : m_Errors(errors) {}

//-----------------------------------------------------------------------------

void
Error::Push(const ErrorUnit &unit)
{
   m_Errors.push_back(unit);
}

//-----------------------------------------------------------------------------

void
Error::Clear()
{
   m_Errors.clear();
}

//-----------------------------------------------------------------------------

Error::operator bool() const { return m_Errors.size() > 0; }

//-----------------------------------------------------------------------------

const ErrorUnit &
Error::First() const
{
   return m_Errors[0];
}

//-----------------------------------------------------------------------------

const ErrorUnit &
Error::Last() const
{
   return m_Errors[m_Errors.size() - 1];
}

//-----------------------------------------------------------------------------

void
Error::Print(const std::string &title) const
{
   // struct winsize ws;
   // int            fd;
   // fd = open("/dev/tty", O_RDWR);
   // if (fd < 0 || ioctl(fd, TIOCGWINSZ, &ws) < 0) err(8, "/dev/tty");
   // const int shellColumns = ws.ws_col;
   // close(fd);

   const int shellColumns = 80;

   printf("%s", std::string(shellColumns, '-').c_str());
   if (title.size() > 0)
   {
      printf("\n| %s", title.c_str());
      printf("%s", std::string(shellColumns - title.size() - 3, ' ').c_str());
      printf("|\n");
      printf("%s", std::string(shellColumns, '-').c_str());
   }

   for (int i = 0; i < m_Errors.size(); i++)
   {
      const auto &error = m_Errors[i];

      int indent = 1                    // (
                   + log10(i + 1) + 1   // i + 1
                   + 1                  // )
                   + 1;                 // ' '

      printf("\n(%d) [0x%04X] %s:%d:%d\n%s\"%s\"",
             i + 1,
             error.GetCode(),
             error.GetLocation().file_name(),
             error.GetLocation().line(),
             error.GetLocation().column(),
             std::string(indent, ' ').c_str(),
             //  "error str"
             error.GetMessage().c_str());
   }
   printf("\n%s\n", std::string(shellColumns, '-').c_str());
}

//-----------------------------------------------------------------------------

void
Error::Raise() const
{
   throw *this;
}

//-----------------------------------------------------------------------------
