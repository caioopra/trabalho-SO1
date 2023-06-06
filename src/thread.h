#ifndef thread_h
#define thread_h

#include <chrono>
#include <ctime>
#include <queue>

#include "cpu.h"
#include "debug.h"
#include "list.h"
#include "traits.h"

__BEGIN_API

class Thread {
   protected:
    typedef CPU::Context Context;

   public:
    typedef Ordered_List<Thread> Ready_Queue;
    typedef Ordered_List<Thread> Suspended_Queue;
    typedef Ordered_List<Thread> Sleep_queue;
    // Thread State
    enum State {
        RUNNING,
        READY,
        FINISHING,
        SUSPENDED,
        WAITING
    };

    /*
     * Construtor vazio. Necessário para inicialização, mas sem importância para a execução das Threads.
     */
    Thread() {}

    /*
     * Cria uma Thread passando um ponteiro para a função a ser executada
     * e os parâmetros passados para a função, que podem variar.
     * Cria o contexto da Thread.
     * PS: devido ao template, este método deve ser implementado neste mesmo arquivo .h
     */
    template <typename... Tn>
    Thread(void (*entry)(Tn...), Tn... an);

    /*
     * Retorna a Thread que está em execução.
     */
    static Thread* running() { return _running; }

    /*
     * Método para trocar o contexto entre duas thread, a anterior (prev)
     * e a próxima (next).
     * Deve encapsular a chamada para a troca de contexto realizada pela class CPU.
     * Valor de retorno é negativo se houve erro, ou zero.
     */
    static int switch_context(Thread* prev, Thread* next);

    /*
     * Termina a thread.
     * exit_code é o código de término devolvido pela tarefa (ignorar agora, vai ser usado mais tarde).
     * Quando a thread encerra, o controle deve retornar à main.
     */
    void thread_exit(int exit_code);

    /*
     * Retorna o ID da thread.
     */
    int id();

    /*
     * Daspachante (disptacher) de threads.
     * Executa enquanto houverem threads do usuário.
     * Chama o escalonador para definir a próxima tarefa a ser executada.
     */
    static void dispatcher();

    /*
     * Realiza a inicialização da class Thread.
     * Cria as Threads main e dispatcher.
     */
    static void init(void (*main)(void*));

    /*
     * Devolve o processador para a thread dispatcher que irá escolher outra thread pronta
     * para ser executada.
     */
    static void yield();

    /*
     * Destrutor de uma thread. Realiza todo os procedimentos para manter a consistência da classe.
     */
    ~Thread();

    /*
     * Qualquer outro método que você achar necessário para a solução.
     */

    Context* context();

    /*
     * Suspende thread até que a thread "alvo" finalize
     */
    int join();

    // suspende thread até que resume seja chamado
    void suspend();

    // coloca thread suspensa de volta na fila de prontos
    void resume();

    void sleep(Sleep_queue *sleep_queue);

    void wakeup();

   private:
    int _id;
    Context* volatile _context;
    static Thread* _running;
    Thread* _joining;

    static Thread _main;
    static CPU::Context _main_context;
    static Thread _dispatcher;
    static Ready_Queue _ready;
    Ready_Queue::Element _link;
    volatile State _state;
    static Suspended_Queue _suspended;     // fila de threads suspensas esperando o fim da thread

    /*
     * Qualquer outro atributo que você achar necessário para a solução.
     */

    static unsigned int _thread_count;
    int _exit_code;
    Sleep_queue* _sleep_queue;      
};

// construtor das Threads
// faz a definição de _link, passando ponteiro para thread e tempo que foi criada
template <typename... Tn>
Thread::Thread(void (*entry)(Tn...), Tn... an) : _link(this, (std::chrono::duration_cast<std::chrono::microseconds>
    (std::chrono::high_resolution_clock::now().time_since_epoch()).count()))/* inicialização de _link */
{
    db<Thread>(TRC) << " - Thread (" << _thread_count << ") criada \n";
    this->_context = new CPU::Context(entry, an...);
    this->_id = Thread::_thread_count++;
    this->_state = READY;

    if (_id > 0) {
        _ready.insert(&_link);
    }
}

__END_API

#endif
