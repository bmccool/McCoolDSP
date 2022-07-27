#pragma once

#ifdef WIN32
  #define McCoolDSP_EXPORT __declspec(dllexport)
#else
  #define McCoolDSP_EXPORT
#endif

McCoolDSP_EXPORT void McCoolDSP();
