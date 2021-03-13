#include <functional>

#ifndef _MENU_COMMAND_H
#define _MENU_COMMAND_H

template <class T>
class Future
{
public:
  virtual void then(std::function<void(T)> handler) = 0;
};

#endif
