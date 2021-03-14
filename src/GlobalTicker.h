#ifndef _GLOBAL_TICKER_H
#define _GLOBAL_TICKER_H

#include <vector>
#include <Ticker.h>

class GlobalTicker : public Ticker
{
  static std::vector<GlobalTicker *> tickers;

public:
  static void updateAll()
  {
    for (GlobalTicker *ticker : tickers)
      ticker->update();
  }

  GlobalTicker(uint32_t timer, fptr callback, uint32_t repeat = 0, resolution_t resolution = MILLIS) : Ticker(callback, timer, repeat, resolution)
  {
    tickers.push_back(this);
  }

  ~GlobalTicker()
  {
    tickers.erase(std::remove(tickers.begin(), tickers.end(), this), tickers.end());
  }
};

std::vector<GlobalTicker *> GlobalTicker::tickers;

#endif
