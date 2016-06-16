/*
 *This implementation is based on Herb Sutter's blog about Active objects:
 * http://herbsutter.com/2010/07/12/effective-concurrency-prefer-using-active-objects-instead-of-naked-threads/
 */
#ifndef ACTIVE_OBJECT_H
#define ACTIVE_OBJECT_H

#include <functional>
#include <thread>

#include "sync_queue.h"

namespace maild
{
class active_object
{
public:
    typedef std::function<void()> message;
    active_object ( const active_object& ) = delete;
    active_object ( active_object&& ) = delete;
    active_object& operator= ( const active_object& ) = delete;
    active_object& operator= (active_object&& ) = delete;

private:
    bool done;
    sync_queue<message> messages;
    std::thread workingThread;

private:
  void run();
public:
  active_object();
  ~active_object();
  void send(message msg);
  void clear();
  bool empty() const;
};
}
#endif // ACTIVE_OBJECT_H
