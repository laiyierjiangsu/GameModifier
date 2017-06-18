#pragma  once
class DllInjectDetector
{
public:
	DllInjectDetector();
	~DllInjectDetector();
	void Detect();
private:
	bool Init();
	bool DeInit();
	bool EnumAllModulesOfProcess();
private:
	bool bInitialized;
};

