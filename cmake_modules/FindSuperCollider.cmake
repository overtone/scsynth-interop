# - Find SuperCollider (a cross platform RPC lib/tool)
# This module defines
# SuperCollider_INCLUDE_DIR, where to find SuperCollider headers
# SuperCollider_LIBS, SuperCollider libraries
# SuperCollider_FOUND, If false, do not try to use SuperCollider

find_path(SuperCollider_INCLUDE_DIR SuperCollider/plugin_interface/SC_PlugIn.h PATHS
    /usr/local/include
    /opt/local/include
  )

#find_library can't seem to find a 64-bit binary if the 32-bit isn't there

set(SuperCollider_LIB_PATHS /usr/local/lib /opt/local/lib /usr/lib64)
find_library(SuperCollider_LIB NAMES scsynth PATHS ${SuperCollider_LIB_PATHS})

if (SuperCollider_LIB AND SuperCollider_INCLUDE_DIR)
  set(SuperCollider_FOUND TRUE)
  set(SuperCollider_LIBS ${SuperCollider_LIB})
else ()
  set(SuperCollider_FOUND FALSE)
endif ()

if (SuperCollider_FOUND)
  if (NOT SuperCollider_FIND_QUIETLY)
    message(STATUS "Found SuperCollider: ${SuperCollider_LIBS}")
  endif ()
else ()
  if (SuperCollider_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find SuperCollider library.")
  endif ()
  message(STATUS "SuperCollider NOT found.")
endif ()

mark_as_advanced(
    SuperCollider_LIB
    SuperCollider_INCLUDE_DIR
  )
