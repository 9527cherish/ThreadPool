#pragma once 
#include <memory>


class Any
{
public:
    Any() = default;
    ~Any() = default;
    // unique_ptr禁用了拷贝构造和赋值
    Any(const Any&) = delete;
    Any& operator=(const Any&) = delete;

    Any(Any&&) = default;
    Any& operator=(Any&&) = default;
    
    // 构造函数，子类指向父类
    template<typename T>
    Any(T data) : m_basePtr(std::make_unique<Derive<T>>(data)){}
    
    // 将Any转化为T
    template<typename T>
    T cast_()
    {
        Derive<T>* ptr = dynamic_cast<Derive<T>*>(m_basePtr.get());
        if(nullptr == ptr)
        {
            throw "转换类型不匹配";
        }
        return ptr->m_data;
    }


private:
    class Base
    {
    public:
        // 当父类指针指向子类 对象时，若父类析构函数未声明为虚函数，
        // 析构时只会调用父类的析构函数，子类的析构函数不会被调用，可能导致资源泄漏；
        // 若父类析构函数声明为虚函数，则会先调用子类的析构函数，再调用父类的析构函数，确保所有资源被正确释放。
        virtual ~Base() = default;
    };

    template<typename T>
    class Derive : public Base
    {
    public:
        Derive(T data) : m_data(data){};
    // private:
        T m_data;
    };

    std::unique_ptr<Base> m_basePtr;
};