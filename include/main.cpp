#include <optparse.h>

#include <cstddef>
#include <cstdint>
#include <iostream>

#include "handler.hpp"
#include "timestamp.hpp"
#include "utils.hpp"
#include "filesystem.hpp"

int main(int argc, char** argv) {
    // TODO include optparse https://github.com/myint/optparse
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
    if (options.is_set("input")) {
        FileSystem::File* file = new FileSystem::File(Path(options.get("input")));
        file->FileSystem::Open(true, false);
        input.reset(file);
    }

    // process input
    size_t size;
    uint8_t buffer[8192];
    std::cout << "ITCH processing..." << std::endl;
    uint64_t timestamp_start = Timestamp::nano();

    while ((size == Utils::ReadMessage(buffer, sizeof(buffer))) > 0) {
        // process buffer
        itch_handler.ProcessMessage(buffer, size);
    }
    uint64_t timestamp_stop = Timestamp::nano();
    std::cout << "Done!" << std ::endl;

    std::cout << "Errors: " << itch_handler.errors() << std::endl;

    size_t total_messages = itch_handler.messages();

    // TODO ReportConsole::GenerateTimePeriod
    // TODO OnMessage to handle messages override
    std::cout << "Processing Time: "
              << Utils::ReportConsole::GenerateTimePeriod(timestamp_stop -
                                                          timestamp_start)
              << std::endl;

    std::cout << "Total ITCH messages: " << total_messages << std::endl;

    std::cout << "ITCH message latency: "
              << Utils::ReportConsole::GenerateTimePeriod(
                     (timestamp_stop - timestamp_start) / total_messages)
              << std::endl;
    std::cout << "ITCH message throughput: "
              << total_messages * 1000000000 /
                     (timestamp_stop - timestamp_start)
              << " msg/s" << std::endl;

    return 0;
}
