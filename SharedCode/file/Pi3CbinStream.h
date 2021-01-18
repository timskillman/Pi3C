#pragma once
#include <iostream>
#include <fstream>

class Pi3CbinStream {
public:

	Pi3CbinStream() {}
	~Pi3CbinStream();
	Pi3CbinStream(const char* filepath);

	std::ifstream * stream = nullptr;

	bool is_open() { return (stream && stream->is_open()); }

	template <typename T>
	void read(T& item) {
		stream->read(reinterpret_cast<char*>(&item), sizeof(T));
	}

	template<class ContainerClass>
	void readContainer(ContainerClass& container) {
		uint32_t size = 0; // container.size();
		read(size);
		//container.clear();
		for (size_t i = 0; i < size; ++i) {
			if (stream->fail()) break;
			typename ContainerClass::value_type value;
			read(value);
			container.push_back(value);
		}
	}
};
