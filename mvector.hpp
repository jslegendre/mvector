#ifndef MVECTOR_MVECTOR_HPP
#define MVECTOR_MVECTOR_HPP

#include "mmap_allocator.hpp"
#include <vector>

#include "_shared.h"

template <typename T, typename A = mmap_allocator<T> >
class mvector: public std::vector<T, A> {
public:
    typedef std::vector<T, A> Base;

    typedef typename Base::const_iterator const_iterator;
    typedef typename Base::iterator iterator;
    typedef T value_type;
    typedef A allocator_type;

    mvector(const mvector<T, A> &other): Base(other) {}

    explicit mvector(size_t n)
    : Base() {
        Base::reserve(n);
    }

    explicit mvector(A alloc): Base(alloc) {}

    mvector(iterator from, iterator to): Base(from, to) {}

    template <typename Iter>
    mvector(Iter first, Iter last, A a = A())
    : Base(first, last, a) {}

    mvector(int n, T val, A alloc)
    : Base(n, val, alloc) {}

    mvector(int n, T val)
    : Base(n, val) {}

    mvector(std::vector<T, std::allocator<T>> v)
    : std::vector<T, std::allocator<T>>(std::move(v)) {}
    
    mvector(const std::string& path, off_t offset = 0)
    : Base(0, 0, mmap_allocator<T>(path, offset)) {
        size_t _s = Base::get_allocator().size();
        if (likely(_s >= sizeof(T))) {
            size_t s = _s / sizeof(T);
            Base::reserve(s);
            Base::resize(s);
        }
    }
    
    ~mvector() {
        Base::get_allocator().resize_to_fit(Base::size());
    }
};

#endif // MVECTOR_MVECTOR_HPP
