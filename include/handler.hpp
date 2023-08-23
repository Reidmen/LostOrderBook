/*
 * NASDAQ ITCH handler class
 * This handler is used to parse NASDAQ ITCH protocol and its messages
 *
 * Protocol examples can be found in:
 * https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/
 *
 * Not thread-safe
 */

#include <cstddef>
#include <cstdint>
#include <vector>

class ITCHHandler {
   public:
    ITCHHandler() { ResetHandler(); }
    ITCHHandler(const ITCHHandler& ithandler) = delete;

    virtual ~ITCHHandler() = default;

    bool Process(void* buffer, std::size_t size);

    bool ProcessMessage(void* buffer, std::size_t size);

    void ResetHandler();

   private:
    using size_t = std::size_t;
    using cache = std::vector<std::uint8_t>;

    bool ProcessSystemEventMessage(void* buffer, size_t size);
    bool ProcessStockDirectoryMessage(void* buffer, size_t size);
    bool ProcessStockTradingActionMessage(void* buffer, size_t size);
    bool ProcessMarketParticipantPositionMessage(void* buffer, size_t size);

    template <size_t N>
    size_t ReadString(const void* buffer, char (&str)[N]);
    size_t readTimestamp(const void* buffer, std::uint64_t& value);
};
