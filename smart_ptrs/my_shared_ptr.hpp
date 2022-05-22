typedef unsigned int uint;

template<class T>
class my_shared_ptr {
private:
	T* ptr = nullptr;
	uint* refCount = nullptr;

public:
	my_shared_ptr() : ptr(nullptr), refCount(new uint(0)) { }

	my_shared_ptr(T* ptr) : ptr(ptr), refCount(new uint(1)) { }

	my_shared_ptr(const my_shared_ptr& obj) {
		ptr = obj.ptr;
		refCount = obj.refCount;
		if (obj.ptr != nullptr)
			(*refCount)++;
	}

	my_shared_ptr& operator=(const my_shared_ptr& obj) {
		__cleanup__(); 

		ptr = obj.ptr;
		refCount = obj.refCount;
		if (obj.ptr != nullptr)
			(*refCount)++;
	}

	my_shared_ptr(my_shared_ptr&& dyingObj) {
		ptr = dyingObj.ptr;
		refCount = dyingObj.refCount;

		dyingObj.ptr = dyingObj.refCount = nullptr;
	}

	my_shared_ptr& operator=(my_shared_ptr && dyingObj) {
		__cleanup__();
		
		ptr = dyingObj.ptr;
		refCount = dyingObj.refCount;

		dyingObj.ptr = dyingObj.refCount = nullptr;
	}

	uint get_count() const {
		return *refCount;
	}

	T* get() const {
		return ptr;
	}

	T* operator->() const {
		return ptr;
	}

	T& operator*() const {
		return ptr;
	}

	~my_shared_ptr() {
		__cleanup__();
	}

private:
	void __cleanup__() {
		(*refCount)--;
		if (*refCount == 0) {
			if (nullptr != ptr)
				delete ptr;
			delete refCount;
		}
	}
};