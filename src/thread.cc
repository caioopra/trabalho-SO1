#include "thread.h"

#include <ucontext.h>

#include <iostream>

#include "traits.h"

__BEGIN_API

unsigned int Thread::thread_count = 0;


template <typename... Tn>
static int Thread::switch_context(Thread* prev, Thread* next) {
    Thread::_running = next;
    return CPU::switch_context(prev->context(),next->contex();
}

template <typename... Tn>
void Thread::thread_exit(int exit_code) {
    delete this->_context;
    
    Thread::thread_count--;
}

template <typename... Tn>
int Thread::id() {
    return this->_id;
}

__END_API