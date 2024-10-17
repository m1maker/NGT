application @application_instance = null;
enum applicationtype
{
	WINDOW,
	CONSOLE
};
enum msgtype
{
	MESSAGE,
	WARNING,
	ERROR
};

class application
{
	iostream @output;
	applicationtype system = applicationtype::WINDOW;
	application()
	{
		if (application_instance is null)
		{
			@application_instance = this;
		}
		else
		{
			throw("Too many applications in context");
		}
	}
	void log(const string&in text, const msgtype&in message_type = msgtype::MESSAGE){
		string msg;
		if (message_type == msgtype::WARNING)
			msg += "Warning:\n";
		else if (message_type == msgtype::ERROR)
			msg += "Error:\n";
		msg += text;
		if (this.system == applicationtype::WINDOW)
		{
			screen_reader::speak(msg);
			return;
		}
		else
		{
			if (message_type == msgtype::WARNING || message_type == msgtype::MESSAGE)
				@output = cout;
			else
				@output = cerr;
			output << msg;
			return;
		}
	}
	void initialize(){}
	void uninitialize(){}
	int main(array<string> @args){return 0;}
	int run(){
		int result = this.main(get_char_argv());
		this.uninitialize();
		return result;
	}
};
int main(){
	if (application_instance is null)
	{
		throw("Null application instance");
	}
	try
	{
		application_instance.initialize();
	}
	catch
	{
		application_instance.log(get_exception_info());
		return -1;
	}
	return application_instance.run();
}