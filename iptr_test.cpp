#include <iostream>
#include "iptr.hpp"

class A /*: public RefCounted*/ {
public:
    A(int _x, int _y)
    : x{_x}, y{_y}
    { std::cout<< "A()\n"; }
    
    A(A const& other)
    : x{other.x+1}, y{other.y+1}
    { std::cout<<"A(A const&)\n"; }
    
    ~A() { std::cout << "~A()\n"; }
    
    int x = 1;
    int y = 2;
};

int main()
{
    /*
    iptr<A> ptr = nullptr;

    auto ip2 = make_iptr( new A(1,2) ); // refcount = 1
    ptr = ip2;                        // refcount = 2
    {
        auto ip3 = ip2;               // refcount = 3
        std::cout << ip2.refcount() << "\n";
    }
    std::cout <<ip2.refcount()<<"\n"; // refcount = 2
    ptr.~iptr();
    std::cout <<ip2.refcount()<<"\n"; // refcount = 1
    
    std::cout << ip2->x << ", "
              << ip2->y << "\n";
    */
    
    using B = MkRefCounted<A>;
    
    iptr<B> ptr = nullptr;
    ptr = make_intrusive<B>(1,2);
    std::cout << "#refs: " << ptr.refcount() << "\n";
    auto ptr2 = ptr;
    std::cout << "#refs: " << ptr.refcount() << "\n";
    auto ptr3 = ptr;
    std::cout << "#refs: " << ptr.refcount() << "\n";
    
    ptr3.reset(ptr2);
    std::cout << "#refs: " << ptr.refcount() << "\n";
    
    ptr3.reset();
    std::cout << "#refs: " << ptr.refcount() << "\n";
    
    ptr3.reset(std::move(ptr2));
    std::cout << "#refs: " << ptr.refcount() << "\n";
    
    static_assert( sizeof(B*) == sizeof(ptr), "NOPE");
}
