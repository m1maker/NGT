#ifndef MEMORY_STREAM_H
#define MEMORY_STREAM_H
#pragma once
#include <algorithm>
#include <string>
#include <vector>

enum seek_origin {
	seek_origin_start,
	seek_origin_current,
	seek_origin_end
};

class MemoryStream {
public:
	MemoryStream(size_t initialSize);
	void AddRef();
	void Release();
	~MemoryStream();
	void write(const char* data, size_t size);
	void write(const std::string& str);

	void read(char* data, size_t size);
	void read(std::string& str, size_t size);

	void seek(size_t pos);
	void seek(seek_origin origin, int offset);

	size_t tell() const;
	size_t size() const;
	void clear();

private:
	std::vector<char> buffer; // Memory buffer
	size_t position;          // Current position in the stream
	mutable int ref = 0;
	void ensureCapacity(size_t requiredSize);
};
class asIScriptEngine;
void RegisterMemstream(asIScriptEngine*);
#endif