#include "Pi3CbinStream.h"

Pi3CbinStream::Pi3CbinStream(const char* filepath)
{
	stream = new std::ifstream(filepath, std::istream::in | std::istream::binary);
}


Pi3CbinStream::~Pi3CbinStream()
{
	if (stream) {
		if (stream->is_open()) stream->close();
		delete stream;
	}
}