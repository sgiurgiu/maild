#ifndef SYNC_QUEUE_H
#define SYNC_QUEUE_H

#include <vector>

#include <mutex>
#include <condition_variable>

namespace maild
{
template < class T, class Container = std::vector<T> > class sync_queue
{
private:
  mutable std::mutex m;
  std::condition_variable condition;

protected:
  Container c;
public:
  explicit
      sync_queue(const Container& container = Container()):  c(container)
      { }
      sync_queue& operator=(const sync_queue&) = delete;
      sync_queue(const sync_queue& other) = delete;
      sync_queue& operator=(sync_queue&&) = delete;
      sync_queue(sync_queue&& other) = delete;

      typedef typename Container::value_type                value_type;
      typedef typename Container::reference                 reference;
      typedef typename Container::const_reference           const_reference;
      typedef typename Container::size_type                 size_type;
      typedef          Container                            container_type;
      typedef typename Container::iterator		    iterator;
      typedef typename Container::const_iterator	    const_iterator;
      typedef typename Container::reverse_iterator	    reverse_iterator;
      typedef typename Container::const_reverse_iterator    const_reverse_iterator;
      

      void clear()
      {
          std::lock_guard<std::mutex> lock(m);
          c.clear();
      }

      bool
      empty() const
      {
        std::lock_guard<std::mutex> lock(m);
        return c.empty();
      }

      size_type
      size() const
      {
        std::lock_guard<std::mutex> lock(m);
        return c.size();
      }

      void
      push(const value_type& x)
      {
        std::lock_guard<std::mutex> lock(m);
        c.push_back(x);
        condition.notify_one();
      }
      

      void
      push_front(const value_type& x)
      {
        std::lock_guard<std::mutex> lock(m);
        c.insert(c.begin(),x);
        condition.notify_one();
      }

      /**
       * Returns the first element in the collection
       * and removes it.
       */
      bool
      try_get_front_and_pop(reference ref)
      {
        std::lock_guard<std::mutex> lock(m);
        if(c.empty()) return false;
        auto it = c.begin();
        ref = *it;
        c.erase(it);
        return true;
      }

      /**
       * Returns the first element in the collection
       * and removes it. If the queue is empty it will
       * wait forever to be filled with an element.
       */
      void wait_and_pop(reference ref) {
        std::unique_lock<std::mutex> lock(m);
        condition.wait(lock, [&](){return !c.empty();});
        auto it = c.begin();
        ref = (*it);
        c.erase(it);
      }

      void
      swap(sync_queue& other)
      noexcept(noexcept(swap(c, other.c)))
      {
        std::lock_guard<std::mutex> lock(m);
        using std::swap;
        swap(c, other.c);
      }
};


}
#endif
