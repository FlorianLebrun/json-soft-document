
char* initMem(int size) {
   char* buf = (char*)malloc(size);
   memset(buf, 0, size);
   return buf;
}

int StackAllocator_size = 100000000;
char* StackAllocator_buffer = initMem(StackAllocator_size);


template <class T>
class StackAllocator : public std::allocator<T> {
public:
   typedef char byte;

    StackAllocator(byte *pool, int nPoolSize)
    {
    }
    StackAllocator(int n)
    {
    }
 
    StackAllocator()
    {
    }
    void Init()
    {
    }
    StackAllocator(const StackAllocator &obj) // copy constructor
    {
    }
private:
    void operator =(const StackAllocator &);
public:
    byte *m_pool;
    unsigned  m_nPoolSize;
 
    template <class _Other>
    StackAllocator(const StackAllocator<_Other> &other)
    {
        Init();
        m_pool= other.m_pool;
        m_nPoolSize = other.m_nPoolSize;
    }
 
    ~StackAllocator()
    {
    }
 
    template <class U>
    struct rebind
    {
        typedef StackAllocator<U> other ;
    };
 
 
    pointer
    address( reference r ) const
    {
        return &r;
    }
 
    const_pointer
    address( const_reference r ) const
    {
        return &r;
    }
 
    pointer
    allocate( size_type n, const void* /*hint*/=0 )
    {
      char* ptr = StackAllocator_buffer;
      StackAllocator_buffer += n*sizeof(T);
      return (pointer)ptr;
    }
 
    void
    deallocate( pointer p, size_type /*n*/ )
    {

    }
 
    void
    construct( pointer p, const T& val )
    {
        new (p) T(val);
    }
 
    void
    destroy( pointer p )
    {
    }
 
    size_type
    max_size() const
    {
        return ULONG_MAX / sizeof(T);
    }
 
};
 
 
template <class T>
bool
operator==( const StackAllocator<T>& left, const StackAllocator<T>& right )
{
    if (left.m_pool == right.m_pool)
    {
        return true;
    }
    return false;
}
 
template <class T>
bool
operator!=( const StackAllocator<T>& left, const StackAllocator<T>& right)
{
    if (left.m_pool != right.m_pool)
    {
        return true;
    }
    return false;
}
// </StackAllocator>