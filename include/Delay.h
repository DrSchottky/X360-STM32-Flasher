#ifndef SRC_DELAY_H_
#define SRC_DELAY_H_
#include <stdint.h>

void msleep(uint32_t);
uint32_t mtime(void);
void clock_setup(void);

#endif