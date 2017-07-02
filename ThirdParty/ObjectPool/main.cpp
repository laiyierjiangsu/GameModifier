#include <iostream>
#include "ObjectPool.hpp"

class UserRequest
{
public:
    UserRequest() {}
    ~UserRequest() {}

};

UserRequest& obtainUserRequest(OPool::ObjectPool<UserRequest>& pool)
{
    UserRequest& request = pool.acquireObject();
    std::cout << "user request" << std::endl;
    return request;
}

void processUserRequest(OPool::ObjectPool<UserRequest>& pool, UserRequest& req)
{
    std::cout << "releaseObject" << std::endl;
    pool.releaseObject(req);
}

int main()
{
    OPool::ObjectPool<UserRequest> requestPool(1000);

    while (true) {
        UserRequest& req = obtainUserRequest(requestPool);
        processUserRequest(requestPool, req);
    }
    return 0;
}
