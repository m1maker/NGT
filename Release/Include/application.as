application @application_instance = null;
class application
{
	string output;
	application()
	{
		@application_instance = this;
	}
	void log(const string&in text){
		output += time_stamp_seconds + "\n" + text;
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