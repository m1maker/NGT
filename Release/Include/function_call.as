#include "smart_ptr"
class c_type
{
	c_type()
	{}
	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::VOID;
	}
};

class c_void_p : c_type
{
	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::UINT64;
	}
};

class c_short : c_type
{
	int16 val;
	c_short(int16 value)
	{
		val = value;
	}
	c_short& opAssign(int16& in value)
	{
		this.val = value;
		return this;
	}
	c_short(const c_short& in other)
	{
		this.val = other.val;
	}

	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::INT16;
	}
}

class c_int : c_type
{
	int32 val;
	c_int(int32 value)
	{
		val = value;
	}
	c_int& opAssign(int32& in value)
	{
		this.val = value;
		return this;
	}
	c_int(const c_int& in other)
	{
		this.val = other.val;
	}

	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::INT32;
	}
}

class c_int64 : c_type
{
	int64 val;
	c_int64(int64 value)
	{
		val = value;
	}
	c_int64& opAssign(int64& in value)
	{
		this.val = value;
		return this;
	}
	c_int64(const c_int64& in other)
	{
		this.val = other.val;
	}

	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::INT64;
	}
}

class c_char : c_type
{
	int8 val;
	c_char(int8 value)
	{
		val = value;
	}
	c_char& opAssign(int8& in value)
	{
		this.val = value;
		return this;
	}
	c_char(const c_char& in other)
	{
		this.val = other.val;
	}

	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::INT8;
	}
}

class c_ushort : c_type
{
	uint16 val;
	c_ushort(uint16 value)
	{
		val = value;
	}
	c_ushort& opAssign(uint16& in value)
	{
		this.val = value;
		return this;
	}
	c_ushort(const c_ushort& in other)
	{
		this.val = other.val;
	}

	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::UINT16;
	}
}

class c_uint : c_type
{
	uint32 val;
	c_uint(uint32 value)
	{
		val = value;
	}
	c_uint& opAssign(uint32& in value)
	{
		this.val = value;
		return this;
	}
	c_uint(const c_uint& in other)
	{
		this.val = other.val;
	}

	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::UINT32;
	}
}

class c_uint64 : c_type
{
	uint64 val;
	c_uint64(uint64 value)
	{
		val = value;
	}
	c_uint64& opAssign(uint64& in value)
	{
		this.val = value;
		return this;
	}
	c_uint64(const c_uint64& in other)
	{
		this.val = other.val;
	}

	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::UINT64;
	}
}

class c_uchar : c_type
{
	uint8 val;
	c_uchar(uint8 value)
	{
		val = value;
	}
	c_uchar& opAssign(uint8& in value)
	{
		this.val = value;
		return this;
	}
	c_uchar(const c_uchar& in other)
	{
		this.val = other.val;
	}

	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::UINT8;
	}
}
