#include "active_object.h"

using namespace maild;

void active_object::run()
{
    while(!done) {
        message msg;
        messages.wait_and_pop(msg);
        msg();
    }
}

active_object::active_object():done(false)
{
    workingThread=std::thread([this]{
        this->run();
    });
}

active_object::~active_object()
{
    send([&]{
        done = true;
    });
    workingThread.join();//should really not have to wait long here
}

void active_object::send(message msg)
{
    messages.push(msg);
}

void active_object::clear()
{
    messages.clear();
}

bool active_object::empty() const
{
    return messages.empty();
}
