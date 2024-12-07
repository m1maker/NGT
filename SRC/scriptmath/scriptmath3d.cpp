#include "scriptmath3d.h"
#include <assert.h>
#include <math.h>
#include <new> // new()
#include <string.h> // strstr

#ifdef __BORLANDC__
// C++Builder doesn't define a non-standard "sqrtf" function but rather an overload of "sqrt"
// for float arguments.
inline float sqrtf(float x) { return sqrt(x); }
#endif

BEGIN_AS_NAMESPACE

Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(const Vector3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

Vector3::Vector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

bool operator==(const Vector3& a, const Vector3& b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

bool operator!=(const Vector3& a, const Vector3& b)
{
	return !(a == b);
}

Vector3& Vector3::operator=(const Vector3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

Vector3& Vector3::operator+=(const Vector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

float Vector3::length() const
{
	return sqrtf(x * x + y * y + z * z);
}

Vector3 operator+(const Vector3& a, const Vector3& b)
{
	// Return a new object as a script handle
	Vector3 res(a.x + b.x, a.y + b.y, a.z + b.z);
	return res;
}

Vector3 operator-(const Vector3& a, const Vector3& b)
{
	// Return a new object as a script handle
	Vector3 res(a.x - b.x, a.y - b.y, a.z - b.z);
	return res;
}

Vector3 operator*(float s, const Vector3& v)
{
	// Return a new object as a script handle
	Vector3 res(v.x * s, v.y * s, v.z * s);
	return res;
}

Vector3 operator*(const Vector3& v, float s)
{
	// Return a new object as a script handle
	Vector3 res(v.x * s, v.y * s, v.z * s);
	return res;
}

Vector3 operator/(const Vector3& v, float s)
{
	// Return a new object as a script handle
	Vector3 res(v.x / s, v.y / s, v.z / s);
	return res;
}

//-----------------------
// Swizzle operators
//-----------------------

Vector3 Vector3::get_xyz() const
{
	return Vector3(x, y, z);
}
Vector3 Vector3::get_yzx() const
{
	return Vector3(y, z, x);
}
Vector3 Vector3::get_zxy() const
{
	return Vector3(z, x, y);
}
Vector3 Vector3::get_zyx() const
{
	return Vector3(z, y, x);
}
Vector3 Vector3::get_yxz() const
{
	return Vector3(y, x, z);
}
Vector3 Vector3::get_xzy() const
{
	return Vector3(x, z, y);
}
void Vector3::set_xyz(const Vector3& v)
{
	x = v.x; y = v.y; z = v.z;
}
void Vector3::set_yzx(const Vector3& v)
{
	y = v.x; z = v.y; x = v.z;
}
void Vector3::set_zxy(const Vector3& v)
{
	z = v.x; x = v.y; y = v.z;
}
void Vector3::set_zyx(const Vector3& v)
{
	z = v.x; y = v.y; x = v.z;
}
void Vector3::set_yxz(const Vector3& v)
{
	y = v.x; x = v.y; z = v.z;
}
void Vector3::set_xzy(const Vector3& v)
{
	x = v.x; z = v.y; y = v.z;
}

//-----------------------
// AngelScript functions
//-----------------------

static void Vector3DefaultConstructor(Vector3* self)
{
	new(self) Vector3();
}

static void Vector3CopyConstructor(const Vector3& other, Vector3* self)
{
	new(self) Vector3(other);
}

static void Vector3InitConstructor(float x, float y, float z, Vector3* self)
{
	new(self) Vector3(x, y, z);
}

//-----------------------
// Generic calling convention
//-----------------------

static void Vector3DefaultConstructor_Generic(asIScriptGeneric* gen)
{
	Vector3* self = (Vector3*)gen->GetObject();
	new(self) Vector3();
}

static void Vector3CopyConstructor_Generic(asIScriptGeneric* gen)
{
	Vector3* other = (Vector3*)gen->GetArgObject(0);
	Vector3* self = (Vector3*)gen->GetObject();
	Vector3CopyConstructor(*other, self);
}

static void Vector3InitConstructor_Generic(asIScriptGeneric* gen)
{
	float x = gen->GetArgFloat(0);
	float y = gen->GetArgFloat(1);
	float z = gen->GetArgFloat(2);
	Vector3* self = (Vector3*)gen->GetObject();
	Vector3InitConstructor(x, y, z, self);
}

static void Vector3Equal_Generic(asIScriptGeneric* gen)
{
	Vector3* a = (Vector3*)gen->GetObject();
	Vector3* b = (Vector3*)gen->GetArgAddress(0);
	bool r = *a == *b;
	*(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void Vector3Length_Generic(asIScriptGeneric* gen)
{
	Vector3* s = (Vector3*)gen->GetObject();
	gen->SetReturnFloat(s->length());
}

static void Vector3AddAssign_Generic(asIScriptGeneric* gen)
{
	Vector3* a = (Vector3*)gen->GetArgAddress(0);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	*thisPointer += *a;
	gen->SetReturnAddress(thisPointer);
}

static void Vector3SubAssign_Generic(asIScriptGeneric* gen)
{
	Vector3* a = (Vector3*)gen->GetArgAddress(0);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	*thisPointer -= *a;
	gen->SetReturnAddress(thisPointer);
}

static void Vector3MulAssign_Generic(asIScriptGeneric* gen)
{
	float s = gen->GetArgFloat(0);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	*thisPointer *= s;
	gen->SetReturnAddress(thisPointer);
}

static void Vector3DivAssign_Generic(asIScriptGeneric* gen)
{
	float s = gen->GetArgFloat(0);
	Vector3* thisPointer = (Vector3*)gen->GetObject();
	*thisPointer /= s;
	gen->SetReturnAddress(thisPointer);
}

static void Vector3Add_Generic(asIScriptGeneric* gen)
{
	Vector3* a = (Vector3*)gen->GetObject();
	Vector3* b = (Vector3*)gen->GetArgAddress(0);
	Vector3 res = *a + *b;
	gen->SetReturnObject(&res);
}

static void Vector3Sub_Generic(asIScriptGeneric* gen)
{
	Vector3* a = (Vector3*)gen->GetObject();
	Vector3* b = (Vector3*)gen->GetArgAddress(0);
	Vector3 res = *a - *b;
	gen->SetReturnObject(&res);
}

static void Vector3FloatMulVector3_Generic(asIScriptGeneric* gen)
{
	float s = gen->GetArgFloat(0);
	Vector3* v = (Vector3*)gen->GetObject();
	Vector3 res = s * *v;
	gen->SetReturnObject(&res);
}

static void Vector3Vector3MulFloat_Generic(asIScriptGeneric* gen)
{
	Vector3* v = (Vector3*)gen->GetObject();
	float s = gen->GetArgFloat(0);
	Vector3 res = *v * s;
	gen->SetReturnObject(&res);
}

static void Vector3Vector3DivFloat_Generic(asIScriptGeneric* gen)
{
	Vector3* v = (Vector3*)gen->GetObject();
	float s = gen->GetArgFloat(0);
	Vector3 res = *v / s;
	gen->SetReturnObject(&res);
}

//--------------------------------
// Registration
//-------------------------------------

void RegisterScriptMath3D_Native(asIScriptEngine* engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("vector", sizeof(Vector3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK | asOBJ_APP_CLASS_ALLFLOATS); assert(r >= 0);

	// Register the object properties
	r = engine->RegisterObjectProperty("vector", "float x", asOFFSET(Vector3, x)); assert(r >= 0);
	r = engine->RegisterObjectProperty("vector", "float y", asOFFSET(Vector3, y)); assert(r >= 0);
	r = engine->RegisterObjectProperty("vector", "float z", asOFFSET(Vector3, z)); assert(r >= 0);

	// Register the constructors
	r = engine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Vector3DefaultConstructor), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(const vector &in)", asFUNCTION(Vector3CopyConstructor), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(float, float y = 0, float z = 0)", asFUNCTION(Vector3InitConstructor), asCALL_CDECL_OBJLAST); assert(r >= 0);

	// Register the operator overloads
	r = engine->RegisterObjectMethod("vector", "vector &opAddAssign(const vector &in)", asMETHODPR(Vector3, operator+=, (const Vector3&), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector &opSubAssign(const vector &in)", asMETHODPR(Vector3, operator-=, (const Vector3&), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector &opMulAssign(float)", asMETHODPR(Vector3, operator*=, (float), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector &opDivAssign(float)", asMETHODPR(Vector3, operator/=, (float), Vector3&), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "bool opEquals(const vector &in) const", asFUNCTIONPR(operator==, (const Vector3&, const Vector3&), bool), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector opAdd(const vector &in) const", asFUNCTIONPR(operator+, (const Vector3&, const Vector3&), Vector3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector opSub(const vector &in) const", asFUNCTIONPR(operator-, (const Vector3&, const Vector3&), Vector3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector opMul(float) const", asFUNCTIONPR(operator*, (const Vector3&, float), Vector3), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector opMul_r(float) const", asFUNCTIONPR(operator*, (float, const Vector3&), Vector3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector opDiv(float) const", asFUNCTIONPR(operator/, (const Vector3&, float), Vector3), asCALL_CDECL_OBJFIRST); assert(r >= 0);

	// Register the object methods
	r = engine->RegisterObjectMethod("vector", "float length() const", asMETHOD(Vector3, length), asCALL_THISCALL); assert(r >= 0);

	// Register the swizzle operators
	r = engine->RegisterObjectMethod("vector", "vector get_xyz() const", asMETHOD(Vector3, get_xyz), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector get_yzx() const", asMETHOD(Vector3, get_yzx), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector get_zxy() const", asMETHOD(Vector3, get_zxy), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector get_zyx() const", asMETHOD(Vector3, get_zyx), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector get_yxz() const", asMETHOD(Vector3, get_yxz), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "vector get_xzy() const", asMETHOD(Vector3, get_xzy), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "void set_xyz(const vector &in)", asMETHOD(Vector3, set_xyz), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "void set_yzx(const vector &in)", asMETHOD(Vector3, set_yzx), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "void set_zxy(const vector &in)", asMETHOD(Vector3, set_zxy), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "void set_zyx(const vector &in)", asMETHOD(Vector3, set_zyx), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "void set_yxz(const vector &in)", asMETHOD(Vector3, set_yxz), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("vector", "void set_xzy(const vector &in)", asMETHOD(Vector3, set_xzy), asCALL_THISCALL); assert(r >= 0);
}


void RegisterScriptMath3D(asIScriptEngine* engine)
{
	RegisterScriptMath3D_Native(engine);
}

END_AS_NAMESPACE


