#pragma  once
class MemProtect
{
public:
	MemProtect();
	~MemProtect();
	void Test();
private:
	int  GetValue();
	void SetValue(int a);
private:
	unsigned int m_iEncodeValue;
};

