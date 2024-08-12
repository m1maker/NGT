#include "angelscript.h"
#include "MemoryStream.h"
#include<assert.h>
MemoryStream::MemoryStream(size_t initialSize)
	: buffer(initialSize), position(0), ref(1) {}
void MemoryStream::AddRef() {
	asAtomicInc(ref);
}
void MemoryStream::Release() {
	if (asAtomicDec(ref) < 1)
		delete this;
}
MemoryStream::~MemoryStream() {
	this->clear();
}
void MemoryStream::write(const char* data, size_t size) {
	ensureCapacity(position + size);

	// Increment position BEFORE copying
	position += size;

	// Copy the data
	std::copy(data, data + size, buffer.begin() + position - size);

	// Add the null terminator (if needed)
	buffer[position] = '\0';
}
void MemoryStream::write(const std::string& str) {
	write(str.data(), str.size());
}
void MemoryStream::read(char* data, size_t size) {
	if (position + size > buffer.size()) {
		return;
	}
	std::copy(buffer.begin() + position, buffer.begin() + position + size, data);
	position += size;
}

void MemoryStream::read(std::string& str, size_t size) {
	str.resize(size);
	read(&str[0], size);
}

void MemoryStream::seek(size_t pos) {
	if (pos > buffer.size()) {
		return;
	}
	position = pos;
}

void MemoryStream::seek(seek_origin origin, int offset) {
	switch (origin) {
	case seek_origin_start:
		if (offset < 0 || static_cast<size_t>(offset) > buffer.size()) {
			return;
		}
		position = static_cast<size_t>(offset);
		break;

	case seek_origin_current:
		if (position + offset > buffer.size() || position + offset < 0) {
			return;
		}
		position += offset;
		break;

	case seek_origin_end:
		if (offset > 0 || static_cast<size_t>(-offset) > buffer.size()) {
			return;
		}
		position = buffer.size() + static_cast<size_t>(offset);
		break;

	default:
		return;
	}
}

size_t MemoryStream::tell() const {
	return position;
}

size_t MemoryStream::size() const {
	return buffer.size();
}

void MemoryStream::clear() {
	buffer.clear();
	position = 0;
}

void MemoryStream::ensureCapacity(size_t requiredSize) {
	if (requiredSize > buffer.size()) {
		buffer.resize(requiredSize); // Double the size for future writes
	}
};
MemoryStream* fmemoryStream(size_t size) { return new MemoryStream(size); }
void RegisterMemstream(asIScriptEngine* engine) {
	engine->RegisterEnum("seek_origin");
	engine->RegisterEnumValue("seek_origin", "SEEK_ORIGIN_START", seek_origin_start);
	engine->RegisterEnumValue("seek_origin", "SEEK_ORIGIN_CURRENT", seek_origin_current);
	engine->RegisterEnumValue("seek_origin", "SEEK_ORIGIN_END", seek_origin_end);
	engine->RegisterObjectType("memory_stream", sizeof(MemoryStream), asOBJ_REF);

	// Register the constructor
	engine->RegisterObjectBehaviour("memory_stream", asBEHAVE_FACTORY, "memory_stream@ mem(size_t=0)", asFUNCTION(fmemoryStream), asCALL_CDECL);
	engine->RegisterObjectBehaviour("memory_stream", asBEHAVE_ADDREF, "void f()", asMETHOD(MemoryStream, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("memory_stream", asBEHAVE_RELEASE, "void f()", asMETHOD(MemoryStream, Release), asCALL_THISCALL);

	engine->RegisterObjectMethod("memory_stream", "void write(const string &in)", asMETHODPR(MemoryStream, write, (const std::string&), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("memory_stream", "void read(string &out, size_t)", asMETHODPR(MemoryStream, read, (std::string&, size_t), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("memory_stream", "void write(uint64, size_t)", asMETHODPR(MemoryStream, write, (const char*, size_t), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("memory_stream", "void read(uint64&out, size_t)", asMETHODPR(MemoryStream, read, (char*, size_t), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("memory_stream", "void seek(int)", asMETHODPR(MemoryStream, seek, (size_t), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("memory_stream", "void seek(seek_origin, int)", asMETHODPR(MemoryStream, seek, (seek_origin, int), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("memory_stream", "size_t tell() const", asMETHOD(MemoryStream, tell), asCALL_THISCALL); assert(r >= 0);
	engine->RegisterObjectMethod("memory_stream", "size_t get_size() const property", asMETHOD(MemoryStream, size), asCALL_THISCALL); assert(r >= 0);
	engine->RegisterObjectMethod("memory_stream", "void clear() const", asMETHOD(MemoryStream, clear), asCALL_THISCALL); assert(r >= 0);
}
