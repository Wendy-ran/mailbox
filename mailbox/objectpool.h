#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

// #include <QList>
#include <QMutex>
#include <QMutexLocker>
//#include <QVector>
#include <QMap>

template <typename T>
class ObjectPool {
public:
    static ObjectPool<T>& getInstance();

    T* acquire(const QString& name);  // 从池中获取一个对象
    void release(const QString& name, T* object);  // 删除并释放所有资源
    //void deleteObject(T* object);
    //void expandPoolSize(int size);  // 管理扩展池大小，size 表示新增加的对象数量

private:
    //QVector<T*> pool;
    QMap<QString, T*> pool;
    QMutex mutex;  // 互斥锁，确保 ObjectPool 类在多线程环境中的线程安全
    //void initialize(int size);

    ObjectPool();
    ~ObjectPool();
    ObjectPool(const ObjectPool&) = delete;            // 删除拷贝构造函数
    ObjectPool& operator=(const ObjectPool&) = delete; // 删除赋值操作符
};

//#include "ObjectPool.tpp"

#endif // OBJECTPOOL_H

// 使用单例模式的 ObjectPool
// auto& pool = ObjectPool<newSocket>::getInstance();
// newSocket* socket = pool.acquire("socket1");
// // 使用 socket...
// pool.release("socket1", socket);


