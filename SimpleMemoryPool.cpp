/*
 *@Author		: JungDo Yun
 *@Created		: 2020-08-17 오후 10:20:55
 *@Description	: 1시간만에 대충만들어본 메모리풀 / 예외처리 제대로안함
 */

#include <list>
#include <map>
#include <iostream>
#include <algorithm>

using namespace std;

struct IMemoryPool
{
	virtual ~IMemoryPool() {};
	virtual bool Init() = 0;
	virtual void* Alloc(size_t size) = 0;
	virtual bool Free(void* ptr) = 0;
	virtual const int GetMemoryBlockSize() const = 0;
	virtual const int GetMemoryBlockCount() const = 0;
};

template<int MemoryBlockSize, int MemoryBlockCount>
class MemoryPool : public IMemoryPool
{
private:
	list<char*> MemoryList;
	char* MemoryChunk;
protected:
	virtual bool Init()
	{
		MemoryChunk = new char[MemoryBlockSize * MemoryBlockCount];
		if (MemoryChunk == nullptr)
			return false;

		for (int i = 0; i < MemoryBlockSize * MemoryBlockCount; i += MemoryBlockSize)
			MemoryList.push_back(MemoryChunk + MemoryBlockSize);

		cout << MemoryBlockSize << " 메모리 블록 " << MemoryBlockCount << "개를 동적할당하였습니다." << endl;

		return true;
	}

	MemoryPool()
	{
		Init();
	}

	virtual ~MemoryPool()
	{
		if (MemoryChunk != nullptr)
			delete MemoryChunk;

		cout << MemoryBlockSize << " 메모리 블록 " << MemoryBlockCount << "개의 동적할당을 해제하였습니다" << endl;
	}


	virtual void* Alloc(size_t size)
	{
		if (size > MemoryBlockSize)
			return nullptr;

		void* ret = (void*)MemoryList.front();
		MemoryList.pop_front();
		return ret;
	}

	virtual bool Free(void* ptr)
	{
		MemoryList.push_back(static_cast<char*>(ptr));
		return true;
	}

	virtual const int GetMemoryBlockSize() const { return MemoryBlockSize; }
	virtual const int GetMemoryBlockCount() const { return MemoryBlockCount; }

	friend class MemoryPoolManager;
};

class MemoryPoolManager
{
	map<int, IMemoryPool*> MemoryPoolMap;

	MemoryPoolManager()
	{
		MemoryPoolMap.insert(std::make_pair(16, static_cast<IMemoryPool*>(new MemoryPool<16, 200>())));
		MemoryPoolMap.insert(std::make_pair(32, static_cast<IMemoryPool*>(new MemoryPool<32, 200>())));
		MemoryPoolMap.insert(std::make_pair(64, static_cast<IMemoryPool*>(new MemoryPool<64, 200>())));
		MemoryPoolMap.insert(std::make_pair(128, static_cast<IMemoryPool*>(new MemoryPool<128, 200>())));
		MemoryPoolMap.insert(std::make_pair(256, static_cast<IMemoryPool*>(new MemoryPool<256, 200>())));
		MemoryPoolMap.insert(std::make_pair(512, static_cast<IMemoryPool*>(new MemoryPool<512, 200>())));
		MemoryPoolMap.insert(std::make_pair(1024, static_cast<IMemoryPool*>(new MemoryPool<1024, 200>())));
		MemoryPoolMap.insert(std::make_pair(2048, static_cast<IMemoryPool*>(new MemoryPool<2048, 200>())));
		MemoryPoolMap.insert(std::make_pair(4096, static_cast<IMemoryPool*>(new MemoryPool<4096, 50>())));
	}
public:
	~MemoryPoolManager()
	{
		std::for_each(MemoryPoolMap.begin(), MemoryPoolMap.end(), [](const std::pair<int, IMemoryPool*>& pair) { delete pair.second; });
	}

	static MemoryPoolManager& GetInstance()
	{
		static MemoryPoolManager memoryPoolManager;
		return memoryPoolManager;
	}

	void* Alloc(size_t& size)
	{
		map<int, IMemoryPool*>::iterator it = MemoryPoolMap.lower_bound(size);
		if (it == MemoryPoolMap.end())
			return nullptr;
		size = it->second->GetMemoryBlockSize();
		cout << it->second->GetMemoryBlockSize() << "바이트만 담긴 메모리풀에서 메모리 블록을 가져왔습니다." << endl;
		return it->second->Alloc(size);
	}

	bool Free(void* ptr, size_t ptrSize)
	{
		if (MemoryPoolMap.find(ptrSize) == MemoryPoolMap.end())
			return false;

		cout << MemoryPoolMap[ptrSize]->GetMemoryBlockSize() << "바이트만 담긴 메모리풀에 다시 메모리 블록을 되돌려 놓았습니다." << endl;
		return MemoryPoolMap[ptrSize]->Free(ptr);
	}
};



int main(int argc, wchar_t* argv[]) {
	MemoryPoolManager::GetInstance(); //메모리풀 초기화

	cout << "================================" << endl;
	cout << "50바이트 요청시" << endl;
	{
		size_t blockSize = 50;
		void* ptr = MemoryPoolManager::GetInstance().Alloc(blockSize);
		MemoryPoolManager::GetInstance().Free(ptr, blockSize);
	}
	cout << "100바이트 요청" << endl;
	{
		size_t blockSize = 100;
		void* ptr = MemoryPoolManager::GetInstance().Alloc(blockSize);
		MemoryPoolManager::GetInstance().Free(ptr, blockSize);
	}
	cout << "200바이트 요청" << endl;
	{
		size_t blockSize = 200;
		void* ptr = MemoryPoolManager::GetInstance().Alloc(blockSize);
		MemoryPoolManager::GetInstance().Free(ptr, blockSize);
	}
	cout << "500바이트 요청" << endl;
	{
		size_t blockSize = 300;
		void* ptr = MemoryPoolManager::GetInstance().Alloc(blockSize);
		MemoryPoolManager::GetInstance().Free(ptr, blockSize);
	}

	cout << "1000바이트 요청" << endl;
	{
		size_t blockSize = 1000;
		void* ptr = MemoryPoolManager::GetInstance().Alloc(blockSize);
		MemoryPoolManager::GetInstance().Free(ptr, blockSize);
	}

	cout << "4000바이트 요청" << endl;
	{
		size_t blockSize = 4000;
		void* ptr = MemoryPoolManager::GetInstance().Alloc(blockSize);
		MemoryPoolManager::GetInstance().Free(ptr, blockSize);
	}
	cout << "================================" << endl;

	return 0;
}