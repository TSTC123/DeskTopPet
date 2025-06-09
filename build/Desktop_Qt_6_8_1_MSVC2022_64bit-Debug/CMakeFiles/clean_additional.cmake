# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\DesktopPet_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\DesktopPet_autogen.dir\\ParseCache.txt"
  "DesktopPet_autogen"
  )
endif()
