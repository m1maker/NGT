#ifndef AS_CLASS_H
#define AS_CLASS_H
#include "angelscript.h"
#pragma once
class as_class {
private:
	mutable int ref = 0;
public:
	as_class();
	~as_class();
	void add_ref();
	void release();
};
#endif