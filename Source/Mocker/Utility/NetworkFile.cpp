#include "NetworkFile.hpp"

#include <fstream>
#include <curl/curl.h>

//-----------------------------------------------------------------------------

NetworkFile::NetworkFile(std::string url, std::string destinationPath)
    : m_URL(url), m_DesinationPath(destinationPath)
{
}

//-----------------------------------------------------------------------------

// Callback function to write the data to a file
size_t
WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
   std::ofstream *file      = static_cast<std::ofstream *>(userp);
   size_t         totalSize = size * nmemb;
   file->write(static_cast<char *>(contents), totalSize);
   return totalSize;
}

Result<bool>
NetworkFile::Fetch()
{
   /* Fetch image from the repository */
   CURL *curl = curl_easy_init();
   if (curl)
   {
      std::ofstream outfile(m_DesinationPath, std::ios::binary);
      if (!outfile.is_open())
      {
         return { true,
                  new Error({ ErrorCode::FILE_IO,
                              "Could not open file: " + m_DesinationPath }) };
      }

      curl_easy_setopt(curl, CURLOPT_URL, m_URL.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);

      CURLcode res = curl_easy_perform(curl);
      if (res != CURLE_OK)
      {
         return { true,
                  new Error({ ErrorCode::MOCKER_NETWORK_FETCH_FAILED,
                              "Failed to fetch file: " + m_URL }) };
         // TODO: Handle error
         // std::cerr << "curl_easy_perform() failed: " <<
         // curl_easy_strerror(res)
         //           << std::endl;
      }

      curl_easy_cleanup(curl);
      outfile.close();
   }

   return { false, nullptr };
}

//-----------------------------------------------------------------------------
