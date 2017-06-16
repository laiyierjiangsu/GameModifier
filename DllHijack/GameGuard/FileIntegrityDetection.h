#pragma once
class FileIntegrityDetector
{
public:
	FileIntegrityDetector();
	~FileIntegrityDetector();
	void Detect();
private:
	bool CheckFileSum();

private:

};

