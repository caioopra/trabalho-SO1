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

    // "placement new" pra isntanciar Thread main
    new (&_main) Thread(main, (void*)"Main");
    new (&_main_context) CPU::Context();
    new (&_dispatcher) Thread(&dispatcher);

    _main._state = RUNNING;     // muda estado da thread main
    _running = &_main;          // atualiza ponteiro da thread em exec

    CPU::switch_context(&_main_context, _main.context());
}

int Thread::switch_context(Thread* prev, Thread* next) {
    if (prev->id() != next->id()) {
        _running = next;
        return CPU::switch_context(prev->_context, next->_context);
    }

    return 0;
}

void Thread::thread_exit(int exit_code) {
    db<Thread>(TRC) << " - Thread (" << id() << "): exit code: " << exit_code << "\n";
    Thread::_thread_count--;
    _exit_code = exit_code;
    _state = FINISHING;

    // enquanto tiverem threads na fila que estava esperando pelo fim dessa, vai voltando elas para fila de pronto
    while (_suspended.size() > 0) {
        Thread* _joining = _suspended.remove_head()->object();
        db<Thread>(TRC) << " - Thread " << _joining->id() << " saindo\n"; 
        _joining->resume();
    }

    yield();        // devolve processador para dispatcher
}

int Thread::id() {
    return this->_id;
}

void Thread::dispatcher() {
    db<Thread>(TRC) << " - Thread dispatcher chamada \n";
    
    while(Thread::_ready.size() > 0) {
        // retorna ponteiro para proxima thread na fila
        Thread* proxima_thread = Thread::_ready.remove_head()->object();
        db<Thread>(TRC) << " - proxima thread : " << proxima_thread->id() << "\n";
        
        Thread::_dispatcher._state = READY;
        Thread::_ready.insert(&Thread::_dispatcher._link);
        Thread::_running = proxima_thread;
        proxima_thread->_state = RUNNING;

        Thread::switch_context(&Thread::_dispatcher, proxima_thread);

        if (Thread::_ready.size() > 0 && Thread::_ready.head()->object()->_state == FINISHING){
            Thread::_ready.remove_head();
        }
    }

    Thread::_dispatcher._state = FINISHING;

    db<Thread>(TRC) << " - Dispatcher liberado, indo para Thread Main \n";
    Thread::switch_context(&Thread::_dispatcher, &Thread::_main);
}

void Thread::yield() {
    db<Thread>(TRC) << " - yield chamado pela Thread " << Thread::_running->id() << "\n";

    Thread *current_thread = Thread::_running;
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

int Thread::join() {
    db<Thread>(TRC) << " - Thread " << id() << " fazendo join.\n";
   
    // nao pode esperar por ela mesmo
    if (_running == this) {
        return -1;
    }

    if (_state != SUSPENDED) {
        // thread é suspensa até que as que ela está esperando finalizem
        _suspended.insert(&_running->_link);
        _running->suspend();
    }

    return _exit_code;
} 

void Thread::suspend() {
    db<Thread>(TRC) << " - Thread " << id() << "sendo suspensa.\n";

    Thread::_ready.remove(this);   // remove thread da fila de prontos
    _state = SUSPENDED;            // vai para estado de suspensa
    yield();  // deixa o processador
}

void Thread::resume() {
    db<Thread>(TRC) << " - Thread " << id() << " fez resume.\n";
    // volta para a fila de prontos
    _state = READY;
    Thread::_ready.insert(&this->_link); 
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
