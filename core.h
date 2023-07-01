#ifndef CORE_H
#define CORE_H

enum core_state
{
    CORE_IDLE,
    CORE_BUSY
};

void core1_interrupt();

void core1_entry();


#endif //CORE_H