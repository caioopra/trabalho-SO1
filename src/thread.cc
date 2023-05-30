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
Thread::Suspended_Queue Thread::_suspended;

// métodos das classes
void Thread::init(void (*main)(void*)) {
    db<Thread>(TRC) << " - Inicializando Threads Main e Dispatcher\n";

    // "placement new" pra isntanciar Thread main
    new (&_main) Thread(main, (void*)"Main");
    new (&_main_context) CPU::Context();
    new (&_dispatcher) Thread(&dispatcher);
    new (&_suspended)  Thread::Suspended_Queue;

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

    if (this->_joining)  {
        _joining->resume();
        _joining = 0;
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
    
    if(current_thread->_state != FINISHING && current_thread != &Thread::_main && current_thread->_state != WAITING && current_thread->_state != SUSPENDED){
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
        yield();
        return -1;
    }

    if (_state != FINISHING) {
        // thread é suspensa até que as que ela está esperando finalizem
        _joining = running();
        _running->suspend();
    }

    return _exit_code;
} 

void Thread::suspend() {
    db<Thread>(TRC) << " - Thread " << id() << "sendo suspensa.\n";

    if (_running == this) {
        yield();  // deixa o processador 
    }
    _ready.remove(this);   // remove thread da fila de prontos
    _suspended.insert(&_running->_link);
    
    _state = SUSPENDED;
    // _state = SUSPENDED;            // vai para estado de suspensa
}

void Thread::resume() {
    db<Thread>(TRC) << " - Thread " << id() << " fez resume.\n";
    // volta para a fila de prontos
    _suspended.remove(&this->_link); 
    _state = READY;
    _ready.insert(&this->_link); 
}

CPU::Context* Thread::context() {
    return _context;
}

void Thread::sleep() {
    db<Thread>(TRC) << " - Thread " << id() << " sleep\n";
    Thread* thread_running = running();
    thread_running->_state = WAITING;
    yield();
}

void Thread::wakeup()
{
    db<Thread>(TRC) << " - Thread " << id() << " wakeup\n";

    this->_state = READY;
    int now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    this->_link.rank(now);
    _ready.insert(&this->_link);
    yield();
}

Thread::~Thread() {
    db<Thread>(TRC) << " - fim da thread " << id() << "\n";
    Thread::_ready.remove(&this->_link);
    if (this->context()){
        delete this->_context;
    }
}

__END_API
