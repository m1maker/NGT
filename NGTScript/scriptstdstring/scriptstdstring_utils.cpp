#include <assert.h>
#include "scriptstdstring.h"
#include "../scriptarray/scriptarray.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "../uni_algo.h"

using namespace std;

BEGIN_AS_NAMESPACE

// This function takes an input string and splits it into parts by looking
// for a specified delimiter. Example:
//
// string str = "A|B||D";
// array<string>@ array = str.split("|");
//
// The resulting array has the following elements:
//
// {"A", "B", "", "D"}
//
// AngelScript signature:
// array<string>@ string::split(const string &in delim) const
static CScriptArray *StringSplit(const string &delim, const string &str)
{
	// Obtain a pointer to the engine
	asIScriptContext *ctx = asGetActiveContext();
	asIScriptEngine *engine = ctx->GetEngine();

	// TODO: This should only be done once
	// TODO: This assumes that CScriptArray was already registered
	asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<string>");

	// Create the array object
	CScriptArray *array = CScriptArray::Create(arrayType);

	// Find the existence of the delimiter in the input string
	size_t pos = 0, prev = 0;
	asUINT count = 0;
	while ((pos = str.find(delim, prev)) != string::npos)
	{
		// Add the part to the array
		array->Resize(array->GetSize() + 1);
		((string *)array->At(count))->assign(&str[prev], pos - prev);

		// Find the next part
		count++;
		prev = pos + delim.length();
	}

	// Add the remaining part
	array->Resize(array->GetSize() + 1);
	((string *)array->At(count))->assign(&str[prev]);

	return array;
}

static void StringSplit_Generic(asIScriptGeneric *gen)
{
	// Get the arguments
	string *str = (string *)gen->GetObject();
	string *delim = *(string **)gen->GetAddressOfArg(0);

	// Return the array by handle
	*(CScriptArray **)gen->GetAddressOfReturnLocation() = StringSplit(*delim, *str);
}

// This function takes as input an array of string handles as well as a
// delimiter and concatenates the array elements into one delimited string.
// Example:
//
// array<string> array = {"A", "B", "", "D"};
// string str = join(array, "|");
//
// The resulting string is:
//
// "A|B||D"
//
// AngelScript signature:
// string join(const array<string> &in array, const string &in delim)
static string StringJoin(const CScriptArray &array, const string &delim)
{
	// Create the new string
	string str = "";
	if (array.GetSize())
	{
		int n;
		for (n = 0; n < (int)array.GetSize() - 1; n++)
		{
			str += *(string *)array.At(n);
			str += delim;
		}

		// Add the last part
		str += *(string *)array.At(n);
	}

	return str;
}

static void StringJoin_Generic(asIScriptGeneric *gen)
{
	// Get the arguments
	CScriptArray *array = *(CScriptArray **)gen->GetAddressOfArg(0);
	string *delim = *(string **)gen->GetAddressOfArg(1);

	// Return the string
	new (gen->GetAddressOfReturnLocation()) string(StringJoin(*array, *delim));
}
static string StringReplace(const string &from, const string &to, const string &inp)
{
	// create new string to return, default value with the given string
	string str = inp;
	size_t start_pos = 0;
	while ((start_pos = str.find(from.c_str(), start_pos)) != string::npos)
	{
		str = str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return str;
}

static void StringReplace_Generic(asIScriptGeneric *gen)
{
	// Get the arguments
	string *str = (string *)gen->GetObject();
	string *from = (string *)gen->GetAddressOfArg(0);
	string *to = (string *)gen->GetAddressOfArg(1);

	// Return the replaced string
	new (gen->GetAddressOfReturnLocation()) string(StringReplace(*from, *to, *str));
}
	
static string StringCapitalize(const string &inp)
{
	// check the given string's length
	// create new string to return, default value with the given string
	string str = inp;
std::string temp= una::cases::to_uppercase_utf8(str);
str[0] = temp[0];
return str;
}

static void StringCapitalize_Generic(asIScriptGeneric *gen)
{
	// Get the arguments
	string *str = (string *)gen->GetObject();
	// Return the Capitalized string
	new (gen->GetAddressOfReturnLocation()) string(StringCapitalize(*str));
}

static CScriptArray *StringSplitLines(const string &str)
{
	return StringSplit("\n", StringReplace("\r", "", str.c_str()));
}
static void StringSplitLines_Generic(asIScriptGeneric *gen)
{
	// Get the arguments
	string *str = (string *)gen->GetObject();

	// Return the array by handle
	*(CScriptArray **)gen->GetAddressOfReturnLocation() = StringSplitLines(*str);
}

static bool StringEndsWith(const string& toMatch, const string& inp)
{
	// compare strings length
	una::ranges::utf8_view temp(inp);
	una::ranges::utf8_view temp2(toMatch);
	return std::search(temp.begin(), temp.end(), temp2.begin(), temp2.end()) != temp.end();

}
static void StringEndsWith_Generic(asIScriptGeneric *gen)
{
	// Get the arguments
	string *str = (string *)gen->GetObject();
	string *toMatch = (string *)gen->GetAddressOfArg(0);
	*(bool *)gen->GetAddressOfReturnLocation() = StringEndsWith(*toMatch, *str);
}

static bool StringStartsWith(const string &toMatch, const string &inp)
{
	// compare strings
	return inp.find(toMatch) == 0;
}

static void StringStartsWith_Generic(asIScriptGeneric *gen)
{
	// Get the arguments
	string *str = (string *)gen->GetObject();
	string *toMatch = (string *)gen->GetAddressOfArg(0);
	*(bool *)gen->GetAddressOfReturnLocation() = StringStartsWith(*toMatch, *str);
}

static bool StringContains(const string &toMatch, const string &inp)
{
	// compare strings
	una::ranges::utf8_view temp(inp);
	una::ranges::utf8_view temp2(toMatch);
	return std::search(temp.begin(), temp.end(), temp2.begin(), temp2.end()) != temp.end();
}

static void StringContains_Generic(asIScriptGeneric *gen)
{
	// Get the arguments
	string *str = (string *)gen->GetObject();
	string *toMatch = (string *)gen->GetAddressOfArg(0);
	*(bool *)gen->GetAddressOfReturnLocation() = StringContains(*toMatch, *str);
}

static bool is_upper(const string &s)
{
	una::ranges::utf8_view temp(s);
	return std::all_of(temp.begin(), temp.end(), [](auto c)
					   { return una::codepoint::is_uppercase(c); });
}

static void is_upper_Generic(asIScriptGeneric *gen)
{
	string *str = (string *)gen->GetObject();
	bool result = is_upper(*str);
	*(bool *)gen->GetAddressOfReturnLocation() = result;
}

// This is where the utility functions are registered.
// The string type must have been registered first.
void RegisterStdStringUtils(asIScriptEngine *engine)
{
	int r;

	if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
	{
		r = engine->RegisterObjectMethod("string", "array<string>@ split(const string &in) const", asFUNCTION(StringSplit_Generic), asCALL_GENERIC);
		assert(r >= 0);
		r = engine->RegisterGlobalFunction("string join(const array<string> &in, const string &in)", asFUNCTION(StringJoin_Generic), asCALL_GENERIC);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "string replace(const string &in, const string &in) const", asFUNCTION(StringReplace_Generic), asCALL_GENERIC);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "string capitalize() const", asFUNCTION(StringCapitalize_Generic), asCALL_GENERIC);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "array<string>@ split_lines() const", asFUNCTION(StringSplitLines_Generic), asCALL_GENERIC);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "bool ends_with(const string &in) const", asFUNCTION(StringEndsWith_Generic), asCALL_GENERIC);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "bool starts_with(const string &in) const", asFUNCTION(StringStartsWith_Generic), asCALL_GENERIC);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "bool contains(const string &in) const", asFUNCTION(StringContains_Generic), asCALL_GENERIC);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "bool is_upper() const", asFUNCTION(is_upper_Generic), asCALL_GENERIC);
		assert(r >= 0);
	}
	else
	{
		r = engine->RegisterObjectMethod("string", "array<string>@ split(const string &in) const", asFUNCTION(StringSplit), asCALL_CDECL_OBJLAST);
		assert(r >= 0);
		r = engine->RegisterGlobalFunction("string join(const array<string> &in, const string &in)", asFUNCTION(StringJoin), asCALL_CDECL);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "string replace(const string &in, const string &in) const", asFUNCTION(StringReplace), asCALL_CDECL_OBJLAST);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "string capitalize() const", asFUNCTION(StringCapitalize), asCALL_CDECL_OBJLAST);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "array<string>@ split_lines() const", asFUNCTION(StringSplitLines), asCALL_CDECL_OBJLAST);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "bool ends_with(const string &in) const", asFUNCTION(StringEndsWith), asCALL_CDECL_OBJLAST);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "bool starts_with(const string &in) const", asFUNCTION(StringStartsWith), asCALL_CDECL_OBJLAST);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "bool contains(const string &in) const", asFUNCTION(StringContains), asCALL_CDECL_OBJLAST);
		assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "bool is_upper() const", asFUNCTION(is_upper), asCALL_CDECL_OBJLAST);
		assert(r >= 0);
	}
}

END_AS_NAMESPACE
