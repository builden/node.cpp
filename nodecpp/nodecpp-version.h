#pragma once
#define NODECPP_MAJOR_VERSION 0
#define NODECPP_MINOR_VERSION 1
#define NODECPP_PATCH_VERSION 0

#define NODECPP_VERSION_IS_RELEASE 0

#ifndef NODECPP_STRINGIFY
#define NODECPP_STRINGIFY(n) NODECPP_STRINGIFY_HELPER(n)
#define NODECPP_STRINGIFY_HELPER(n) #n
#endif

#ifndef NODECPP_TAG
# if NODECPP_VERSION_IS_RELEASE
#  define NODECPP_TAG ""
# else
#  define NODECPP_TAG "-pre"
# endif
#else
// NODECPP_TAG is passed without quotes when rc.exe is run from msbuild
# define NODECPP_EXE_VERSION NODECPP_STRINGIFY(NODECPP_MAJOR_VERSION) "." \
                          NODECPP_STRINGIFY(NODECPP_MINOR_VERSION) "." \
                          NODECPP_STRINGIFY(NODECPP_PATCH_VERSION)     \
                          NODECPP_STRINGIFY(NODECPP_TAG)
#endif

# define NODECPP_VERSION_STRING  NODECPP_STRINGIFY(NODECPP_MAJOR_VERSION) "." \
                              NODECPP_STRINGIFY(NODECPP_MINOR_VERSION) "." \
                              NODECPP_STRINGIFY(NODECPP_PATCH_VERSION)     \
                              NODECPP_TAG
#ifndef NODECPP_EXE_VERSION
# define NODECPP_EXE_VERSION NODECPP_VERSION_STRING
#endif

#define NODECPP_VERSION "v" NODECPP_VERSION_STRING


#define NODECPP_VERSION_AT_LEAST(major, minor, patch) \
  (( (major) < NODECPP_MAJOR_VERSION) \
  || ((major) == NODECPP_MAJOR_VERSION && (minor) < NODECPP_MINOR_VERSION) \
  || ((major) == NODECPP_MAJOR_VERSION && \
      (minor) == NODECPP_MINOR_VERSION && (patch) <= NODECPP_PATCH_VERSION))