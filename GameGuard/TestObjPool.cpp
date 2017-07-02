#include "TestObjPool.h"
//#include "ThirdParty/generic_object_pool/include/cfood/generic_object_pool.h"
#include <memory.h>
#include <iostream>
#include "ThirdParty/ObjectPool/ObjectPool.hpp"
#include <windows.h>
#pragma  comment(lib,"Winmm.lib")
class UserRequest
{
public:
	UserRequest() {}
	~UserRequest() {}
	int GetID()
	{
		return m_iID;
	}
	void SetID(int iID)
	{
		m_iID = iID;
	}
private:
	int m_iID;


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


//class SomeObject : public cfood::PoolableObject {
//public:
//	SomeObject() {
//		printf("SomeObject::SomeObject()\n");
//	}
//	~SomeObject() {
//		printf("SomeObject::~SomeObject()\n");
//	}
//	void foo() {
//		printf("SomeObject::foo()\n");
//	}
//};
//typedef cfood::GenericObjectPool<SomeObject> PoolType;
//typedef cfood::PoolableObjectFactory<SomeObject> FactoryType;
TestObjPool::TestObjPool()
{
}

TestObjPool::~TestObjPool()
{
}

void TestObjPool::Test()
{
	TestObjPool1();
}

void TestObjPool::TestgenericPool()
{
	/*
	size_t max_idle = 5;
	FactoryType* pFactory = new FactoryType();
	PoolType* pool = new PoolType(pFactory, max_idle);
	for (int i = 0; i < 40; ++i)
	{
		// some other code
		{
			boost::shared_ptr<SomeObject> obj = pool->get_object();
			obj->foo();
			// if some error occur
			// obj->set_reusable(false);
		}
	}
	*/
}

void TestObjPool::TestObjPool1()
{
	OPool::ObjectPool<UserRequest> requestPool(1000);
	std::vector<UserRequest*> vecUserReq;
	while (true)
	{
		for (int i = 0; i < 300;i++)
		{
			UserRequest& req = obtainUserRequest(requestPool);
			req.SetID(i);
			vecUserReq.push_back(&req);
		}
		DWORD iLast = timeGetTime();
		int iSize = vecUserReq.size();
		for (int i = 0; i < iSize; i++)
		{
			UserRequest& req = *vecUserReq[i];
			//processUserRequest(requestPool, req);
			printf("sz objid:%d\n", req.GetID());
		}
		printf("sz time used: %d", timeGetTime() - iLast);
		break;
		
	}
	return ;
}