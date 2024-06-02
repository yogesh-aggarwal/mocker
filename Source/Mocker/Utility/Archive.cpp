#include "Archive.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

Archive::Archive(std::string absolutePath)
    : m_AbsolutePath(std::move(absolutePath))
{
}

std::string
Archive::GetAbsolutePath() const
{
   return m_AbsolutePath;
}

Result<bool>
Archive::ExtractTo(const std::string &destination) const
{
   // Set extraction options
   const int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
                     ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;

   // Open the archive
   struct archive *a = archive_read_new();
   archive_read_support_format_tar(a);
   archive_read_support_filter_gzip(a);

   // Open the archive file
   int r = archive_read_open_filename(a, m_AbsolutePath.c_str(), 10240);
   if (r)
   {
      return Result<bool>(
          false,
          new Error({ ErrorCode::MOCKER_ARCHIVE_EXTRACT_FAILED_IO,
                      "Could not open archive file: " + m_AbsolutePath }));
   }

   // Create a new archive for extraction
   struct archive *ext = archive_write_disk_new();
   archive_write_disk_set_options(ext, flags);
   archive_write_disk_set_standard_lookup(ext);

   // Read entries from the archive
   struct archive_entry *entry;
   while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
   {
      const char       *currentFile = archive_entry_pathname(entry);
      const std::string currentFileAbsolutePath =
          destination + "/" + currentFile;

      // Set the new path for the extracted entry
      archive_entry_set_pathname(entry, currentFileAbsolutePath.c_str());

      // Write entry to disk
      r = archive_write_header(ext, entry);
      if (r != ARCHIVE_OK)
      {
         return Result<bool>(
             false,
             new Error({ ErrorCode::MOCKER_ARCHIVE_EXTRACT_FAILED_WRITE,
                         "Could not write archive entry: " +
                             std::string(currentFile) }));
      }

      const void *buff;
      size_t      size;
      la_int64_t  offset;

      // Read data from the archive entry and write to disk
      while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK)
      {
         archive_write_data_block(ext, buff, size, offset);
      }

      archive_write_finish_entry(ext);
   }

   // Close the archives
   archive_read_close(a);
   archive_read_free(a);
   archive_write_close(ext);
   archive_write_free(ext);

   return { false, nullptr };
}
