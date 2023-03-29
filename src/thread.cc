#include "thread.h"

#include <ucontext.h>

#include <iostream>

#include "traits.h"

__BEGIN_API

<<<<<<< HEAD
int Thread::switch_context(Thread* prev, Thread* next) {
}

=======
unsigned int Thread::thread_count = 0;



int Thread::switch_context(Thread* prev, Thread* next) {
    Thread::_running = next;
    return CPU::switch_context(prev->_context,next->_context);
}


>>>>>>> 3063a370d5e0245154033465d6e6a77c061d534e
void Thread::thread_exit(int exit_code) {
    delete this->_context;

    Thread::thread_count--;
}

int Thread::id() {
    return this->_id;
}

__END_API