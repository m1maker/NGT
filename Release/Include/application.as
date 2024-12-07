bool running = false; // Application run state. Consider using this in the any application loops with !quit_requested, if you wish to exit by closing the window. It is assumed that if this variable is false, any loop will terminate.

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
	bool is_initialized = false;

	protected application() /// Do not override this constructor
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

	protected ~application() // Do not override this destructor
	{
		if (this is application_instance)
			@application_instance = null; // Release the ref
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
			@output = (message_type == msgtype::WARNING || message_type == msgtype::MESSAGE) ? cout : cerr;
			output << msg;
			return;
		}
	}
	void initialize(){}
	void uninitialize(){}

	void restart()
	{
		running = false;
		if (is_initialized)uninitialize(); // If you have instance class, please release it's handle to avoid restart errors, such as process is already running.
		is_initialized = false;
		string cmd = SCRIPT_EXECUTABLE;
		array<string> args = get_char_argv();
		if (args.length() > 0)
		{
			for (uint i = 0; i < args.length(); ++i)
			{
				cmd += " " + args[i];
			}
		}
		dictionary d;
		d[0] = cmd;
		// We are creating a thread that invokes the process, because the existing instance will wait when the second process will exit. But we want to destroy this instance and run a new one
		if (os::platform == "Windows") // So we should use overloaded function that takes an output string on Windows. We don't need to stdout/stderr, then make this out void. I don't know why first overloaded function that uses system not works
		{
			thread t(@function(dictionary@ args) { exec(string(args[0]), void); }, d);		// Unfortunately, on Windows we were not able to call all CloseHandle's correctly, since we need to wait for the command to complete execution. This leads to memory leaks, however, once the process terminates, Windows will try to close handles and free the memory. However, I still need to think this through.
			t.detach();
		}
		else // But try make it usable on other platforms
		{
			thread t(@function(dictionary@ args) { exec(string(args[0])); }, d);
			t.detach();
		}

		wait(500); // We can only hope that the application has started. There is no way to determine process runs yet.
	}

	int main(array<string> @args){return 0;}
	int run(){
		running = true;
		int result = this.main(get_char_argv());
		if (is_initialized)this.uninitialize();
		running = false;
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
		@application_instance = null;
		return -1;
	}
	application_instance.is_initialized = true;
	return application_instance.run();
}

