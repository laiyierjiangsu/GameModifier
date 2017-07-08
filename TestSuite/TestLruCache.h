#pragma  once
#include <string>
class Item
{
public:
	void Init(int iObjID, int iTempleID);
	std::string &Key();
	static std::string GetKey(int iObj, int iT);
	void TestChange(std::string content);
	std::string& Content(){ return _content; }
private:
	int _iTempleteID;
	int _iObjID;
	std::string _key;

	std::string _content;
};
class TestLruCache
{
public:
	TestLruCache();
	~TestLruCache();
	void Test();
private:
	void testNoLock();
	void testWithLock();
	void TestItem();
};