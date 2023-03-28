#include "thread.h"

#include <ucontext.h>

#include <iostream>

#include "traits.h"

__BEGIN_API

template <typename... Tn>
static int Thread::switch_context(Thread* prev, Thread* next) {
}

template <typename... Tn>
void Thread::thread_exit(int exit_code) {
}

template <typename... Tn>
int Thread::id() {
    return this->_id;
}

__END_API