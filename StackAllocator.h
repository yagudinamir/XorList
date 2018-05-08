#include <iostream>
#include <memory>
#include <cstdlib>
#include <list>
#include <ctime>

class Block {
public:
    typedef char* pointer;
    typedef std::size_t size_type;
    friend class SharedAllocator;
    Block (Block* previous_, const size_type n) {
        previous = previous_;
        unused_memory = n;
        used_memory = 0;
        current_memory_pointer = reinterpret_cast<char*>(std::malloc(n));
    }

    ~Block() {
        std::free(current_memory_pointer);
    }

    pointer get_current_pointer() {
        return current_memory_pointer + used_memory;
    }

    char* allocate(size_type n) {
        pointer p = get_current_pointer();
        unused_memory -= n;
        used_memory += n;
        return p;
    }

    void deallocate(size_type n) {
        return;
    }

private:
    pointer current_memory_pointer;
    size_type unused_memory;
    size_type used_memory;
    Block* previous;
};

class SharedAllocator {
private:
    Block* LastBlock;
public:
    const std::size_t block_size = 4096;

    SharedAllocator() {
        LastBlock = nullptr;
    }

    ~SharedAllocator() {
        Block* current;
        while (LastBlock->previous != nullptr) {
            current = LastBlock->previous;
            delete LastBlock;
            LastBlock = current;
        }
        delete LastBlock;
    }

    template <typename T>
    T* allocate(std::size_t n) {
        if (LastBlock == nullptr) {
            Block* b = new Block(LastBlock, std::max(n * sizeof(T), block_size));
            LastBlock = b;
            return reinterpret_cast<T*>(LastBlock->allocate(n * sizeof(T)));
        } else if (LastBlock->unused_memory < n * sizeof(T)) {
            Block* b = new Block(LastBlock, std::max(n * sizeof(T), block_size));
            LastBlock = b;
            return reinterpret_cast<T*>(LastBlock->allocate(n * sizeof(T)));
        } else {
            return reinterpret_cast<T*>(LastBlock->allocate(n * sizeof(T)));
        }
    }

    void deallocate() {
        return;
    }
};


template <typename T>
class StackAllocator : public std::allocator<T> {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    template< class U > struct rebind { typedef StackAllocator<U> other; };

    StackAllocator(): m_allocator(std::make_shared<SharedAllocator>()) { }

    template <typename U>
    StackAllocator& operator=(const StackAllocator<U>& other) {
        m_allocator = other.m_allocator;
        return *this;
    }

    template <typename U>
    StackAllocator& operator=(StackAllocator<U>&& other) {
        m_allocator = std::move(other.m_allocator);
        return *this;
    }

    template<class U>
    StackAllocator(const StackAllocator<U>& other) {
        m_allocator = other.m_allocator;
    }

    ~StackAllocator() { }

    pointer allocate(size_type n, const_pointer hint = 0) {
        return m_allocator->allocate<T>(n);
    }

    void deallocate(pointer p, size_type n) {
        return;
    }

    std::shared_ptr<SharedAllocator> m_allocator;
};