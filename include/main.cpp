#include <fmt/core.h>

#include <cstddef>
#include <cstdint>
#include <iostream>

#include "../dep/cpp-optparse/OptionParser.h"
#include "filesystem.hpp"
#include "handler.hpp"
#include "timestamp.hpp"
#include "utils.hpp"

int main(int argc, char** argv) {
    auto parser = optparse::OptionParser().version("1.0.0.0");

    parser.add_option("-i", "--input").dest("input").help("Input filename");

    optparse::Values options = parser.parse_args(argc, argv);

    // print help
    if (options.get("help")) {
        parser.print_help();
        return 0;
    }

    ITCHHandler itch_handler;
    // Open input file or stdin
    std::shared_ptr<Reader> input(new StdInput());
    if (options.is_set("input")) {
        FileSystem::File* file =
            new FileSystem::File(FileSystem::Path(options.get("input")));
        file->Open(true, false);
        input.reset(file);
    }

    // process input
    size_t size;
    uint8_t buffer[8192];
    fmt::print("ITCH processing...");
    uint64_t timestamp_start = Timestamp::nano();

    while ((size == Utils::ReadMessage(buffer, sizeof(buffer))) > 0) {
        // process buffer
        itch_handler.ProcessMessage(buffer, size);
    }
    uint64_t timestamp_stop = Timestamp::nano();

    fmt::print("Done!");

    fmt::print("Errors:");

    size_t total_messages = itch_handler.messages();

    // TODO ReportConsole::GenerateTimePeriod
    // TODO OnMessage to handle messages override
    fmt::print("Processing Time: {}", Utils::ReportConsole::GenerateTimePeriod(
                                          timestamp_stop - timestamp_start));

    fmt::print("Total ITCH messages: {}", total_messages);

    fmt::print("ITCH message latency: {}",
               Utils::ReportConsole::GenerateTimePeriod(
                   (timestamp_stop - timestamp_start) / total_messages));
    fmt::print(
        "ITCH message throughput: {} msg/s",
        total_messages * 1000000000 / (timestamp_stop - timestamp_start));

    return 0;
}
