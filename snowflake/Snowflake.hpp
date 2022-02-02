#ifndef SNOWFLAKE_HPP_
#define SNOWFLAKE_HPP_
#include <stdint.h> //int64_t
#include <chrono>
#include <stdexcept>
#include <mutex>

namespace distributedAlgo{
class Snowflake_nonlock {
public:
    void lock() {}
    void unlock() {}
};

template<int64_t Twepoch, typename Lock = Snowflake_nonlock>
class Snowflake {
private:
    // 定义两个类型别名
    using lock_type = Lock;
    using time_point = std::chrono::time_point<std::chrono::steady_clock>;

    // 定义几个常量表达式
    // 项目开启时间，时间戳减去这个值
    static constexpr int64_t TWEPOCH = Twepoch;
    // hostid占5位
    static constexpr int64_t WORKER_ID_BITS = 5L;
    // data center id占5位
    static constexpr int64_t DATACENTER_ID_BITS = 5L;
    // 每个数据中心支持的最多主机数
    static constexpr int64_t MAX_WORKER_ID = (1 << WORKER_ID_BITS) - 1; 
    // 雪花算法支持的最多数据中心数
    static constexpr int64_t MAX_DATACENTER_ID = (1 << DATACENTER_ID_BITS) - 1;
    // 每台主机每毫秒最多可以产生的id占12位，即每毫秒最多产生4096个id
    static constexpr int64_t SEQUENCE_BITS = 12L;
    // 雪花算法总共分为5个部分
    /*
    符号位      时间戳                  数据中心id          主机id              序号
    0       41位（可用69年）          5位（32个机房）     5位（32台服务器）  12位（每台主机每毫秒最多产生4096个ID）
    */
    static constexpr int64_t WORKER_ID_SHIFT = SEQUENCE_BITS;
    static constexpr int64_t DATACENTER_ID_SHIFT = WORKER_ID_SHIFT + WORKER_ID_BITS;
    static constexpr int64_t TIMESTAMP_LEFT_SHIFT = DATACENTER_ID_SHIFT + DATACENTER_ID_BITS;
    static constexpr int64_t SEQUENCE_MASK = (1 << SEQUENCE_BITS) - 1;

private:
    time_point start_time_point_ = std::chrono::steady_clock::now();
    int64_t start_millsecond_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    int64_t last_timestamp_ = -1;
    int64_t workerid_ = 0;
    int64_t datacenterid_ = 0;
    int64_t sequence_ = 0;
    lock_type lock_;

public:
    Snowflake() = default;
    Snowflake(int64_t workerid, int64_t datacenterid);
    
    // 禁用默认的拷贝构造函数和拷贝赋值运算符
    Snowflake(const Snowflake&) = delete;
    Snowflake& operator=(const Snowflake&) = delete;

    void init(int64_t workerid, int64_t datacenterid);
    int64_t nextid();

private:
    int64_t millsecond() const noexcept;
    int64_t wait_next_millis(int64_t last) const noexcept;
};
};

#endif