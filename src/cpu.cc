#include "cpu.h"

#include <ucontext.h>

#include <iostream>

__BEGIN_API

void CPU::Context::save() {
    getcontext(&_context);
}

void CPU::Context::load() {
    setcontext(&_context);
}

CPU::Context::~Context() {
    if (_stack) {
        delete _stack;
    }
}

int CPU::switch_context(Context *from, Context *to) {
    if (from && to) {
        return swapcontext(&from->_context, &to->_context);
    } else {
        return -1;
    }
}

__END_API
