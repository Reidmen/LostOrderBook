/*
 * Inspired from timestamp.h (Ivan Shynkarenka)
 * under MIT License
 */

#ifndef TIMESTAMP_HPP
#define TIMESTAMP_HPP

#include <chrono>
#include <cstdint>

// Timestamp
/*
    Timestamp wraps time moment in nanoseconds and allows to get separate values
   of days, hours, minutes, seconds, milliseconds, microseconds or nanoseconds.
   Also it is possible to get difference between two timestamps as a timespan.

    Nanosecond timestamp based on 64-bit integer can represent each nanosecond
   in the time range of ~584.554531 years. Therefore timestamp bounds from
   01.01.1970 to 31.12.2553.

    Timestamp epoch January 1, 1970 at 00:00:00

    Not thread-safe.
*/

class Timestamp {
   protected:
    uint64_t _timestamp;

   public:
    // initialize timestamp with an epoch time
    Timestamp() noexcept : _timestamp(epoch()){};
    // initialize timestamp with a given time moment in nanoseconds
    explicit Timestamp(uint64_t timestamp) noexcept : _timestamp(timestamp){};

    // initialize with a given std::chrono time point
    template <class Clock, class Duration>
    explicit Timestamp(
        const std::chrono::time_point<Clock, Duration>& time_point) noexcept
        : _timestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(
                         time_point.time_since_epoch())
                         .count()){};
    Timestamp(const Timestamp&) noexcept = default;
    Timestamp(Timestamp&&) noexcept = default;
    ~Timestamp() noexcept = default;

    // Timestamp offset operations
    Timestamp& operator+=(int64_t offset) noexcept {
        _timestamp += offset;
        return *this;
    }

    Timestamp& operator-=(int64_t offset) noexcept {
        _timestamp -= offset;
        return *this;
    }

    friend Timestamp operator+(const Timestamp& timestamp,
                               int64_t offset) noexcept {
        return Timestamp(timestamp.total() + offset);
    }
    friend Timestamp operator+(int64_t offset,
                               const Timestamp& timestamp) noexcept {
        return Timestamp(offset + timestamp.total());
    }

    friend Timestamp operator-(const Timestamp& timestamp,
                               int64_t offset) noexcept {
        return Timestamp(timestamp.total() + offset);
    }
    friend Timestamp operator-(int64_t offset,
                               const Timestamp& timestamp) noexcept {
        return Timestamp(offset + timestamp.total());
    }

    // Convert timestmap to std::chrono time point
    std::chrono::system_clock::time_point chrono() const noexcept {
        return std::chrono::time_point_cast<
            std::chrono::system_clock::duration>(
            (std::chrono::time_point<std::chrono::system_clock>() +
             std::chrono::nanoseconds(_timestamp)));
    }

    // Get total days
    uint64_t days() const noexcept {
        return _timestamp / (24 * 60 * 60 * 1000000000ull);
    }
    // Get total hours
    uint64_t hours() const noexcept {
        return _timestamp / (60 * 60 * 1000000000ull);
    }
    // Get total minutes
    uint64_t minutes() const noexcept {
        return _timestamp / (60 * 1000000000ull);
    }
    // Get total seconds
    uint64_t seconds() const noexcept { return _timestamp / 1000000000; }
    // Get milliseconds
    uint64_t milliseconds() const noexcept { return _timestamp / 1000000; }
    // Get microseconds
    uint64_t microseconds() const noexcept { return _timestamp / 1000; }
    // Get nanoseconds
    uint64_t nanoseconds() const noexcept { return _timestamp; }

    // Get total value of the current timestamp (total nanoseconds)
    uint64_t total() const noexcept { return _timestamp; }
    // Get the epoch timestamp (thread-safe)
    static uint64_t epoch() noexcept { return 0; }
    // Get the UTC timetamp (thread-safe)
    static uint64_t utc();
    // Get the local timestamp (thread-safe)
    static uint64_t local();
    // Get the high resolution timestamp (thread-safe)
    static uint64_t nano();
    // Get the current value RDTS (Read timestamp counter)
    static uint64_t rdts();

    // swap two instances
    void swap(Timestamp& timestamp) noexcept;
    friend void swap(Timestamp& timestamp1, Timestamp& timestamp2) noexcept;
};

class EpochTimestamp : public Timestamp {
   public:
    // Initialize with timestamp epoch
    EpochTimestamp() : Timestamp(Timestamp::epoch()){};
    // Initialize epoch timestamp with another one
    EpochTimestamp(const Timestamp& timestamp) : Timestamp(timestamp){};
};

class NanoTimestamp : public Timestamp {
   public:
    // Initialize high resolution timestamp with current high resolition time
    NanoTimestamp() : Timestamp(Timestamp::nano()){};
    // Initialize high resolution timestamp with another timestamp value
    NanoTimestamp(const Timestamp& timestamp) : Timestamp(timestamp){};
};

#endif
