#pragma  once
class TestLruCache
{
public:
	TestLruCache();
	~TestLruCache();
	void Test();
private:
	void testNoLock();
	void testWithLock();
};