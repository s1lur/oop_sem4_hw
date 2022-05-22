template <class T>
class my_unique_ptr {
private:
	T* ptr = nullptr;

public:
	my_unique_ptr() : ptr(nullptr) { }

	explicit my_unique_ptr(T* ptr) : ptr(ptr) { }

	my_unique_ptr(const my_unique_ptr& obj) = delete;
	my_unique_ptr& operator=(const my_unique_ptr& obj) = delete;

	my_unique_ptr(my_unique_ptr&& dyingObj) {
		this->ptr = dyingObj.ptr;
		dyingObj.ptr = nullptr; 
	}

	void operator=(my_unique_ptr&& dyingObj) {
		__cleanup__();

		this->ptr = dyingObj.ptr;
		dyingObj.ptr = nullptr;
	}

	T* operator->() {
		return ptr;
	}

	T& operator*() {
		return *ptr;
	}

    explicit operator bool() const {
        return ptr;
    }

	~my_unique_ptr() {
		__cleanup__();
	}

private:
	void __cleanup__() {
		if (ptr != nullptr)
			delete ptr;
	}
};