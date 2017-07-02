#include <vector>
#include <queue>

using std::queue;
using std::vector;

/*
 * template class ObjectPool
 *
 * Provides an object pool that can be used with any class that provides a
 * default constructor
 *
 *
 *
 *
 */

namespace OPool
{

template <typename T>
class ObjectPool
{
public:
    // Create an object pool with chunkSize objects.
    // Whenever the object pool runs out of objects, chunkSize more objects will
    // be added to the pool. The pool only grows: objects are never removed from
    // the pool (freed), until the pool is destroyed.
    //
    // Throws invalid_argument if chunkSize is <= 0.
    //
    ObjectPool(int chunkSize = defaultChunkSize) throw(std::invalid_argument, std::bad_alloc);

    //
    // Frees all the allocated objects. Invalidates any objects that have been
    // acquired for use.
    //
    ~ObjectPool();

    // 
    // Reserve an object for use. The reference to the object is invalidated if
    // the object pool itself is freed.
    //
    // Clients must not free the object!
    //
    T& acquireObject();

    // 
    // Return the object to the pool. Clients must not use the object after it
    // has been returned to the pool.
    //
    void releaseObject(T& obj);

protected:
    // 
    // freeList stores the objects that are not currently in use by clients.
    //
    queue<T*> freeList;

    // 
    // allObjects stores pointers to all the objects, in use or not. This vector
    // is needed in order to ensure that all objects are freed properly in the
    // destructor.
    //
    vector<T*> allObjects;

    int chunkSize;
    static const int defaultChunkSize = 30;

    // 
    // Allocates chunckSize new objects and adds them to the freeList
    //
    void allocateChunk();
    static void arrayDeleteObject(T* obj);

private:
    // Prevent assignment and pass-by-value
    ObjectPool(const ObjectPool<T>& src);
    ObjectPool<T>& operator=(const ObjectPool<T>& rhs);
};

template <typename T>
const int ObjectPool<T>::defaultChunkSize;

template <typename T>
ObjectPool<T>::ObjectPool(int chunkSize) throw(std::invalid_argument, std::bad_alloc) : chunkSize(chunkSize)
{
    if (chunkSize <= 0)
    {
        throw std::invalid_argument("chunk size must be positive");
    }
    // create chunckSize objects to start
    allocateChunk();
}

/* 
* Allocates an array of chunckSize objects because that's more efficient than
* allocating each of them individually.
* Stores a pointer to the first element of the array in the allObjects vector.
* Adds a pointer to each new object to the freeList.
*/
template <typename T>
void ObjectPool<T>::allocateChunk()
{
    T* newObjects = new T[chunkSize];
    allObjects.push_back(newObjects);
    for (int i = 0; i < chunkSize; ++i) {
        freeList.push(&newObjects[i]);
    }
}

/*
 * Freeing function for use in the for_each algorithm in the destructor.
 */
template <typename T>
void ObjectPool<T>::arrayDeleteObject(T* obj)
{
    delete [] obj;
}

template <typename T>
ObjectPool<T>::~ObjectPool()
{
    // free each of the allocation chunks
    for_each(allObjects.begin(), allObjects.end(), arrayDeleteObject);
}

template <typename T>
T& ObjectPool<T>::acquireObject()
{
    if (freeList.empty()) {
        allocateChunk();
    }
    T* obj = freeList.front();
    freeList.pop();
    return (*obj);
}

template <typename T>
void ObjectPool<T>::releaseObject(T& obj)
{
    freeList.push(&obj);
}



}
