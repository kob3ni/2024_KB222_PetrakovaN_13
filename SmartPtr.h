#ifndef SMARTPTR_H
#define SMARTPTR_H

template<class T>
class SmartPtr
{
private:
    T* ptr;

public:
    SmartPtr(T* ptr)
    {
        this->ptr = ptr;
    }

    ~SmartPtr()
    {
        delete ptr;
    }

    T& operator *()
    {
        return *ptr;
    }

    T* operator ->()
    {
        return ptr;
    }
};

#endif // SMARTPTR_H
