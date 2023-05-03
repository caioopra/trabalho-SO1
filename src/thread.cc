#include "thread.h"

#include <ucontext.h>

#include <iostream>

__BEGIN_API

// valores iniciais para os atributos static
unsigned int Thread::_thread_count = 0;

Thread* Thread::_running;
Thread Thread::_main;
CPU::Context Thread::_main_context;
Thread Thread::_dispatcher;
Thread::Ready_Queue Thread::_ready;

// métodos das classes
void Thread::init(void (*main)(void*)) {
    db<Thread>(TRC) << " - Inicializando Threads Main e Dispatcher\n";

    // new (&_ready) Thread::Ready_Queue();

    // "placement new" pra isntanciar Thread main
    new (&_main) Thread(main, (void*)"Main");
    new (&_main_context) CPU::Context();
    new (&_dispatcher) Thread(&dispatcher);

    // criando thread Dispatcher, argumentos: *dispatcher(), (void*) NULL
    // cast para void* de dispatcher() e depois para void o conteúdo desse void*

    _main._state = RUNNING;     // muda estado da thread main
    _running = &_main;  // atualiza ponteiro da thread em exec


    CPU::switch_context(&_main_context, _main.context());
}

int Thread::switch_context(Thread* prev, Thread* next) {
    _running = next;
    int switch_return = CPU::switch_context(prev->_context, next->_context);

    return switch_return;
}

// TODO: alterar
void Thread::thread_exit(int exit_code) {
    db<Thread>(TRC) << " - Thread (" << id() << "): exit code: " << exit_code << "\n";
    Thread::_thread_count--;
    _exit_code = exit_code;
    _state = FINISHING;
    yield();  // devolve processador para dispatcher
}

int Thread::id() {
    return this->_id;
}

void Thread::dispatcher() {
    db<Thread>(TRC) << " - Thread dispatcher chamada \n";
    
    while(Thread::_ready.size() > 0) {
        // retorna ponteiro para proxima thread na fila
        db<Thread>(TRC) << " - remove linha 64 : " << Thread::_ready.size() << "\n";
        Thread* proxima_thread = Thread::_ready.remove_head()->object();
        
        Thread::_dispatcher._state = READY;
        Thread::_ready.insert(&Thread::_dispatcher._link);
        Thread::_running = proxima_thread;
        proxima_thread->_state = RUNNING;

        Thread::switch_context(&Thread::_dispatcher, proxima_thread);

        if (Thread::_ready.size() > 0 && Thread::_ready.head()->object()->_state == FINISHING){
        // if (Thread::_ready.size() > 0 && Thread::_running->_state == FINISHING){  
            db<Thread>(TRC) << " - remove linha 76 : " << Thread::_ready.size() << "\n";
            Thread::_ready.remove_head();

        }
    }

    Thread::_dispatcher._state = FINISHING;
    // Thread::_ready.remove(&Thread::_dispatcher);

    db<Thread>(TRC) << " - Dispatcher liberado, indo para Thread Main \n";
    Thread::switch_context(&Thread::_dispatcher, &Thread::_main);
}

void Thread::yield(){
    db<Thread>(TRC) << " - yield chamado pela Thread " << Thread::_running->id() << "\n";

    Thread *current_thread = Thread::_running;
    db<Thread>(TRC) << " - remove linha 92 : " << Thread::_ready.size() << "\n";
    Thread *proxima_thread = Thread::_ready.remove()->object();
    
    if(current_thread->_state != FINISHING && current_thread != &Thread::_main){
        current_thread->_link.rank(std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        current_thread->_state = READY;
        Thread::_ready.insert(&current_thread->_link);
        db<Thread>(TRC) << " - inseriu Thread " << proxima_thread->id() << " na fila de prontos\n";
    }
    Thread::_running = proxima_thread;
    Thread::_running->_state = RUNNING;
    
    Thread::switch_context(current_thread,proxima_thread);
}

CPU::Context* Thread::context() {
    return _context;
}

Thread::~Thread() {
    db<Thread>(TRC) << " - fim da thread " << id() << "\n";
    Thread::_ready.remove(&this->_link);
    if (this->context()){
        delete this->_context;
    }
}

__END_API