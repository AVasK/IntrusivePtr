// Intrusive Ptr (needs RefCounted class)

class RefCounted;

class RCAccessor {
protected:
    template <typename T>
    unsigned inc_refcount(T& obj) { return obj.inc_refcount(); }
    
    template <typename T>
    unsigned dec_refcount(T& obj) { return obj.dec_refcount(); }
    
    template <typename T>
    unsigned get_refcount(T& obj) { return obj.get_refcount(); }
};


template <typename T>
class iptr : protected RCAccessor {
public:
    // C'tor
    iptr(T * _ptr = nullptr)
    : ptr{_ptr}
    {
        if (ptr) RCAccessor::inc_refcount(*ptr);
    }
    
    // D'tor
    ~iptr()
    {
        if (ptr)
        {
            auto refcount = RCAccessor::dec_refcount(*ptr);
            if (refcount == 0)
            {
                delete ptr;
            }
        }
    }
    
    // Copy C'tor
    iptr (iptr const& other) : ptr{other.ptr}
    {
        if (ptr)
        {
            RCAccessor::inc_refcount(*ptr);
        }
    }
    
    // Copy assignment
    iptr& operator= (iptr const& other)
    {
        ptr = other.ptr;
        if (ptr)
        {
            RCAccessor::inc_refcount(*ptr);
        }
        
        return *this;
    }
    
    // Move C'tor:
    iptr (iptr && other) : ptr{other.ptr}
    {
        other.ptr = nullptr;
    }
    
    // Move assignment:
    iptr & operator= (iptr && other)
    {
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }
    
    // Indirection overload
    T * operator-> () const noexcept { return get(); }
    T & operator* () const { return *get(); }
    
    // Get # of references to object
    unsigned refcount()
    {
        return ptr
               ? RCAccessor::get_refcount(*get())
               : 0;
    }
    
    // Reset the pointer with the new value
    iptr& reset(T *new_ptr = nullptr) noexcept
    {
        auto old = ptr;
        ptr = new_ptr;
        if (old) RCAccessor::dec_refcount(*old);
        return *this;
    }
    
    iptr& reset(iptr const& other) noexcept
    {
        if (ptr == other.ptr)
            return *this;
        
        auto old = ptr;
        ptr = other.ptr;
        if (ptr) RCAccessor::inc_refcount(*ptr);
        if (old) RCAccessor::dec_refcount(*old);
        
        return *this;
    }
    
    iptr& reset(iptr && other) noexcept
    {
        auto old = ptr;
        ptr = other.ptr;
        if (old) RCAccessor::dec_refcount(*old);
        
        return *this;
    }
    
    // Detach a pointer
    T* detach() noexcept
    {
        auto old = ptr;
        ptr = nullptr;
        return old;
    }
    
    // Bool conversion = check for nullptr
    operator bool() const noexcept { return get() != nullptr; }
    
private:
    T * ptr;
    T * get() const noexcept { return ptr; }
};


template <typename T, typename... Args>
auto make_intrusive (Args&&... args) noexcept -> iptr<T>
{
    auto raw_ptr = ::new T(std::forward<Args>(args)...);
    return iptr<T>{raw_ptr};
}


class RefCounted {
public:
    RefCounted() : counter{0} {}
    RefCounted(RefCounted const&) : counter{0} {}
    RefCounted(RefCounted && moved) : counter{moved.counter} {}
private:
    friend class RCAccessor;
    inline unsigned inc_refcount() const { return ++counter; }
    inline unsigned dec_refcount() const { return --counter; }
    inline unsigned get_refcount() const { return counter; }
    mutable unsigned counter;
};


template <typename T>
class MkRefCounted : public T, public RefCounted {
public:
    template <typename... Args>
    MkRefCounted (Args... args) : RefCounted(), T(args...) {}
    
    // Add some Copy & Move here & you're welcome!
};
