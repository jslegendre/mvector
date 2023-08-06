#ifndef MVECTOR_MAPPED_FILE_HPP
#define MVECTOR_MAPPED_FILE_HPP

#include <atomic>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "_shared.h"

struct mapped_file {
public:
    mapped_file(const std::string& path, off_t offset = 0)
    : _path(path), _offset(offset) {
        struct stat st;
        if (stat(_path.c_str(), &st) != 0) {
            FILE *ff = fopen(path.c_str(), "w+b");
            if (ff == nullptr) {
                throw std::runtime_error("Could not create file: " + _path);
            }
            fclose(ff);
            _size = 1;
            return;
        }
        
        _size = st.st_size;
    }
    
    ~mapped_file() {
        if (likely(ref_count() > 0)) {
            return;
        }
        close();
    }
    
    void resize(size_t new_size) {
        (void)data();
        ftruncate(_handle, new_size);
        _size = new_size;
    }
    
    size_t size() {
        return _size;
    }
    
    void close() {
        if (unlikely(handle() == -1)) return;
        msync(_data, _size, MS_SYNC);
        fsync(_handle);
        munmap(_data, _size);
        ::close(_handle);
        
        set_handle(-1);
        _size = 0;
        _data = nullptr;
        _offset = 0;
    }
    
    char *data() {
        if (handle() == -1) {
            int handle = open(_path.c_str(), O_RDWR, 0644);
            if (unlikely(handle == -1)) {
                throw std::runtime_error("Could not open file: " + _path);
            }
            set_handle(handle);
            fcntl(_handle, F_SETFD, FD_CLOEXEC);
            
            void *data = mmap(nullptr, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _handle, _offset);
            if (unlikely(data == MAP_FAILED)) {
                throw std::runtime_error("Could not map file: " + _path);
            }
            
            _data = static_cast<char*>(data);
        }
        
        return _data;
    }
    
    inline void retain() {
       std::atomic_fetch_add_explicit(&_ref_count, 1, std::memory_order_release);
    }
    
    inline void release() {
        int r = std::atomic_fetch_sub_explicit(&_ref_count, 1, std::memory_order_release);
        if (unlikely(r-1 == 0)) {
            delete this;
        }
    }

private:
    std::atomic<int> _ref_count = 0;
    const std::string _path;
    size_t _size = 0;
    off_t _offset = 0;
    char *_data = nullptr;
    std::atomic<int> _handle = -1;
    
    inline int handle() {
        return std::atomic_load_explicit(&_handle, std::memory_order_acquire);
    }
    
    inline void set_handle(int handle) {
        std::atomic_store_explicit(&_handle, handle, std::memory_order_release);
    }
    
    inline int ref_count() {
        return std::atomic_load_explicit(&_ref_count, std::memory_order_acquire);
    }
};


#endif // MVECTOR_MAPPED_FILE_HPP
