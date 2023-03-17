#ifndef cpu_h
#define cpu_h

#include <ucontext.h>
#include <iostream>
#include "traits.h"

__BEGIN_API

class CPU
{
    public:

        class Context
        {
        private:
            static const unsigned int STACK_SIZE = Traits<CPU>::STACK_SIZE;
        public:
            Context() { _stack = 0; }

            template<typename ... Tn>
            Context(void (* func)(Tn ...), Tn ... an);

            ~Context();

            void save();
            void load();

        private:         
            char *_stack;
        public:
            ucontext_t _context;
        };

    public:

        /**
         * @brief Troca contexto usando os contextos recebidos como parametro
         * 
         * @param from contexto atual
         * @param to contexto que irá entrar
         * @return int: 0 caso seja bem sucedido ou negativo caso haja erro
         */
        static int switch_context(Context *from, Context *to);

};

__END_API

#endif

