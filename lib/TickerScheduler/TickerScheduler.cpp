#include <Arduino.h>
#include "TickerScheduler.h"

void tickerFlagHandle(volatile bool * flag)
{
    if (!*flag)
        *flag = true;
}

TickerScheduler::TickerScheduler(){}

TickerScheduler::TickerScheduler(uint size)
{
    this->items = new TickerSchedulerItem[size];
    this->_size = size;
}

void TickerScheduler::init(uint size)
{
  //remove all old handlers
  if(this->_size > 0)
  {
    for (uint i = 0; i < this->_size; i++)
    {
        this->remove(i);
        yield();
    }

    delete[] this->items;
    this->items = NULL;
  }

  this->items = new TickerSchedulerItem[size];
  this->_size = size;
}

uint TickerScheduler::getSize()
{
  return this->_size;
}

TickerScheduler::~TickerScheduler()
{
    for (uint i = 0; i < this->_size; i++)
    {
        this->remove(i);
        yield();
    }

    delete[] this->items;
    this->items = NULL;
    this->_size = 0;
}

void TickerScheduler::handleTicker(tscallback_t f, volatile bool * flag)
{
    if (*flag)
    {
        yield();
        *flag = false;
        yield();
        f();
        yield();
    }
}

boolean TickerScheduler::add(uint i, uint32_t period, tscallback_t f, boolean shouldFireNow)
{
    if (i >= this->_size)
        return false;

    if (this->items[i].is_used)
        return false;

    this->items[i].cb = f;
    this->items[i].flag = shouldFireNow;
    this->items[i].t.attach_ms(period, tickerFlagHandle, &this->items[i].flag);
    this->items[i].is_used = true;

    return true;
}

boolean TickerScheduler::remove(uint i)
{
    if (i >= this->_size)
        return false;

    if (!this->items[i].is_used)
        return false;

    this->items[i].is_used = false;
    this->items[i].t.detach();
    this->items[i].flag = false;
    this->items[i].cb = NULL;

    return true;
}

void TickerScheduler::update()
{
    for (uint i = 0; i < this->_size; i++)
    {

        if (this->items[i].is_used)
            handleTicker(this->items[i].cb, &this->items[i].flag);

        yield();
    }
}
