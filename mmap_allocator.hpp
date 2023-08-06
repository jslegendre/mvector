#ifndef MVECTOR_MMAP_ALLOCATOR_HPP
#define MVECTOR_MMAP_ALLOCATOR_HPP

#include <string>
#include <memory>

#include "mapped_file.hpp"
#include "_shared.h"

template<typename T>
class mmap_allocator: public std::allocator<T> {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    
    mmap_allocator(const std::string& path, off_t offset = 0)
    : _mmfile(new mapped_file(path, offset)) { }
    
    T* allocate(size_t n) {
        if (n == 0) return nullptr;
        
        size_t new_size = (n * sizeof(T));
        if (likely(new_size > _mmfile->size())) {
            _mmfile->resize(new_size);
        }
        _mmfile->retain();
        return reinterpret_cast<T*>(_mmfile->data());
    }
    
    void deallocate (T* p, size_t n) {
        if (n == 0) return;
        _mmfile->release();
    }
    
    void construct(T* p, const T& value) {
        new (p) T(value);
    }
    
    template <class... _Args>
    void construct(pointer __p, _Args&&... __args) {
        if constexpr (sizeof...(__args) <= 0) {
            return;
        }
        
        #if _LIBCPP_STD_VER > 17
        std::construct_at(__p, std::forward<_Args>(__args)...);
        #else
        ::new ((void*)__p) T(std::forward<_Args>(__args)...);
        #endif
    }
    
    size_t size() {
        return _mmfile->size();
    }
    
    void resize_to_fit(size_t n) {
        size_t new_size = (n * sizeof(T));
        _mmfile->resize(new_size);
    }
    
private:
    mapped_file *_mmfile;
};

#endif // MVECTOR_MMAP_ALLOCATOR_HPP
