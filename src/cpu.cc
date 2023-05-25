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

int CPU::finc(volatile int& number) {
    int reg = 1;
    // faz lock durante xadd
    // troca o valor dos operandos e coloca a soma dos dois no destino
    asm("lock xadd %0, %2" : "=a" (reg) : "a" (reg), "m" (number));
    return reg;
}

int CPU::fdec(volatile int& number) {
    int reg = -1;
    asm("lock xadd %0, %2" : "=a" (reg) : "a" (reg), "m" (number));
    return reg;
}

__END_API
