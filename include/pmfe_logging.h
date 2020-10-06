/* pmfe_logging.h : A self-contained set of boost::log configuration routines to 
 *                  facilitate viewing live logs for debugging and troubleshooting;
 * Author:  Maxie D. Schmidt (github.com/maxieds)
 * Created: 2020.10.05
 */

#ifndef __PMFE_LOCAL_LOGGING_CONFIG_H__
#define __PMFE_LOCAL_LOGGING_CONFIG_H__

#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <string>
#include <iostream>
#include <ctime>
#include <locale>

#pragma once
#define BOOST_LOG_DYN_LINK 1 // enables linking the boost_log library dynamically

#include <boost/core/null_deleter.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_unique.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/date_facet.hpp>
#include <boost/log/utility/string_literal.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/expressions/predicates/is_debugger_present.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp> 
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/utility/setup/from_settings.hpp>
#include <boost/log/attributes/function.hpp>

#ifndef __BOOST_LOCAL_NAMESPACE_DEFINES__
     #define __BOOST_LOCAL_NAMESPACE_DEFINES__
     namespace BoostLogger   = boost::log;
     namespace BoostSrc      = boost::log::sources;
     namespace BoostSink     = boost::log::sinks;
     namespace BoostKeywords = boost::log::keywords;
     namespace BoostAttrs    = boost::log::attributes;
     namespace BoostFS       = boost::filesystem;
#endif

BOOST_LOG_GLOBAL_LOGGER(logger, BoostSrc::severity_logger_mt< boost::log::trivial::severity_level >)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "Timestamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", int)
BOOST_LOG_ATTRIBUTE_KEYWORD(__loggerAttr_SrcFileName, "SourceFileName", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(__loggerAttr_SrcLineNumber, "SourceLineNumber", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(__loggerAttr_SrcFileFunc, "SourceFileFunc", std::string)

namespace PMFELocalLogging {

     const std::string PMFE_DEBUG_ENVNAME       = "PMFE_DEBUG";
     const std::string PMFE_LOGFILE_DEFAULT_DIR = "~/.pmfe-logger";
     const std::string PMFE_LOGFILE_DEFAULT_FILE = "PMFERuntimeOutput-%Y.%m.%d@%H:%M:%S.log";

     typedef enum {
          BOOST_LOG_DEFAULT = 0,
          BOOST_LOG_TRACE   = 1,
          BOOST_LOG_DEBUG   = 2,
          BOOST_LOG_INFO    = 3,
          BOOST_LOG_WARNING = 4,
          BOOST_LOG_ERROR   = 5,
          BOOST_LOG_FATAL   = 6
     } BoostLoggerSeverityLevel_t;

     static inline BoostLogger::trivial::severity_level GetBoostLoggerLevel(BoostLoggerSeverityLevel_t sevLevelKeyword) {
          switch(sevLevelKeyword) {
               case BOOST_LOG_TRACE:
                    return BoostLogger::trivial::trace;
               case BOOST_LOG_DEBUG:
                    return BoostLogger::trivial::debug;
               case BOOST_LOG_INFO:
                    return BoostLogger::trivial::info;
               case BOOST_LOG_WARNING:
                    return BoostLogger::trivial::warning;
               case BOOST_LOG_ERROR:
                    return BoostLogger::trivial::error;
               case BOOST_LOG_FATAL:
                    return BoostLogger::trivial::fatal;
               case BOOST_LOG_DEFAULT: /* Fall through */
               default:
                    break;
          }
          return BoostLogger::trivial::debug;
     }

     static inline std::string GetExpandedFilePath(std::string symbolicFilePath, bool mkdirIfNotExists = false) {
          const char *envHomePath = std::getenv("HOME");
          if(envHomePath != NULL) {
               std::string homeDirPath = std::string(envHomePath);
               unsigned int nextHomeRelativeMarker = symbolicFilePath.find_first_of("~");
               while(nextHomeRelativeMarker != std::string::npos) {
                    symbolicFilePath.replace(symbolicFilePath.begin() + nextHomeRelativeMarker, 
                                             symbolicFilePath.begin() + nextHomeRelativeMarker + 1, 
                                             homeDirPath.begin(), homeDirPath.end());
                    nextHomeRelativeMarker = symbolicFilePath.find_first_of("~");
               }
          }
          BoostFS::path actualFilePath = BoostFS::path(symbolicFilePath);
          if(mkdirIfNotExists && !BoostFS::exists(actualFilePath)) {
               BoostFS::create_directories(actualFilePath);
          }
          return BoostFS::canonical(actualFilePath).string();
     }

     static inline std::string GetTimeStampedFilePath(std::string fmtFilePath, bool expandPath = true) {
          std::locale::global(std::locale("en_US.utf8"));
          std::time_t tm = std::time(nullptr);
          char tempStampedFileBuf[256];
          std::strftime(tempStampedFileBuf, 256, fmtFilePath.c_str(), std::localtime(&tm));
          tempStampedFileBuf[255] = '\0';
          std::string stampedFilePath = std::string(tempStampedFileBuf);
          if(expandPath) {
               stampedFilePath = GetExpandedFilePath(stampedFilePath);
          }
          return stampedFilePath;
     }

     static inline std::string GetFullLoggingFileName(std::string fmtDirPath, 
                                                      std::string fmtFilePath, 
                                                      bool expandPath = true) {
          return GetExpandedFilePath(fmtDirPath) + "/" + GetTimeStampedFilePath(fmtFilePath, expandPath);
     }

     typedef enum {
          ANSI_COLOR_OFF           = -1,
          ANSI_COLOR_NORMAL        = 0,
          ANSI_COLOR_BACKGROUND    = 10,
          ANSI_COLOR_BLACK         = 30,
          ANSI_COLOR_RED           = 31, 
          ANSI_COLOR_GREEN         = 32,
          ANSI_COLOR_YELLOW        = 33,
          ANSI_COLOR_BLUE          = 34,
          ANSI_COLOR_MAGENTA       = 35,
          ANSI_COLOR_CYAN          = 36,
          ANSI_COLOR_WHITE         = 37,
          ANSI_COLOR_GRAY          = 90,
          ANSI_COLOR_BRIGHT_RED    = 91,
          ANSI_COLOR_BRIGHT_GREEN  = 92,
          ANSI_COLOR_BRIGHT_YELLOW = 93,
          ANSI_COLOR_BRIGHT_BLUE   = 94,
          ANSI_COLOR_BRIGHT_PINK   = 95,
          ANSI_COLOR_BRIGHT_CYAN   = 96,
          ANSI_COLOR_BRIGHT_WHITE  = 97,
     } AnsiFormatType_t;

     #define GetANSIFormatString(fmtType)                                 ({ \
          char __tempCharBuf[64];                                            \
          std::string __returnStr;                                           \
          do {                                                               \
               if(fmtType == ANSI_COLOR_OFF) {                               \
                    __tempCharBuf[0] = '\0';                                 \
               }                                                             \
               else {                                                        \
                    std::snprintf(__tempCharBuf, 64, "\033[%dm", fmtType);   \
                    __tempCharBuf[63] = '\0';                                \
               }                                                             \
               __returnStr = std::string(__tempCharBuf);                     \
          } while(0);                                                        \
          __returnStr;                                                       \
     })

     std::string GetAnsiFormattedTextMsg(AnsiFormatType_t ansiFormatType, const char *msgText) {
          return GetANSIFormatString(ansiFormatType) + std::string(msgText) + GetANSIFormatString(ANSI_COLOR_NORMAL);
     }

     static inline std::string GetBoostLoggerMessageFormat(bool enableAnsiTerminalColor) { 
          AnsiFormatType_t loggerHeaderColor    = ANSI_COLOR_BRIGHT_PINK;
          AnsiFormatType_t loggerTimestampColor = ANSI_COLOR_BRIGHT_GREEN;
          AnsiFormatType_t loggerSeverityColor  = ANSI_COLOR_BRIGHT_RED;
          AnsiFormatType_t loggerMessageColor   = ANSI_COLOR_GRAY;
          if(enableAnsiTerminalColor) {
               loggerHeaderColor = loggerTimestampColor = 
                                   loggerSeverityColor = 
                                   loggerMessageColor = ANSI_COLOR_OFF;
          }
          std::string msgFmt = GetAnsiFormattedTextMsg(loggerHeaderColor, "[PMFE-LOGGER]") + std::string(" ") + 
                               GetAnsiFormattedTextMsg(loggerTimestampColor, "@%TimeStamp%") + std::string(" ") + 
                               std::string("(Type=") + GetAnsiFormattedTextMsg(loggerSeverityColor, "%Severity%") + 
                               std::string("): ") + GetAnsiFormattedTextMsg(loggerMessageColor, "%Message%");
          return msgFmt;
     }

     static BoostSrc::severity_logger_mt<BoostLogger::trivial::severity_level> __logger;

     static inline bool ConfigureBoostLogging(BoostLoggerSeverityLevel_t defaultSeverityLevel = BOOST_LOG_DEFAULT, 
                                              bool enableLoggingToFile = true, 
                                              bool enableVerboseConsoleDevelReporting = true) {
          BoostLogger::settings loggerDefaultSettings;
          loggerDefaultSettings["Core"]["DisableLogging"] = false;
          loggerDefaultSettings["Sinks.File.AutoFlush"] = true;
          BoostLogger::init_from_settings(loggerDefaultSettings);
          BoostLogger::add_common_attributes();
          BoostLogger::trivial::severity_level defaultLogReportLevel = GetBoostLoggerLevel(defaultSeverityLevel);
          BoostLogger::core::get()->set_filter(BoostLogger::trivial::severity >= defaultLogReportLevel);
          BoostLogger::core::get()->add_global_attribute("Timestamp",        BoostAttrs::local_clock());
          BoostLogger::core::get()->add_global_attribute("SourceFileName",   BoostAttrs::mutable_constant<std::string>(__FILE__));
          BoostLogger::core::get()->add_global_attribute("SourceLineNumber", BoostAttrs::mutable_constant<unsigned int>(__LINE__));
          BoostLogger::core::get()->add_global_attribute("SourceFileFunc",   BoostAttrs::mutable_constant<std::string>(__func__));
          /*if(enableLoggingToFile) {
               auto fsSink = BoostLogger::add_file_log(
                    BoostKeywords::target = GetExpandedFilePath(PMFE_LOGFILE_DEFAULT_DIR, true), 
                    BoostKeywords::file_name = GetTimeStampedFilePath(PMFE_LOGFILE_DEFAULT_FILE, true),
                    BoostKeywords::filter = BoostLogger::trivial::severity >= defaultLogReportLevel,
                    BoostKeywords::rotation_size = 10 * 1024 * 1024,
                    BoostKeywords::scan_method = BoostSink::file::scan_matching,
                    BoostKeywords::format = GetBoostLoggerMessageFormat(false),
                    BoostKeywords::min_free_space = 30 * 1024 * 1024,
                    BoostKeywords::open_mode = std::ios_base::app 
              );
              fsSink->locked_backend()->auto_flush(true);
          }
          if(enableVerboseConsoleDevelReporting) { 
               // Setup cues for developers including source file stamps for logging calls:
               std::string develReportPrefix = GetAnsiFormattedTextMsg(ANSI_COLOR_BRIGHT_WHITE, "<<< ") + 
                           GetAnsiFormattedTextMsg(ANSI_COLOR_BRIGHT_YELLOW, "%SourceFileName%//#%%SourceLineNumber% -- in %SourceFileFunc%(...)") + 
                           GetAnsiFormattedTextMsg(ANSI_COLOR_BRIGHT_WHITE, " >>>") + 
                           std::string("\n   ");
               boost::shared_ptr< BoostSink::synchronous_sink< BoostSink::text_ostream_backend > > consoleLoggerSink = 
                      BoostLogger::add_console_log(std::clog, BoostKeywords::format = develReportPrefix);
          }*/
          return true;
     }

     const BoostLogger::trivial::severity_level PMFE_LOG_TRACE   = BoostLogger::trivial::trace;
     const BoostLogger::trivial::severity_level PMFE_LOG_DEBUG   = BoostLogger::trivial::debug;
     const BoostLogger::trivial::severity_level PMFE_LOG_INFO    = BoostLogger::trivial::info;
     const BoostLogger::trivial::severity_level PMFE_LOG_WARNING = BoostLogger::trivial::warning;
     const BoostLogger::trivial::severity_level PMFE_LOG_ERROR   = BoostLogger::trivial::error;
     const BoostLogger::trivial::severity_level PMFE_LOG_FATAL   = BoostLogger::trivial::fatal;

     static bool __PMFELoggerIsInit = false;

     static inline bool Init() {
          __PMFELoggerIsInit = ConfigureBoostLogging();
          return __PMFELoggerIsInit;
     }

     static inline void GetLogger(BoostLogger::trivial::severity_level severityLevel) {
          if(!__PMFELoggerIsInit) {
               Init();
          }
          BOOST_LOG_SEV(PMFELocalLogging::__logger, severityLevel);
     }

} // namespace PMFELocalLogging

namespace PMFELogger = PMFELocalLogging;

using PMFELocalLogging::PMFE_LOG_DEBUG;
using PMFELocalLogging::PMFE_LOG_ERROR;
using PMFELocalLogging::GetLogger;

#endif
