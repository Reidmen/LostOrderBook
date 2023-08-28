#include "handler.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "utils.hpp"

void ITCHHandler::ResetHandler() {
    _size = 0;
    _cache.clear();
}

bool ITCHHandler::ProcessSystemEventMessage(void* buffer, size_t size) {
    assert((size == 12) && "Invalid size of the ITCH message type 'S'");
    if (size != 12) return false;

    uint8_t* data = (uint8_t*)buffer;

    MessageTypes::SystemEventMessage message;
    message.Type = *data++;
    data += Utils::ReadMessage(data, message.StockLocate);
    data += Utils::ReadMessage(data, message.TrackingNumber);
    data += ITCHHandler::readTimestamp(data, message.Timestamp);
    message.EventCode = *data++;
}

bool ITCHHandler::ProcessStockDirectoryMessage(void* buffer, size_t size) {
    assert((size == 39) && "Invalid size of the ITCH message type 'R'");

    if (size != 39) return false;

    uint8_t* data = (uint8_t*)buffer;
    MessageTypes::StockDirectoryMessage message;
    message.Type = *data++;
    data += Utils::ReadMessage(data, message.StockLocate);
    data += Utils::ReadMessage(data, message.TrackingNumber);
    data += ITCHHandler::readTimestamp(data, message.Timestamp);
    data += ITCHHandler::ReadString(data, message.Stock);

    message.MarketCategory *= *data++;
    message.FinancialStatusIndicator = *data++;

    return onMessage(message);
}

bool ITCHHandler::ProcessMessage(void* buffer, size_t size) {
    // message empty
    if (size == 0) return false;
    uint8_t* data = (uint8_t*)buffer;

    switch (*data) {
        case 'S':
            return ProcessSystemEventMessage(data, size);
        case 'R':
            return ProcessStockDirectoryMessage(data, size);
        case 'H':
            return ProcessStockTradingActionMessage(data, size);
        case 'L':
            return ProcessMarketParticipantPositionMessage(data, size);
        case 'A':
            return ProcessAddOrderMesssage(data, size);
        case 'E':
            return ProcessOrderExecutedMessage(data, size);
        case 'C':
            return ProcessOrderExecutedWithPriceMessage(data, size);
        default:
            return ProcessUnknownMessage(data, size);
    }
}

bool ITCHHandler::Process(void* buffer, std::size_t size) {
    size_t index = 0;
    uint8_t* data = (uint8_t*)buffer;

    while (index < size) {
        if (_size == 0) {
            size_t remaining = size - index;
            // collect message size into cache
            if (((_cache.size() == 0) && remaining < 3) ||
                (_cache.size() == 1)) {
                _cache.push_back(data[index++]);
                continue;
            }

            // read a new message size
            uint16_t message_size;
            if (_cache.empty()) {
                // read message size directly from the input buffer
                index += Utils::ReadMessage(&data[index], message_size);
            } else {
                // read message size from chache
                Utils::ReadMessage(_cache.data(), message_size);
                // clear cache
                _cache.clear();
            }

            _size = message_size;
        }

        // read new message
        if (_size > 0) {
            size_t remaining = size - index;
            // complete or place message into cache
            if (!_cache.empty()) {
                size_t tail = _size - _cache.size();
                if (tail > remaining) tail = remaining;
                _cache.insert(_cache.end(), &data[index], &data[index + tail]);
                index += tail;

                if (_cache.size() < _size) continue;
            } else if (_size > remaining) {
                _cache.reserve(_size);
                _cache.insert(_cache.end(), &data[index],
                              &data[index + remaining]);
                index += remaining;
                continue;
            }
            // TODO: missing handling of new message!
        }
    }
}
