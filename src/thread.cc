#include "thread.h"

#include <ucontext.h>

#include <iostream>

#include "traits.h"

__BEGIN_API

// valores iniciais para os atributos static
unsigned int Thread::thread_count = 0;
Thread* Thread::_running = nullptr;

int Thread::switch_context(Thread* prev, Thread* next) {
    int switch_return = CPU::switch_context(prev->_context, next->_context);
    _running = next;

    return switch_return;
}

void Thread::thread_exit(int exit_code) {
    delete this->_context;

    Thread::thread_count--;
}

int Thread::id() {
    return this->_id;
}

__END_API