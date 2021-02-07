#pragma once
#include <iostream>
#include <fstream>
#include <vector>

class Pi3CbinStream {
public:

	Pi3CbinStream() {}
	~Pi3CbinStream();
	Pi3CbinStream(const char* filepath);

	std::ifstream * stream = nullptr;

	bool is_open() { return (stream && stream->is_open()); }
	
	size_t getSize();

	template <typename T>
	void read(T& value) {
		stream->read(reinterpret_cast<char*>(&value), sizeof(T));
	}

	template<class ContainerClass>
	void readContainer(ContainerClass& container) {
		uint32_t size = 0; // container.size();
		read(size);
		//container.clear();
		for (size_t i = 0; i < size; ++i) {
			if (stream->fail()) 
				break;
			typename ContainerClass::value_type value;
			read(value);
			container.push_back(value);
		}
	}

	template <typename T>
	size_t readAll(std::vector<T>& v) {
		size_t size = getSize();
		v.resize(size / sizeof(T));
		stream->read((char*)&v[0], size);
		stream->close();
		return size;
	}
};
