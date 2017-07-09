#include "TestLruCache.h"
#include "ThirdParty/lrucache/LRUCache11.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <windows.h>

using namespace lru11;
typedef LruCachePool<std::string, int32_t> KVCache;
typedef KVCache::node_type KVNode;
typedef KVCache::list_type KVList;

typedef LruCachePool<std::string, Item> ItemCache;
typedef ItemCache::node_type ItemNode;
typedef ItemCache::list_type ItemList;

void TestLruCache::Test()
{
	//testNoLock();
	//testWithLock();
	//TestLruItemList();
	TestUsingObjpoolList();
	TestUsingNewList();
}

TestLruCache::TestLruCache()
{

}

TestLruCache::~TestLruCache()
{

}



// test the vanilla version of the cache
void TestLruCache::testNoLock() 
{

	// with no lock
	auto cachePrint =
		[&](const KVCache& c) {
		std::cout << "Cache (size: " << c.size() << ") (max=" << c.getMaxSize() << ") (e=" 
			<< c.getElasticity() << ") (allowed:" << c.getMaxAllowedSize() << ")" << std::endl;
		size_t index = 0;
		auto nodePrint = [&](const KVNode& n) {
			std::cout << " ... [" << ++index << "] " << n.key << " => " << n.value << std::endl;
		};
		c.cwalk(nodePrint);
	};
	/*KVCache c(5, 2);
	c.insert("hello", 1);
	c.insert("world", 2);
	c.insert("foo", 3);
	c.insert("bar", 4);
	c.insert("blanga", 5);
	cachePrint(c);
	c.insert("toodloo", 6);
	cachePrint(c);
	c.insert("wagamama", 7);
	cachePrint(c);
	c.get("blanga");
	std::cout << "... blanga should move to the top..." << std::endl;
	cachePrint(c);
	c.get("foo");
	std::cout << "... foo should move to the top..." << std::endl;
	cachePrint(c);*/
}

// Test a thread-safe version of the cache with a std::mutex
void TestLruCache::testWithLock() {

	using LCache = LruCachePool<std::string, std::string, std::mutex>;
	auto cachePrint2 =
		[&](const LCache& c) {
		std::cout << "Cache (size: " << c.size() << ") (max=" << c.getMaxSize() << ") (e=" << c.getElasticity() << ") (allowed:" << c.getMaxAllowedSize() << ")" << std::endl;
		size_t index = 0;
		auto nodePrint = [&](const LCache::node_type& n) {
			std::cout << " ... [" << ++index << "] " << n.key << " => " << n.value << std::endl;
		};
		c.cwalk(nodePrint);
	};
	// with a lock
	/*LCache lc(25, 2);
	auto worker = [&]() {
		std::ostringstream os;
		os << std::this_thread::get_id();
		std::string id = os.str();

		for (int i = 0; i < 10; i++) {
			std::ostringstream os2;
			os2 << "id:" << id << ":" << i;
			lc.insert(os2.str(), id);

		}
	};
	std::vector<std::unique_ptr<std::thread>> workers;
	workers.reserve(100);
	for (int i = 0; i < 100; i++) {
		workers.push_back(std::unique_ptr<std::thread>(
			new std::thread(worker)));
	}

	for (const auto& w : workers) {
		w->join();
	}
	std::cout << "... workers finished!" << std::endl;
	cachePrint2(lc);*/
}
#define MAX_ITEM_SIZE 100
#define MAX_ELISTIC_SIZE 10
void TestLruCache::TestLruItemList()
{
	// with no lock
	auto cachePrint =
		[&](const ItemCache& c) {
		std::cout << "Cache (size: " << c.size() << ") (max=" << c.getMaxSize() << ") (e="
			<< c.getElasticity() << ") (allowed:" << c.getMaxAllowedSize() << ")" << std::endl;
		size_t index = 0;
		auto nodePrint = [&](const ItemNode& n) {
			std::cout << " ... [" << ++index << "] " << n.key << " => " << n.value->Key() << std::endl;
		};
		c.cwalk(nodePrint);
	};

	ItemCache itemcache(MAX_ITEM_SIZE, MAX_ELISTIC_SIZE);
	for (int i = 0; i < MAX_ITEM_SIZE; i++)
	{
		Item* a = itemcache.acquireObject();
		a->Init(i, i);
		itemcache.insert(a->Key(), a);
		cachePrint(itemcache);
	}
	auto key = Item::GetKey(1, 1);
	auto item = itemcache.get(key);
	printf("original con :%s\n", item->Content().c_str());
	item->TestChange("I am changed");
	printf("changed con :%s\n", item->Content().c_str());
	auto itemGet = itemcache.get(key);
	printf("get con againe :%s\n", itemGet->Content().c_str());
	itemcache.remove(key);

	//statr using elistic size
	for (int i = MAX_ITEM_SIZE -1; i < MAX_ITEM_SIZE+ MAX_ELISTIC_SIZE + 10; i++)
	{
		Item* a = itemcache.acquireObject();
		a->Init(i, i);
		itemcache.insert(a->Key(), a);
		cachePrint(itemcache);
	}

}

void TestLruCache::TestUsingObjpoolList()
{
	DWORD iLast = timeGetTime();
	OPool::ObjectPool<Item> requestPool(1000);
	std::vector<Item*> vecUserReq;
	int iCount = 0;
	int iTotal = 1;
	while (iCount < iTotal)
	{
		for (int i = 0; i < 300; i++)
		{
			Item& item = requestPool.acquireObject();
			item.Init(i, i);
			vecUserReq.push_back(&item);
		}
		int iSize = vecUserReq.size();
		for (int i = 0; i < iSize; i++)
		{
			Item *p = vecUserReq[i];
			requestPool.releaseObject(*p);
		}
		vecUserReq.clear();
		iCount++;
	}
	printf("Using obj pool: %d\n", timeGetTime() - iLast);
}

void TestLruCache::TestUsingNewList()
{
	DWORD iLast = timeGetTime();
	std::vector<Item*> vecUserReq;
	int iCount = 0;
	int iTotal = 1;
	while (iCount < iTotal)
	{
		for (int i = 0; i < 300; i++)
		{
			Item* item = new Item;
			item->Init(i, i);
			vecUserReq.push_back(item);
		}
		int iSize = vecUserReq.size();
		for (int i = 0; i < iSize; i++)
		{
			Item *p = vecUserReq[i];
			delete p;
		}
		vecUserReq.clear();
		iCount++;
	}
	printf("Using new method: %d\n", timeGetTime() - iLast);
}

void Item::Init(int iObjID, int iTempleID)
{
	_iTempleteID = iTempleID;
	_iObjID = iObjID;
	_key = GetKey(_iObjID, _iTempleteID);
}

std::string& Item::Key()
{
	return _key;
}

std::string Item::GetKey(int iObj, int iT)
{
	char sz[64] = { 0 };
	sprintf_s(sz, "%d_%d", iObj, iT);
	std::string key(sz);
	return std::move(key);
}

void Item::TestChange(std::string content)
{
	_content = content;
}

Item::~Item()
{
	//printf("~ item deconstruct %s\n", _key.c_str());
}

Item::Item()
{
	_key = "default";
}
