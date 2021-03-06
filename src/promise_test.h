#include <Future.h>
#include <Promise.h>

class PromiseTest {
  int num = 0;
  int target = -1;
  Promise<int> promise;

public:
  PromiseTest() {
  }

  void increment() {
    ++num;

    if (num == target) {
      promise.fulfill(num);
    }
  }

  Promise<int>* wait_for_num(int n) {
    target = n;
    return &promise;
  }
};
