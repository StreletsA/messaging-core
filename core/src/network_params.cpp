#include "network_params.hpp"

Context::Context()
{
    context = new zmq::context_t(4);
}

Context& Context::getInstance()
{
    static Context instance;
    return instance;
}

zmq::context_t* Context::get_context()
{
    return context;
}