#pragma once
 /**
  * By defining QUILL_DISABLE_NON_PREFIXED_MACROS before including LogMacros, we disable the
  * default 'LOG_' and then create our own macros using the global logger.
  */
#define QUILL_DISABLE_NON_PREFIXED_MACROS

#include "quill/LogMacros.h"
#include "quill/Logger.h"

  // The logger we defined in quill_wrapper.cpp
extern quill::Logger* global_logger_a;

#ifndef TRACE_PREFIX
#define TRACE_PREFIX ""
#endif

#define LOG_DEBUG(fmt, ...) QUILL_LOG_DEBUG(global_logger_a,      TRACE_PREFIX fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) QUILL_LOG_INFO(global_logger_a,        TRACE_PREFIX fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) QUILL_LOG_WARNING(global_logger_a,  TRACE_PREFIX fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) QUILL_LOG_ERROR(global_logger_a,      TRACE_PREFIX fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(global_logger_a,TRACE_PREFIX fmt, ##__VA_ARGS__)
