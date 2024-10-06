#pragma once
#include "../ServerLibrary.h"

template <typename T>
class Singleton {
private:
    static std::unique_ptr<T> _instance;
    static std::once_flag _initInstanceFlag;

    static void initSingleton()
    {
        _instance.reset(new T);
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

public:
    static T& getInstance() 
    {
        std::call_once(_initInstanceFlag, &Singleton::initSingleton);
        return *_instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

template <typename T> std::unique_ptr<T> Singleton<T>::_instance = nullptr;
template <typename T> std::once_flag Singleton<T>::_initInstanceFlag;

/* How To Use
class MySingleton : public Singleton<MySingleton> {
public:
    void sayHello() {
        std::cout << "Hello, Singleton!" << std::endl;
    }

    // Singleton Ŭ������ protected�� ������ ������ �����ϹǷ� 
    // friend ������ ���� Singleton�� MySingleton�� ������ �� �ְ� �ؾ� ��
    friend class Singleton<MySingleton>;
    
private:
    MySingleton() = default;  // �����ڸ� private���� ����
    ~MySingleton() = default;
};
*/