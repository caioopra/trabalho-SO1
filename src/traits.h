#ifndef traits_h
#define traits_h

// Não alterar as 3 declarações abaixo

#define __BEGIN_API namespace SOLUTION {
#define __END_API }
#define __USING_API using namespace SOLUTION;

__BEGIN_API

class CPU;  // declaração das classes criadas nos trabalhos devem ser colocadas aqui
class Thread;
class System;
class Debug;

// declaração da classe Traits
template <typename T>
struct Traits {
    static const bool debbuged = false;
};

template <>
struct Traits<CPU> {
    static const unsigned int STACK_SIZE = 0x64000;
};

template <>
struct Traits<Debug> : public Traits<void> {
    static const bool error = false;
    static const bool warning = false;
    static const bool info = false;
    static const bool trace = true;
};

template <>
struct Traits<System> : public Traits<void> {
    static const bool debbuged = true;
};

template <>
struct Traits<Thread> : public Traits<void> {
    static const bool debbuged = true;
};

__END_API

#endif
