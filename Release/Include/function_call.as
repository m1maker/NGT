#include "smart_ptr"
class c_type
{
	c_type()
	{}
	scripting::typeid get_type_id() const
	{
		return scripting::typeid ::VOID;
	}

	uint64 get_arg_address()
	{
		return 0;
	}
	uint64 get_address_of_arg()
	{
		return 0;
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
	uint64 get_arg_address()
	{
		return scripting::get_arg_address(val);
	}
	uint64 get_address_of_arg()
	{
		return scripting::get_address_of_arg(val);
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
	uint64 get_arg_address()
	{
		return scripting::get_arg_address(val);
	}
	uint64 get_address_of_arg()
	{
		return scripting::get_address_of_arg(val);
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
	uint64 get_arg_address()
	{
		return scripting::get_arg_address(val);
	}
	uint64 get_address_of_arg()
	{
		return scripting::get_address_of_arg(val);
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
	uint64 get_arg_address()
	{
		return scripting::get_arg_address(val);
	}
	uint64 get_address_of_arg()
	{
		return scripting::get_address_of_arg(val);
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
	uint64 get_arg_address()
	{
		return scripting::get_arg_address(val);
	}
	uint64 get_address_of_arg()
	{
		return scripting::get_address_of_arg(val);
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
	uint64 get_arg_address()
	{
		return scripting::get_arg_address(val);
	}
	uint64 get_address_of_arg()
	{
		return scripting::get_address_of_arg(val);
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
	uint64 get_arg_address()
	{
		return scripting::get_arg_address(val);
	}
	uint64 get_address_of_arg()
	{
		return scripting::get_address_of_arg(val);
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
	uint64 get_arg_address()
	{
		return scripting::get_arg_address(val);
	}
	uint64 get_address_of_arg()
	{
		return scripting::get_address_of_arg(val);
	}

}


class c_caller
{
	protected dictionary functions;
	protected scripting::context call_ctx;
	void reset()
	{
		functions.delete_all();
	}

	uint64 call(uint64 pointer, scripting::typeid return_type_id = scripting::typeid::VOID, array<c_type>@ arguments = null, scripting::callconv call_conv = scripting::CDECL)
	{
		int function_id = -1;
		uint64 return_address = 0;
		if (!functions.get(pointer, function_id))
		{
			function_id = -1;
			string sig;
			sig += get_type_name(return_type_id) + " F" + pointer + "(";
			if(arguments !is null)
			{	
				for (uint i = 0; i < arguments.length(); ++i)
				{
					sig += get_type_name(arguments[i].get_type_id()) + ",";
				}
				if(!arguments.is_empty())
					sig = sig.substr(0, sig.length() - 1); // Remove the last comma
				}
			sig += ")";
			function_id = scripting::register_function(sig, pointer, call_conv); assert(function_id >= 0, "Failed to register" + sig + ": " + scripting::get_messages());
			functions[pointer] = function_id;
		}
		scripting::function function (function_id);
		call_ctx.prepare(function);
		if(arguments !is null)
		{
			for (uint i = 0; i < arguments.length(); ++i)
			{
				call_ctx.set_arg_address(i, arguments[i].get_arg_address());
			}
		}
		int status = call_ctx.execute();
		if (status == scripting::EXECUTION_FINISHED)
		{
			if (return_type_id != scripting::typeid::VOID)
			{
				return_address = call_ctx.get_address_of_return_value();
			}
		}
		else{
			throw("Execution failed! Status: " + status + "Exception: " + call_ctx.get_exception_info() + ", " + scripting::get_messages());
		}
		call_ctx.unprepare();
		return return_address;
	}

	protected string get_type_name(scripting::typeid tid)
	{
		switch (tid)
		{
			case scripting::typeid::INT8:
				return "int8";
			case scripting::typeid::INT16:
				return "int16";
			case scripting::typeid::INT32:
				return "int32";
			case scripting::typeid::INT64:
				return "int64";
			case scripting::typeid::UINT8:
				return "uint8";
			case scripting::typeid::UINT16:
				return "uint16";
			case scripting::typeid::UINT32:
				return "uint32";
			case scripting::typeid::UINT64:
				return "uint64";
			case scripting::typeid::VOID:
				return "void";
			default:
				return "";
		}
	}
};
