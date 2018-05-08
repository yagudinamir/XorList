#include "StackAllocator.h"
#include <memory>
#include <iostream>
#include <random>
#include <ctime>
#include <list>

template< typename T >
class Iterator;

template< typename T, typename Allocator >
class XorList;

template< typename T >
class Node {
private:
    Node* xor_;
    T value;
public:
    template< typename U >
    friend class Iterator;

    template< typename U, typename Allocator>
    friend class XorList;

    static Node* Xor(Node* left, Node* right) {
        return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(left) ^ reinterpret_cast<uintptr_t>(right));
    }

    explicit Node(const  T& value_) : xor_(nullptr), value(value_) {}
    explicit Node(T&& value_) : xor_(nullptr), value(std::move(value_)) {}
    Node& operator= (const Node& other) {
        xor_ = other.xor_;
        value = other.value;
    }

    Node(const T& value_, Node<T>* l, Node<T>* r) : value(value_) {
        xor_ = Xor(l, r);
    }

    Node(const T&& value_, Node<T>* l, Node<T>* r) : value(value_) {
        xor_ = Xor(l, r);
    }
};

template <typename T>
class Iterator : public std::iterator< std::bidirectional_iterator_tag, T> {
private:
    Node<T>* previous;
    Node<T>* current;
    Iterator(Node<T>* previous_ = nullptr, Node<T>* current_ = nullptr) : previous(previous_), current(current_) {}
public:
    template < typename U, typename Allocator>
    friend class XorList;

    Iterator(const Iterator& other) : previous(other.previous), current(other.current) {}

    Iterator& operator= (const Iterator& other) {
        previous = other.previous;
        current = other.current;
        return *this;
    }

    bool operator!= (const Iterator& other) const {
        return (previous != other.previous) || (current != other.current);
    }

    bool operator== (const Iterator& other) const {
        return (previous == other.previous) && (current == other.current);
    }

    T& operator*() {
        return current->value;
    }

    T* operator->() {
        return &(current->value);
    }

    Iterator& operator++() {
        Node<T>* next = Node<T>::Xor(current->xor_, previous);
        previous = current;
        current = next;
        return *this;
    }

    Iterator operator++(int) {
        Iterator<T> it = *this;
        Node<T>* next = Node<T>::Xor(current->xor_, previous);
        previous = current;
        current = next;
        return it;
    }

    Iterator<T>& operator--() {
        Node<T>* before = Node<T>::Xor(previous->xor_, current);
        current = previous;
        previous = before;
        return *this;
    }

    Iterator<T> operator--(int) {
        Iterator<T> it = *this;
        Node<T>* before = Node<T>::Xor(previous->xor_, current);
        current = previous;
        previous = before;
        return it;
    }
};


template< typename T, typename Allocator >
class XorList {
public:
    typedef size_t size_type;
    typedef Iterator<T> iterator;
    typedef std::reverse_iterator<Iterator<T>> r_iterator;
    typedef Iterator<const T> const_iterator;
    typedef std::reverse_iterator<const_iterator> const_r_iterator;

    explicit XorList( const Allocator& alloc = Allocator()) : size_(0), begin_(nullptr), end_(nullptr), allocator(alloc) {
    }

    explicit XorList(size_type count, const T& value = T(), const Allocator& alloc = Allocator()) : size_(count), allocator(alloc) {
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }

    XorList(const XorList& other) {
        allocator = other.allocator;
        for (auto it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }

    XorList(XorList&& other) {
        allocator = std::move(other.allocator);
        begin_ = std::move(other.begin_);
        end_ = std::move(other.end_);
        other.begin_ = nullptr;
        other.end_ = nullptr;
        other.size_ = 0;
        size_ = other.size_;
    }

    ~XorList() {
        while(size_ > 0) {
            pop_back();
        }
        /*std::cout << "destroyed" << std::endl;*/
    }

    XorList& operator=(const XorList& other) {
        while (size_ != 0) {
            pop_back();
        }
        for (auto it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
        allocator = other.allocator;
        return *this;
    }

    XorList& operator=(XorList&& other) {
        while (size_ != 0) {
            pop_back();
        }
        begin_ = std::move(other.begin_);
        end_= std::move(other.end_);
        size_ = other.size_;
        allocator = std::move(other.allocator);
        other.begin_ = nullptr;
        other.end_  = nullptr;
        other.size_ = 0;
        return *this;
    }

    size_type size() {
        return  size_;
    }

    iterator begin() const {
        return iterator(nullptr, begin_);
    }

    iterator end() {
        return iterator(end_, nullptr);
    }

    const_iterator cbegin() const {
        return const_iterator(nullptr, begin_);
    }

    const_iterator cend() const {
        return const_iterator(end_, nullptr);
    }

    r_iterator rbegin() {
        return r_iterator(this->end());
    }

    r_iterator rend() {
        return r_iterator(this->begin());
    }

    const_r_iterator crbegin() {
        return const_r_iterator(this->begin());
    }

    const_r_iterator crend() {
        return const_r_iterator(this->end());
    }

    void push_back(const T& value) {
        Node<T>* new_node =  std::allocator_traits<M_Allocator>::allocate(allocator, 1);
//        new(new_node) Node<T>(value, end_, nullptr);
        std::allocator_traits<M_Allocator>::construct(allocator, new_node, value, end_, nullptr);
        if(end_ != nullptr)
            end_->xor_ = Node<T>::Xor(end_->xor_, new_node);
        size_++;
        end_= new_node;
        if (size_ == 1) {
            begin_ = end_;
        }
    }

    void push_back(T&& value) {
        Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
//        new(new_node) Node<T>(std::move(value), end_, nullptr);
        std::allocator_traits<M_Allocator>::construct(allocator, new_node, std::move(value), end_, nullptr);
        if (end_!= nullptr)
            end_->xor_ =Node<T>::Xor(end_->xor_, new_node);
        size_++;
        end_= new_node;
        if (size_ == 1) {
            begin_ = end_;
        }
    }

    void push_front(const T& value) {
        Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
//        new(new_node) Node<T>(value, nullptr, begin_);
        std::allocator_traits<M_Allocator>::construct(allocator, new_node, value, nullptr, begin_);
        if (begin_ != nullptr)
            begin_->xor_ = Node<T>::Xor(begin_->xor_, new_node);
        size_++;
        begin_ = new_node;
        if (size_ == 1) {
            end_= begin_;
        }
    }

    void push_front(T&& value) {
        Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
        std::allocator_traits<M_Allocator>::construct(allocator, new_node, std::move(value), nullptr, begin_);
//        new(new_node) Node<T>(std::move(value), nullptr, begin_);
        if (begin_ != nullptr)
            begin_->xor_ = Node<T>::Xor(begin_->xor_, new_node);
        begin_ = new_node;
        if (size_ == 1) {
            end_= begin_;
        }
    }

    void pop_back() {
        Node<T>* last = end_;
        Node<T>* pre_last = end_->xor_;
        if (pre_last != nullptr)
            pre_last->xor_ = Node<T>::Xor(pre_last->xor_, last);
        size_--;
        end_->~Node<T>();
        std::allocator_traits<M_Allocator>::deallocate(allocator, end_, 1);
        end_= pre_last;
    }

    void pop_front() {
        Node<T>* first = begin_;
        Node<T>* post_first = begin_->xor_;
        if (post_first != nullptr)
            post_first->xor_ = Node<T>::Xor(post_first->xor_, first);
        size_--;
        begin_->~Node<T>();
        std::allocator_traits<M_Allocator>::deallocate(allocator, begin_, 1);
        begin_ = post_first;
    }

    iterator insert_before(iterator it, const T& value) {
        if (it == this->begin()) {
            Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
            std::allocator_traits<M_Allocator>::construct(allocator, new_node, value, nullptr, it.current);
//            new(new_node) Node<T>(value, nullptr, it.current);
            //Node<T>* new_node = ptr;
            size_++;
            if (it.current != nullptr)
                it.current->xor_ = Node<T>::Xor(it.current->xor_, new_node);
            begin_ = new_node;
            if (size_ == 1)
                end_ = begin_;
            return iterator(nullptr, new_node);
        } else {
            Node<T>* left = it.previous;
            Node<T>* right = it.current;
            Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
            std::allocator_traits<M_Allocator>::construct(allocator, new_node, value, left, right);
            size_++;
            if (right != nullptr) {
                right->xor_ = Node<T>::Xor(right->xor_, left);
                right->xor_ = Node<T>::Xor(right->xor_, new_node);
            }
            if (left != nullptr) {
                left->xor_ = Node<T>::Xor(left->xor_, right);
                left->xor_ = Node<T>::Xor(left->xor_, new_node);
            }
            if (right == nullptr) {
                end_ = new_node;
            }
            return iterator(left, new_node);
        }
    }

    iterator insert_before(iterator it, T&& value) {
        if (it == this->begin()) {
            Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
            std::allocator_traits<M_Allocator>::construct(allocator, new_node, std::move(value), nullptr, it.current);
//            new(new_node) Node<T>(std::move(value), nullptr, it.current);
            //Node<T>* new_node = ptr;
            size_++;
            if (it.current != nullptr)
                it.current->xor_ = Node<T>::Xor(it.current->xor_, new_node);
            begin_ = new_node;
            if (size_ == 1)
                end_ = begin_;
            return iterator(nullptr, new_node);
        } else {
            Node<T>* left = it.previous;
            Node<T>* right = it.current;
            Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
            std::allocator_traits<M_Allocator>::construct(allocator, new_node, std::move(value), left, right);
//            new(new_node) Node<T>(std::move(value), left, right);
            size_++;
            if (right != nullptr) {
                right->xor_ = Node<T>::Xor(right->xor_, left);
                right->xor_ = Node<T>::Xor(right->xor_, new_node);
            }
            if (left != nullptr) {
                left->xor_ = Node<T>::Xor(left->xor_, right);
                left->xor_ = Node<T>::Xor(left->xor_, new_node);
            }
            if (right == nullptr) {
                end_ = new_node;
            }
            return iterator(left, new_node);
        }
    }

    iterator insert_after(iterator it, const T& value) {
        if (it.current == end_) {
            Node<T>* last = it.current;
            Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
            std::allocator_traits<M_Allocator>::construct(allocator, new_node, value, last, nullptr);
//            new(new_node) Node<T>(value, last, nullptr);
            if (last != nullptr)
                last->xor_ = Node<T>::Xor(last->xor_, new_node);
            end_= new_node;
            size_++;
            if (size_ == 1)
                begin_ = end_;
            return iterator(last, new_node);
        } else {
            it++;
            Node<T>* left = it.previous;
            Node<T>* right = it.current;
            Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
            std::allocator_traits<M_Allocator>::construct(allocator, new_node, value, left, right);
//            new(new_node) Node<T>(value, left, right);
            if (left != nullptr) {
                left->xor_ = Node<T>::Xor(left->xor_, right);
                left->xor_ = Node<T>::Xor(left->xor_, new_node);
            }
            if (right != nullptr) {
                right->xor_ = Node<T>::Xor(right->xor_, left);
                right->xor_ = Node<T>::Xor(right->xor_, new_node);
            }
            size_++;
            return iterator(left, new_node);
        }
    }

    iterator insert_after(iterator it, T&& value) {
        if (it.current == end_) {
            Node<T>* last = it.current;
            Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
            std::allocator_traits<M_Allocator>::construct(allocator, new_node, std::move(value), last, nullptr);
//            new(new_node) Node<T>(std::move(value), last, nullptr);
            if (last != nullptr)
                last->xor_ = Node<T>::Xor(last->xor_, new_node);
            end_= new_node;
            size_++;
            if (size_ == 1)
                begin_ = end_;
            return iterator(last, new_node);
        } else {
            it++;
            Node<T>* left = it.previous;
            Node<T>* right = it.current;
            Node<T>* new_node = std::allocator_traits<M_Allocator>::allocate(allocator, 1);
            std::allocator_traits<M_Allocator>::construct(allocator, new_node, std::move(value), left, right);
//            new(new_node) Node<T>(std::move(value), left, right);
            if (left != nullptr) {
                left->xor_ = Node<T>::Xor(left->xor_, right);
                left->xor_ = Node<T>::Xor(left->xor_, new_node);
            }
            if (right != nullptr) {
                right->xor_ = Node<T>::Xor(right->xor_, left);
                right->xor_ = Node<T>::Xor(right->xor_, new_node);
            }
            size_++;
            return iterator(left, new_node);
        }
    }

    void erase(iterator it) {
        Node<T>* left = it.previous;
        Node<T>* right = Node<T>::Xor(it.current->xor_, left);
        if (left != nullptr) {
            left->xor_ = Node<T>::Xor(left->xor_, it.current);
            left->xor_ = Node<T>::Xor(left->xor_, right);
        }
        if (right != nullptr) {
            right->xor_ = Node<T>::Xor(right->xor_, it.current);
            right->xor_ = Node<T>::Xor(right->xor_, left);
        }
        if (right == nullptr) {
            end_ = left;
        }
        if (left == nullptr) {
            begin_ = right;
        }
        --size_;
        it.current->~Node<T>();
        std::allocator_traits<M_Allocator>::deallocate(allocator, it.current, 1);
    }

private:
    typedef typename Allocator::template rebind< Node<T> >::other M_Allocator;
    typename Allocator::template rebind< Node<T> >::other allocator;
    size_type size_;
    Node<T>* begin_;
    Node<T>* end_;

};