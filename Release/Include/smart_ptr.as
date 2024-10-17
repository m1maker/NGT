class smart_ptr
{
	protected uint64 handle;
	protected size_t allocated_size;
	smart_ptr(size_t _size)
	{
		handle = malloc(_size);
		if (handle == 0)
		{
			this.allocated_size = 0;
			return;
		}
		this.allocated_size = _size;
	}
	smart_ptr()
	{}
	~smart_ptr()
	{
		if (handle != 0)
		{
			free(handle);
			allocated_size = 0;
		}
	}
	uint64 opImplConv()
	{
		return handle;
	}
	string opCast() {
		return c_str_to_string(handle);
	}
	uint64 get_size() const property
	{
		return allocated_size;
	}
	void set_size(size_t new_size) property {
		handle = realloc(handle, new_size);
		if (handle == 0)
		{
			this.allocated_size = 0;
			return;
		}
		this.allocated_size = new_size;
	}
};
