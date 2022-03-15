#pragma once

#include <semaphore.h>

#include <string>

namespace netconf {

class SharedSemaphore {
 public:
  SharedSemaphore(const ::std::string& path, uint32_t init_value);
  virtual ~SharedSemaphore();

  void lock();
  void unlock();

  SharedSemaphore(const SharedSemaphore& other) = delete;
  SharedSemaphore(SharedSemaphore&& other)      = delete;
  SharedSemaphore& operator=(const SharedSemaphore& other) = delete;
  SharedSemaphore& operator=(SharedSemaphore&& other) = delete;

 private:
  sem_t* semaphore_;
};

}  // namespace netconf
