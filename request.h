#ifndef __REQUEST_H__
#include "threadpool.h"
//void requestHandle(int fd);
void requestHandle(int fd, struct timeval stat_req_arrival, struct timeval stat_req_dispatch, int thread_id, ThreadPool* thread_pool);
#endif
