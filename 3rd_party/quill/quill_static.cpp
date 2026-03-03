#include "quill_static.h"

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/Logger.h"
#include "quill/sinks/FileSink.h"
#include "quill/sinks/ConsoleSink.h"

// Define a global variable for a logger to avoid looking up the logger each time.
// Additional global variables can be defined for additional loggers if needed.
quill::Logger* global_logger_a;

void setupQuill(char const* log_file)
{
    // Start the backend thread
    quill::BackendOptions backend_options;
    backend_options.check_printable_char = {};  // Disable sanitization
    quill::Backend::start(backend_options);

    // Setup sink and logger
    auto fileSink = quill::Frontend::create_or_get_sink<quill::FileSink>(
        log_file,
        []()
        {
            quill::FileSinkConfig cfg;
            cfg.set_open_mode('w');
            cfg.set_filename_append_option(quill::FilenameAppendOption::StartDateTime);
            return cfg;
        }(),
            quill::FileEventNotifier{});
    auto consoleSink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console");

    // Create and store the logger
    global_logger_a = quill::Frontend::create_or_get_logger(
        "root", { std::move(fileSink),std::move(consoleSink)},
        quill::PatternFormatterOptions{ "%(time) [%(thread_id)] %(short_source_location:<28) "
                                       "LOG_%(log_level:<9) %(logger:<12) %(message)",
                                       "%H:%M:%S.%Qns", quill::Timezone::GmtTime });
}