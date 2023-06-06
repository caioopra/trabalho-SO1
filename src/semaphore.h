#ifndef semaphore_h
#define semaphore_h

#include "cpu.h"
#include "thread.h"
#include "traits.h"
#include "debug.h"
#include "list.h"


__BEGIN_API

class Semaphore
{
public:
    typedef Ordered_List<Thread> Sleep_queue;
    Semaphore(int v = 1) : _value(v) {}; // valor padrão 1, caso especificado, muda _value
    ~Semaphore();

    void p();
    void v();
private:
    // Atomic operations
    int finc(volatile int & number);
    int fdec(volatile int & number);

    // Thread operations
    void sleep();
    void wakeup();
    void wakeup_all();

private:
    //DECLARAÇÃO DOS ATRIBUTOS DO SEMÁFORO  
    Sleep_queue _sleeping;
    volatile int _value;
};

__END_API

#endif

