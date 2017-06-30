#pragma once
class FileIntegrityDetector
{
public:
	FileIntegrityDetector();
	~FileIntegrityDetector();
	void Detect();
private:
	bool CheckFileSum();
	void CheckFileModified();
	bool AddCrc322File();
	bool IsFileModified();
private:

};

