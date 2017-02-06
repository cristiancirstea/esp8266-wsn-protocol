#ifndef ticker_scheduler_h
#define ticker_scheduler_h
#include <Ticker.h>
#include <stdint.h>
#include <functional>

void tickerFlagHandle(volatile bool * flag);

typedef std::function<void(void)> tscallback_t;

struct TickerSchedulerItem
{
    Ticker t;
    volatile bool flag = false;
    tscallback_t cb;
    volatile bool is_used = false;
};

class TickerScheduler
{
private:
    uint _size = 0;
    TickerSchedulerItem *items = NULL;

    void handleTicker(tscallback_t, volatile bool * flag);

public:
    TickerScheduler();
    TickerScheduler(uint size);
    ~TickerScheduler();

    boolean add(uint i, uint32_t period, tscallback_t, boolean shouldFireNow = false);

    boolean remove(uint i);

    void init(uint size);
    uint getSize();

    void update();
};
#endif
