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

namespace MessageTypes {
struct SystemEventMessage {
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char EventCode;
};

struct StockDirectoryMessage {
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char Stock[8];
    char MarketCategory;
    char FinancialStatusIndicator;
};

struct StockTradingActionMessage {
    char Type;
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;
    char Stock[8];
    char TradingState;
    char Reserved;
    char Reason;
};

struct MarketParticipantPositionMessage {};
struct AddOrderMesssage {};
struct OrderExecutedMessage {};
struct OrderExecutedWithPriceMessage {};
struct OrderCancelMessage {};
struct OrderDeleteMessage {};

}  // namespace MessageTypes

class ITCHHandler {
   public:
    ITCHHandler() { ResetHandler(); }
    ITCHHandler(const ITCHHandler& ithandler) = delete;

    virtual ~ITCHHandler() = default;

    bool Process(void* buffer, std::size_t size);

    bool ProcessMessage(void* buffer, std::size_t size);

    void ResetHandler();

   protected:
    // message handlers
    virtual bool onMessage(const MessageTypes::SystemEventMessage& message) {
        return true;
    }
    virtual bool onMessage(const MessageTypes::StockDirectoryMessage& message) {
        return true;
    }
    virtual bool onMessage(
        const MessageTypes::StockTradingActionMessage& message) {
        return true;
    }
    virtual bool onMessage(
        const MessageTypes::MarketParticipantPositionMessage& message) {
        return true;
    }
    virtual bool onMessage(const MessageTypes::AddOrderMesssage& message) {
        return true;
    }
    virtual bool onMessage(const MessageTypes::OrderExecutedMessage& message) {
        return true;
    }
    virtual bool onMessage(
        const MessageTypes::OrderExecutedWithPriceMessage& message) {
        return true;
    }
    virtual bool onMessage(const MessageTypes::OrderCancelMessage& message) {
        return true;
    }
    virtual bool onMessage(const MessageTypes::OrderDeleteMessage& message) {
        return true;
    }

   private:
    using size_t = std::size_t;
    using cache = std::vector<std::uint8_t>;

    bool ProcessSystemEventMessage(void* buffer, size_t size);
    bool ProcessStockDirectoryMessage(void* buffer, size_t size);
    bool ProcessStockTradingActionMessage(void* buffer, size_t size);
    bool ProcessMarketParticipantPositionMessage(void* buffer, size_t size);
    bool ProcessAddOrderMesssage(void* buffer, size_t size);
    bool ProcessOrderExecutedMessage(void* buffer, size_t size);
    bool ProcessOrderExecutedWithPriceMessage(void* buffer, size_t size);
    bool ProcessOrderCancelMessage(void* buffer, size_t size);
    bool ProcessOrderDeleteMessage(void* buffer, size_t size);
    bool ProcessOrderReplaceMessage(void* buffer, size_t size);
    bool ProcessTradeMessage(void* buffer, size_t size);
    bool ProcessCrossTradeMessage(void* buffer, size_t size);
    bool ProcessBrokenTradeMessage(void* buffer, size_t size);
    bool ProcessUnknownMessage(void* buffer, size_t size);

    template <size_t N>
    size_t ReadString(const void* buffer, char (&str)[N]);
    size_t readTimestamp(const void* buffer, std::uint64_t& value);
};
