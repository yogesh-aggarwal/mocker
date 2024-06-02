cmake_minimum_required(VERSION 3.20)

# ----------------------------------------------------------------------------------------------------------------------
# ---- Helper Functions ------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------------------------

function(SUCCESS message)
   message("[SUCCESS] ${message}")
endfunction()

function(LINK_LIBRARY libVendorPath libName)
   add_subdirectory("Vendor/${libVendorPath}")
   target_link_libraries(${PROJECT_NAME} PRIVATE ${libName})
   include_directories("Vendor/${libVendorPath}/include")
   SUCCESS("Linked library: ${libName}")
endfunction()

# ----------------------------------------------------------------------------------------------------------------------
# ---- Compile-Time Defines --------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------------------------

set(MOCKER_DEBUG_AS_APPLICATION ON)
# set(MOCKER_DEBUG_AS_APPLICATION OFF)

if(MOCKER_DEBUG_AS_APPLICATION)
   add_compile_definitions(MOCKER_DEBUG_AS_APPLICATION)
endif()

add_compile_definitions(
   DEBUG

   # MOCKER_DISABLE_LOGGING
   # MOCKER_DISABLE_LOGGING_OUTPUT_TO_FILE
)

# ----------------------------------------------------------------------------------------------------------------------
# ---- Project Setup ---------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------------------------

project(MOCKER)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-pthread")
set(NO_DEFAULT_PATH true)
set(CMAKE_MAKE_PROGRAM "make -j 16")

# ----------------------------------------------------------------------------------------------------------------------
# ---- Files -----------------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------------------------

FILE(GLOB_RECURSE MOCKER_HEADER_FILES Source/*.hpp)
FILE(GLOB_RECURSE MOCKER_SOURCE_FILES Source/*.cpp)
FILE(GLOB_RECURSE MOCKER_TEMPLATE_FILES Source/*.tcc)

set(MOCKER_ALL_FILES ${MOCKER_HEADER_FILES} ${MOCKER_SOURCE_FILES} ${MOCKER_TEMPLATE_FILES})

# ----------------------------------------------------------------------------------------------------------------------
# ---- Product ---------------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------------------------

include_directories(Source)

if(MOCKER_DEBUG_AS_APPLICATION)
   add_executable(${PROJECT_NAME} ${MOCKER_ALL_FILES})
else()
   add_library(${PROJECT_NAME} ${MOCKER_ALL_FILES})
endif()

# ----------------------------------------------------------------------------------------------------------------------
# ---- Link Libraries --------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------------------------

message("[(---- Linking Libraries ----)]")

# CURL Library
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES})

# LibArchive Library
find_package(LibArchive REQUIRED)
include_directories(${LibArchive_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LibArchive_LIBRARIES})

# fmt Library
LINK_LIBRARY("fmt" fmt::fmt)

# yaml-cpp Library
LINK_LIBRARY("yaml-cpp" yaml-cpp::yaml-cpp)

# nanoid_cpp Library
add_subdirectory("Vendor/nanoid_cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE nanoid)
include_directories("Vendor/nanoid_cpp/inc")

message("[(---- Done Linking Libraries ----)]\n")

# ----------------------------------------------------------------------------------------------------------------------
# ---- PCHs ------------------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------------------------

FILE(GLOB_RECURSE MOCKER_PCH_FILES Source/Core/*)
target_precompile_headers(${PROJECT_NAME} PUBLIC ${MOCKER_PCH_FILES})

# ----------------------------------------------------------------------------------------------------------------------
# ---- Installation ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------------------------
set(CMAKE_INSTALL_PREFIX "/usr")
install(DIRECTORY Source/Mocker
   DESTINATION include
   FILES_MATCHING
   PATTERN "*.hpp"
   PATTERN "*.tcc")
install(TARGETS ${PROJECT_NAME}
   LIBRARY DESTINATION lib)
