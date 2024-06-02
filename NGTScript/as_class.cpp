#include "as_class.h"
as_class::as_class() {
	ref = 1;
}
as_class::~as_class() {
}
void as_class::add_ref() {
	asAtomicInc(ref);
}
void as_class::release() {
	if (asAtomicDec(ref) < 1)
		delete this;
}