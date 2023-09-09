#include <cstddef>
#include <cstdint>

#include "handler.hpp"
#include "utils.hpp"

int main(int argc, char** argv) {
    // process input
    size_t size;
    uint8_t buffer[8192];

    ITCHHandler itch_handler;

    while ((size = Utils::ReadMessage(buffer, sizeof(buffer))) > 0) {
        // process buffer
        itch_handler.Process(buffer, size);
    }

    return 0;
}
