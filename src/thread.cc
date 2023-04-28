#include "thread.h"

#include <ucontext.h>

#include <iostream>

__BEGIN_API

// valores iniciais para os atributos static
Thread* Thread::_running = nullptr;
unsigned int Thread::_thread_count = 0;

Thread Thread::_main;
CPU::Context Thread::_main_context;
Thread Thread::_dispatcher;
Thread::Ready_Queue Thread::_ready;

// métodos das classes
void Thread::init(void (*main)(void*)) {
    db<Thread>(TRC) << " - Inicializando Threads Main e Dispatcher\n";

    new (&_ready) Thread::Ready_Queue();

    // "placement new" pra isntanciar Thread main
    new (&_main) Thread(main, (void*)"Main");
    new (&_main_context) CPU::Context();

    // criando thread Dispatcher, argumentos: *dispatcher(), (void*) NULL
    // cast para void* de dispatcher() e depois para void o conteúdo desse void*
    new (&_dispatcher) Thread((void (*)(void*)) & Thread::dispatcher, (void*)NULL);

    Thread::_running = &Thread::_main;  // atualiza ponteiro da thread em exec
    Thread::_main._state = RUNNING;     // muda estado da thread main

    CPU::switch_context(&_main_context, _main.context());
}

int Thread::switch_context(Thread* prev, Thread* next) {
    int switch_return = CPU::switch_context(prev->_context, next->_context);
    _running = next;

    return switch_return;
}

// TODO: alterar
void Thread::thread_exit(int exit_code) {
    db<Thread>(TRC) << " - Thread (" << id() << "): exit code: " << exit_code << "\n";

    Thread::_thread_count--;
    _exit_code = exit_code;
    _state = FINISHING;
    yield();  // devolve processador para dispatcher
    // Thread::thread_count--;
}

int Thread::id() {
    return this->_id;
}

void dispatcher() {
}

CPU::Context* Thread::context() {
    return _context;
}

__END_API