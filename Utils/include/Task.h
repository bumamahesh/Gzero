#ifndef TASK_H
#define TASK_H
#pragma once
#include "AlgoRequest.h"
#include <memory>
typedef struct Task_t {
  std::shared_ptr<AlgoRequest> request = nullptr;
  void *extras = nullptr;
  int timeoutMs =
      1000; // Default 1 second ,task timeout is notitifed on expiration
} Task_t;
#endif // TASK_H