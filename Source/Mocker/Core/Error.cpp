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

   /* Print instructions on how to read the error trace */
   printf("%s", std::string(shellColumns, '-').c_str());

   char instructions[100];
   sprintf(instructions,
           "(1) = Bottom most layer   &   (%d) = Top most layer",
           static_cast<int>(m_Errors.size()));
   PrintAtCenter(shellColumns, instructions, "|*--*[", "]*--*|");

   /* Print separator */
   printf("%s", std::string(shellColumns, '-').c_str());

   /* Print block's header */
   if (title.size() > 0)
   {
      PrintAtCenter(shellColumns, title, "[", "]", false, true);
   }

   for (int i = 0; i < m_Errors.size(); i++)
   {
      const auto &error = m_Errors[i];

      int indent = 1                    // (
                   + 2                  // "00"
                   + log10(i + 1) + 1   // i + 1
                   + 1                  // )
                   + 1;                 // ' '

      printf("\n(%03d) [E:%d] %s:%d:%d\n%s\"%s\"",
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

void
Error::PrintAtCenter(int         shellColumns,
                     std::string message,
                     std::string startDecorator,
                     std::string endDecorator,
                     bool        separatorAtStart,
                     bool        separatorAtEnd) const
{
   int totalWhitespaces = shellColumns              //
                          - startDecorator.size()   //
                          - endDecorator.size()     //
                          - message.size();
   int leftWhitespaces  = totalWhitespaces / 2;
   int rightWhitespaces = totalWhitespaces - leftWhitespaces;

   if (separatorAtStart) printf("%s", std::string(shellColumns, '-').c_str());

   printf("\n%s", startDecorator.c_str());
   printf("%s", std::string(leftWhitespaces, ' ').c_str());
   printf("%s", message.c_str());
   printf("%s", std::string(rightWhitespaces, ' ').c_str());
   printf("%s\n", endDecorator.c_str());

   if (separatorAtEnd) printf("%s", std::string(shellColumns, '-').c_str());
}

//-----------------------------------------------------------------------------
