#include "Snowflake.hpp"
using namespace distributedAlgo;

template<int64_t Twepoch, typename Lock>
void Snowflake<Twepoch, Lock>::init(int64_t workerid, int64_t datacenterid) {
    if (workerid > MAX_WORKER_ID || workerid < 0) {
        throw std::runtime_error("worker Id can't be greater than 31 or less than 0");
    }

    if (datacenterid > MAX_DATACENTER_ID || datacenterid < 0) {
        throw std::runtime_error("datacenter Id can't be greater than 31 or less than 0");
    }

    workerid_ = workerid;
    datacenterid_ = datacenterid;
}

template<int64_t Twepoch, typename Lock>
Snowflake<Twepoch, Lock>::Snowflake(int64_t workerid, int64_t datacenterid) {
    init(workerid, datacenterid);
}

template<int64_t Twepoch, typename Lock>
int64_t Snowflake<Twepoch, Lock>::millsecond() const noexcept {
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time_point_);
    return start_millsecond_ + diff.count();
}

template<int64_t Twepoch, typename Lock>
int64_t Snowflake<Twepoch, Lock>::wait_next_millis(int64_t last) const noexcept{
    auto timestamp = millsecond();
    while (timestamp <= last) {
        timestamp = millsecond();
    } 
    return timestamp;
}

template<int64_t Twepoch, typename Lock>
int64_t Snowflake<Twepoch, Lock>::nextid() {
    std::lock_guard<lock_type> lock(lock_);
    auto timestamp = millsecond();
    if (last_timestamp_ = timestamp) {
        sequence_ = (sequence_ + 1) & SEQUENCE_MASK;
        if (0 == sequence_) {
            timestamp = wait_next_millis(last_timestamp_);
        }
    } else {
        sequence_ = 0;
    }

    last_timestamp_ = timestamp;
    return ( (timestamp - TWEPOCH) << TIMESTAMP_LEFT_SHIFT )
            | (datacenterid_ << DATACENTER_ID_SHIFT)
            | (workerid_ << WORKER_ID_SHIFT)
            | sequence_;  
}