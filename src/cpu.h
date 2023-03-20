#ifndef cpu_h
#define cpu_h

#include <ucontext.h>

#include <iostream>

#include "traits.h"

__BEGIN_API

class CPU {
   public:
    class Context {
       private:
        static const unsigned int STACK_SIZE = Traits<CPU>::STACK_SIZE;
        char *_stack;

       public:
        ucontext_t _context;
        Context() {
            _stack = 0;
        }

        template <typename... Tn>
        Context(void (*func)(Tn...), Tn... an);
        
        ~Context();

        void save();
        void load();
    };

   public:
    static int switch_context(Context *from, Context *to);
};

template <typename ...Tn>
CPU::Context::Context(void (*func)(Tn...), Tn... an) {
    this->_stack = new char[this->STACK_SIZE];
    getcontext(&this->_context);
    this->_context.uc_link = 0;
    this->_context.uc_stack.ss_sp = (void *)_stack;
    this->_context.uc_stack.ss_size = this->STACK_SIZE;
    this->_context.uc_stack.ss_flags = 0;

    //(void (*)(void))func -> casting a function pointer to a void*
    makecontext(&this->_context, (void (*)(void))func, sizeof...(Tn), an...);
}

__END_API

#endif
