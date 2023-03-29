#include "thread.h"

#include <ucontext.h>

#include <iostream>

#include "traits.h"

__BEGIN_API

int Thread::switch_context(Thread* prev, Thread* next) {
}

void Thread::thread_exit(int exit_code) {
}

int Thread::id() {
    return this->_id;
}

__END_API