#include "TestLruCache.h"
#include "ThirdParty/lrucache/LRUCache11.hpp"
#include "ThirdParty/ObjectPool/ObjectPool.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>


using namespace lru11;
typedef Cache<std::string, int32_t> KVCache;
typedef KVCache::node_type KVNode;
typedef KVCache::list_type KVList;

typedef Cache<std::string, Item> ItemCache;
typedef ItemCache::node_type ItemNode;
typedef ItemCache::list_type ItemList;

void TestLruCache::Test()
{
	//testNoLock();
	//testWithLock();
	TestItem();
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
	KVCache c(5, 2);
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
	cachePrint(c);
}

// Test a thread-safe version of the cache with a std::mutex
void TestLruCache::testWithLock() {

	using LCache = Cache<std::string, std::string, std::mutex>;
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
	LCache lc(25, 2);
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
	cachePrint2(lc);
}
#define MAX_ITEM_SIZE 100
#define MAX_ELISTIC_SIZE 10
void TestLruCache::TestItem()
{
	OPool::ObjectPool<Item> requestPool(MAX_ITEM_SIZE + MAX_ELISTIC_SIZE);
	ItemCache itemcache(MAX_ITEM_SIZE, MAX_ELISTIC_SIZE);
	for (int i = 0; i < 100;i++)
	{
		Item a = requestPool.acquireObject();
		a.Init(i, i);
		itemcache.insert(a.Key(), a);
	}

	auto& item = itemcache.get(Item::GetKey(1, 1));
	printf("original con :%s\n", item.Content().c_str());
	item.TestChange("I am changed");
	printf("changed con :%s\n", item.Content().c_str());
	auto& itemGet = itemcache.get(Item::GetKey(1, 1));
	printf("get con againe :%s\n", itemGet.Content().c_str());
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
