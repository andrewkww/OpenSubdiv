#ifndef OPENSUBDIV_DEFINE_H
#define OPENSUBDIV_DEFINE_H

#if defined(WIN32)
#  ifdef OSD_DLL
#    ifdef OSD_INTERNAL
#      define OSD_API __declspec(dllexport)
#    else
#      define OSD_API __declspec(dllimport)
#    endif
#  endif
#endif

#ifndef OSD_API
#define OSD_API
#endif

#endif
