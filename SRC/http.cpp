#include "http.h"
#include<string>
using namespace std;
class http {
public:
	string get(string url);
	string get_headers(string url);
	string post(string url, string parameters);
	string request();
	bool get_progress()const;
	string get_return_code()const;
	string get_headers()const;
	void set_user_agent(const string& user_agent);
	string get_user_agent()const;
};
void register_http(asIScriptEngine* engine) {}