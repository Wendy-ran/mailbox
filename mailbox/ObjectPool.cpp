#include "ObjectPool.h"
#include "newsocket.h"
#include "newdatabase.h"

template <typename T>
ObjectPool<T>::ObjectPool() {
    //initialize(initialSize);
}

template <typename T>
ObjectPool<T>::~ObjectPool() {
    QMutexLocker locker(&mutex);
    qDeleteAll(pool);  // 删除所有数据结构中所有指针指向的对象
    pool.clear();
}

template<typename T>
ObjectPool<T> &ObjectPool<T>::getInstance()
{
    static ObjectPool<T> instance;
    return instance;
}

// template <typename T>
// void ObjectPool<T>::initialize(int size) {
//     for (int i = 0; i < size; ++i) {
//         pool.append(new T());
//     }
// }

template <typename T>
T* ObjectPool<T>::acquire(const QString& name) {
    QMutexLocker locker(&mutex);
    // if (pool.isEmpty()) {
    //     expandPoolSize(1);
    // }
    // return pool.takeFirst();
    if (!pool.contains(name)) {
        pool[name] = new T();
    }
    return pool[name];
}

template <typename T>
void ObjectPool<T>::release(const QString& name, T* object) {
    QMutexLocker locker(&mutex);
    //pool.append(object);
    if (pool.contains(name) && pool[name] == object) {
        pool.remove(name);
        delete object;
    }
}

// template<typename T>
// void ObjectPool<T>::deleteObject(T *object)
// {

// }

// template <typename T>
// void ObjectPool<T>::expandPoolSize(int size) {
//     for (int i = 0; i < size; ++i) {
//         pool.append(new T());
//     }
// }

// Explicit template instantiation
//template class ObjectPool<MyClass>;  // Replace 'MyClass' with actual types you are using
// template class ObjectPool<int>;
// template class ObjectPool<QString>;
template class ObjectPool<NewSocket>;  // 解决模板类成员在链接时因为未被实例化而导致的“undefined reference”错误
template class ObjectPool<newDatabase>;  // 不知道为什么改成指针类型的数据结构成员就是会失败
