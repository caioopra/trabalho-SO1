#include "semaphore.h"
#include "thread.h"

__BEGIN_API

Semaphore::~Semaphore() {

}

void Semaphore::p() {
    db<Semaphore>(TRC) << "Semaphore antes de p(): " << _value << "\n";
    if (CPU::fdec(_value) < 0) {
        sleep(); // bloqueia thread
    }
}

void Semaphore::v() {

}

void Semaphore::sleep() {

}

void Semaphore::wakeup() {

}

void Semaphore::wakeup_all() {

}

__END_API
