#include <iostream>
#include "system.h"
#include "thread.h"

__BEGIN_API

void System::init(void (*main)(void*)) {
    db<System>(TRC) << "Inicializando o sistema\n";
    setvbuf(stdout, 0, _IONBF, 0);
    Thread::init(main);
    db<System>(TRC) << "Thread Main inicializada,\n ";
}

__END_API