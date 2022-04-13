#include <zmq.hpp>

class Context
{

private: 
    Context();
    Context( const Context& );  
    Context& operator=( Context& );

    zmq::context_t * context;
  public:
    static Context& getInstance();

    zmq::context_t * get_context();

};