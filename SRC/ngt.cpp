#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
#include "cmp.h"

#include "ngtreg.h"
#include "sound.h"
#include <Poco/BinaryReader.h>
#include <Poco/Event.h>
#include <Poco/Mutex.h>
#include <Poco/Runnable.h>
#include <Poco/SHA2Engine.h>
#include <Poco/TextConverter.h>
#include <Poco/Thread.h>
#include <Poco/Unicode.h>
#include <Poco/UnicodeConverter.h>
#include <Poco/Util/Application.h>
#include<sdl3/SDL.h>
extern "C" {
#define SRAL_STATIC
#include <SRAL.h>
}
#include <chrono>
#include<condition_variable>
#include <filesystem>
#include<mutex>
#include <random>
#include <string>
#include <thread>
#include<vector>
#define BL_NUMWORDS_IMPLEMENTATION
extern "C"
{
#include"bl_number_to_words.h"
}
#include<map>
#include "uni_algo.h"
#include<algorithm>
#include <stdio.h>
#include "NGT.H"
#include "scriptfile/scriptfilesystem.h"
#include <locale>
#include <codecvt>
#include<fstream>
#include <cstdlib>
#include<algorithm>
#include <cstring>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "AES/aes.hpp"
asIScriptFunction* exit_callback = nullptr;
using namespace string_literals;
using namespace std;
int mouse_x = 0, mouse_y = 0, mouse_z = 0;
SDL_Event e;
Poco::Event g_windowEvent;
bool window_is_focused = false;
bool window_event_show = false;
bool window_event_hide = false;
bool window_event_set_title = false;
bool window_event_push = false;
bool window_event_keyboard_reset = false;
bool window_event_fullscreen = false;
bool window_fullscreen = false;
long update_window_freq = 5;
const char* window_title = nullptr;
int window_w, window_h;
bool window_thread_event_shutdown = false;
bool window_closable = true;
bool default_screen_reader_interrupt = false;
bool window_event_present = false;
bool window_has_renderer = false;// We can't create window in other thread, if we need graphics
#ifdef _WIN32
wstring wstr(const string& utf8String)
{
	wstring str;
	Poco::UnicodeConverter::convert(utf8String, str);
	return str;
}
#endif
std::array<DeviceButton, 512> keys;
std::array<DeviceButton, 8>buttons;
bool keyhook = false;
string inputtext;
int get_cpu_count() {
	return SDL_GetNumLogicalCPUCores();
}
int get_system_ram() {
	return SDL_GetSystemRAM();
}
string get_platform() {
	return string(SDL_GetPlatform());
}
void set_exit_callback(asIScriptFunction* callback) {
	exit_callback = callback;
}
typedef struct {
	std::string* data;
	size_t read_cursor;
} cmp_buffer;
static bool cmp_read_bytes(cmp_ctx_t* ctx, void* output, size_t len) {
	cmp_buffer* buf = (cmp_buffer*)ctx->buf;
	if (!buf || !buf->data) return false;
	if (buf->read_cursor + len > buf->data->size()) len = buf->data->size() - buf->read_cursor;
	if (len == 0) return false;
	memcpy(output, buf->data->data() + buf->read_cursor, len);
	buf->read_cursor += len;
	return true;
}
static bool cmp_skip_bytes(cmp_ctx_t* ctx, size_t len) {
	cmp_buffer* buf = (cmp_buffer*)ctx->buf;
	if (!buf || !buf->data) return false;
	if (buf->read_cursor + len > buf->data->size()) len = buf->data->size() - buf->read_cursor;
	if (len == 0) return false;
	buf->read_cursor += len;
	return true;
}
static size_t cmp_write_bytes(cmp_ctx_t* ctx, const void* input, size_t len) {
	cmp_buffer* buf = (cmp_buffer*)ctx->buf;
	if (!buf || !buf->data) return 0;
	buf->data->append((const char*)input, len);
	return len;
}
bool wait_event_requested = false;
class WindowThread : public Poco::Runnable {
private:
	Poco::Thread thread;
public:
	SDL_Window* win = nullptr;
	SDL_Renderer* renderer = nullptr;
	void start() {
		thread.start(*this);
	}
	bool IsRunning() { return thread.isRunning(); }
	void destroy() {
		if (win != nullptr) {
			if (SDL_TextInputActive(win))
				SDL_StopTextInput(win);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(win);
			win = nullptr;
			window_is_focused = false;
		}
	}
	void monitor() {
		if (window_event_show) {
			window_event_show = false;
			SDL_WindowFlags flags = 0;
			if (SRAL_GetCurrentEngine() == ENGINE_JAWS) {
				SDL_SetHint(SDL_HINT_ALLOW_ALT_TAB_WHILE_GRABBED, "1");
				flags |= SDL_WINDOW_KEYBOARD_GRABBED;
			}
			win = SDL_CreateWindow(window_title, window_w, window_h, flags);

			if (win != nullptr)
			{
				SDL_RaiseWindow(win);
				window_is_focused = true;
				SDL_StartTextInput(win);
				renderer = SDL_CreateRenderer(win, nullptr);
			}
		}
		if (window_event_hide) {
			window_event_hide = false;
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(win);
			win = nullptr;
		}
		if (window_event_set_title) {
			window_event_set_title = false;
			if (win == nullptr)return;
			SDL_SetWindowTitle(win, window_title);
		}
		if (window_event_fullscreen) {
			window_event_fullscreen = false;
			if (win == nullptr)return;
			SDL_SetWindowFullscreen(win, window_fullscreen);
		}
		if (win != nullptr) {
			SDL_PumpEvents();
			bool result;
			if (window_has_renderer && wait_event_requested) {
				wait_event_requested = false;
				result = SDL_WaitEvent(&e);
			}
			else
				result = SDL_PollEvent(&e);
			if (result == true) {
				if (!window_has_renderer) {
					g_windowEvent.set();
				}
			}
			if (window_event_push) {
				window_event_push = false;
				SDL_PushEvent(&e);
			}
			if (window_event_present) {
				window_event_present = false;
				SDL_RenderPresent(renderer);
			}
			if (window_event_keyboard_reset) {
				window_event_keyboard_reset = false;
				SDL_ResetKeyboard();
			}
			if (e.type == SDL_EVENT_QUIT and window_closable == true) {
				exit_engine();
			}
			if (e.type == SDL_EVENT_TEXT_INPUT)
			{
				inputtext += e.text.text;
			}

			if (e.type == SDL_EVENT_KEY_DOWN)
			{
				keys[e.key.scancode].isDown = true;
				keys[e.key.scancode].isReleased = false;
			}
			if (e.type == SDL_EVENT_KEY_UP)
			{
				keys[e.key.scancode].isDown = false;
				keys[e.key.scancode].isPressed = false;
			}
			if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				buttons[e.button.button].isDown = true;
				buttons[e.button.button].isReleased = false;
			}
			if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
				buttons[e.button.button].isDown = false;
				buttons[e.button.button].isPressed = false;
			}


			if (e.type == SDL_EVENT_MOUSE_MOTION) {
				mouse_x = e.motion.x;
				mouse_y = e.motion.y;
			}
			if (e.type == SDL_EVENT_MOUSE_WHEEL) {
				mouse_z += e.wheel.y;
			}
			if (e.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
				window_is_focused = true;
			if (e.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
				window_event_keyboard_reset = true;
				window_is_focused = false;
			}
			SDL_UpdateWindowSurface(win);

		}
	}


	void run() {
		while (!window_thread_event_shutdown) {
			thread.sleep(update_window_freq + 1);
			this->monitor();
		}
		this->destroy();
	}
	void stop() {
		window_thread_event_shutdown = true;
	}
};
WindowThread* windowRunnable;
void set_update_window_freq(long freq) {
	update_window_freq = freq;
}
long get_update_window_freq() {
	return update_window_freq;
}
void init_engine() {
	SDL_Init(SDL_INIT_VIDEO);
	SRAL_Initialize(0);
#ifdef _WIN32
	// SRAL keyboard hooks is stopping all the events on Linux
	SRAL_RegisterKeyboardHooks();
#endif
	enet_initialize();
}
void set_library_path(const string& path) {
	SRAL_Uninitialize();
	filesystem::path current_dir = filesystem::current_path();
	filesystem::path new_dir = filesystem::current_path() / path;

	filesystem::current_path(new_dir);
	SRAL_Initialize(0);
	soundsystem_free();
	soundsystem_init();
	filesystem::current_path(current_dir);
}

random_device rd;
mt19937 gen(rd());
int64_t random(int64_t min, int64_t max) {
	uniform_int_distribution<int64_t> dis(min, max); // Use int64_t 
	return dis(gen);
}

double randomDouble(double min, double max) {
	static_assert(is_floating_point<double>::value, "Type must be floating point");

	uniform_real_distribution<double> dis(min, max);

	return dis(gen); // Ensure 'gen' is a valid random number generator
}

bool random_bool() {
	return random(0, 1) == random(0, 1);
}
int get_last_error() {
	return 0;
}
void set_screen_reader_interrupt(bool interrupt) {
	default_screen_reader_interrupt = interrupt;
}
bool get_screen_reader_interrupt() {
	return default_screen_reader_interrupt;
}
bool speak(const string& text, bool stop) {
	return SRAL_Speak(text.c_str(), stop);
}
bool braille(const string& text) {
	return SRAL_Braille(text.c_str());
}

void stop_speech() {
	SRAL_StopSpeech();
}
string screen_reader_detect() {
	int reader = SRAL_GetCurrentEngine();
	switch (reader) {
	case ENGINE_NONE:
		return "None";
	case ENGINE_NVDA:
		return "NVDA";
	case ENGINE_SAPI:
		return "Sapi";
	case ENGINE_JAWS:
		return "Jaws";
	case ENGINE_SPEECH_DISPATCHER:
		return "SpeechDispatcher";
	case ENGINE_UIA:
		return "UIAutomation";
	default:
		return "None";
	}
	return "None";
}
bool show_window(const string& title, int width, int height, bool closable, bool enable_render)
{
	if (windowRunnable != nullptr) {
		set_window_title(title);
		return true;
	}
	windowRunnable = new WindowThread;
	window_thread_event_shutdown = false;
	if (!enable_render)windowRunnable->start();
	window_title = title.c_str();
	window_w = width;
	window_h = height;
	window_closable = closable;
	// Starting window
	window_event_show = true;
	// Initialize mouse and keyboard state.
	keys_released();
	for (int i = 0; i < 8; i++) {
		mouse_released(i);
	}
	window_event_keyboard_reset = true;
	if (enable_render)windowRunnable->monitor();
	else {
		while (windowRunnable->win == nullptr);
	}
	window_has_renderer = enable_render;
	return windowRunnable->win != 0;
}
void* get_window_handle() {
	if (windowRunnable == nullptr || windowRunnable->win == NULL) {
		return NULL;
	}
#if defined(SDL_PLATFORM_WIN32)
	HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(windowRunnable->win), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	if (hwnd) {
		return(void*)hwnd;
	}
#elif defined(SDL_PLATFORM_MACOS)
	NSWindow* nswindow = (__bridge NSWindow*)SDL_GetPointerProperty(SDL_GetWindowProperties(windowRunnable->win), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
	if (nswindow) {
		return (void*)nswindow;
	}
#elif defined(SDL_PLATFORM_LINUX)
	if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
		Display* xdisplay = (Display*)SDL_GetPointerProperty(SDL_GetWindowProperties(windowRunnable->win), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
		Window xwindow = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(windowRunnable->win), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
		if (xdisplay && xwindow) {
			return(void*)xwindow;
		}
	}
	else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
		struct wl_display* display = (struct wl_display*)SDL_GetPointerProperty(SDL_GetWindowProperties(windowRunnable->win), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
		struct wl_surface* surface = (struct wl_surface*)SDL_GetPointerProperty(SDL_GetWindowProperties(windowRunnable->win), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
		if (display && surface) {
			return(void*)display;
		}
	}
#elif defined(SDL_PLATFORM_IOS)
	SDL_PropertiesID props = SDL_GetWindowProperties(windowRunnable->win);
	UIWindow* uiwindow = (__bridge UIWindow*)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, NULL);
	if (uiwindow) {
		GLuint framebuffer = (GLuint)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_UIKIT_OPENGL_FRAMEBUFFER_NUMBER, 0);
		GLuint colorbuffer = (GLuint)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_UIKIT_OPENGL_RENDERBUFFER_NUMBER, 0);
		GLuint resolveFramebuffer = (GLuint)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_UIKIT_OPENGL_RESOLVE_FRAMEBUFFER_NUMBER, 0);
		return (void*)uiwindow;
	}
#endif
	return nullptr;
}
void hide_window() {
	window_event_hide = true;
	window_thread_event_shutdown = true;
	wait(20);
	windowRunnable->stop();
	windowRunnable->destroy();

	delete windowRunnable;
	windowRunnable = nullptr;
}
void set_window_title(const string& new_title) {
	window_title = new_title.c_str();
	window_event_set_title = true;
	if (window_has_renderer)windowRunnable->monitor();
}
void set_window_closable(bool set_closable) {
	window_closable = set_closable;
}
SDL_Renderer* get_window_renderer() {
	if (windowRunnable != nullptr)return windowRunnable->renderer;
	return nullptr;
}
void window_present() {
	window_event_present = true;
	if (window_has_renderer)windowRunnable->monitor();
}
void garbage_collect() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	engine->GarbageCollect(1 | 2, 1);
}
SDL_Surface* get_window_surface() {
	if (windowRunnable == nullptr || windowRunnable->win == nullptr)return NULL;
	return SDL_GetWindowSurface(windowRunnable->win);
}
SDL_Surface* load_bmp(const string& file) {
	return SDL_LoadBMP(file.c_str());
}
string key_to_string(SDL_Scancode key) {
	return string(SDL_GetScancodeName(key));
}
SDL_Scancode string_to_key(const string& key) {
	return SDL_GetScancodeFromName(key.c_str());
}
bool get_window_active() {
	return window_is_focused;
}
void set_window_fullscreen(bool fullscreen) {
	window_fullscreen = fullscreen;
	window_event_fullscreen = true;
	if (window_has_renderer)windowRunnable->monitor();
}
bool mouse_pressed(unsigned char button)
{
	if (buttons[button].isDown == true and buttons[button].isPressed == false)
	{
		buttons[button].isPressed = true;
		return true;
	}
	return false;
}
bool mouse_released(unsigned char button)
{
	if (buttons[button].isDown == false and buttons[button].isReleased == false)
	{
		buttons[button].isReleased = true;
		return true;
	}
	return false;
}
bool mouse_down(unsigned char button)
{
	return buttons[button].isDown;
}

int get_MOUSE_X() {
	return mouse_x;
}

int get_MOUSE_Y() {
	return mouse_y;
}
int get_MOUSE_Z() {
	return mouse_z;
}
void ftp_download(const string& url, const string& file) {
	try {
		HTTPStreamFactory::registerFactory(); // Must register the HTTP factory to stream using HTTP
		FTPStreamFactory::registerFactory(); // Must register the FTP factory to stream using FTP
		ofstream fileStream;
		fileStream.open(file, ios::out | ios::trunc | ios::binary);

		// Create the URI from the URL to the file.
		URI uri(url);

		// Open the stream and copy the data to the file. 
		unique_ptr<istream> pStr(URIStreamOpener::defaultOpener().open(uri));
		StreamCopier::copyStream(*pStr.get(), fileStream);

		fileStream.close();
	}
	catch (const Poco::Exception& e) {
		asIScriptContext* ctx = asGetActiveContext();
		ctx->SetException(e.displayText().c_str());
	}

}

void mail_send(Poco::Net::SMTPClientSession::LoginMethod login_method, asUINT port, const string& username, const string& password, const string& mailhost, const string& sender, const string& recipient, const string& subject, const string& content, const string& attachment) {
	try {
		MailMessage message;
		message.setSender(sender);
		message.addRecipient(MailRecipient(MailRecipient::PRIMARY_RECIPIENT, recipient));
		message.setSubject(subject);
		message.addContent(new StringPartSource(content));
		if (attachment != "")
			message.addAttachment("Attachment", new StringPartSource(attachment, "file@ f"));
		SMTPClientSession session(mailhost, port);
		session.login(login_method, username, password);
		session.sendMessage(message);
		session.close();
	}
	catch (const Poco::Exception& e) {
		asIScriptContext* ctx = asGetActiveContext();
		ctx->SetException(e.displayText().c_str());
	}

}

void exit_engine(int return_number)
{
	asIScriptContext* ctx = asGetActiveContext();
	if (exit_callback != nullptr) {
		asIScriptEngine* engine = ctx->GetEngine();
		asIScriptContext* e_ctx = engine->RequestContext();
		e_ctx->Prepare(exit_callback);
		e_ctx->Execute();
		int result = e_ctx->GetReturnDWord();
		if (result == 1) {
			e_ctx->Release();
			return;
		}
		e_ctx->Release();
		exit_callback = nullptr;
	}
	if (windowRunnable != nullptr)
		hide_window();
	soundsystem_free();
	enet_deinitialize();
	SDL_Quit();
	SRAL_Uninitialize(); // Keyboard hooks are automatically uninstalls when uninitialize
	CContextMgr* ctxmgr = get_context_manager();
	if (ctxmgr) {
		ctxmgr->AbortAll();
	}
	std::exit(return_number);
}

CScriptArray* keys_pressed() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<int>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)0);
	for (auto i = 0; i < 290; i++) {
		if (key_pressed(i))
			array->InsertLast(&i);
	}
	return array;
}
CScriptArray* keys_released() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<int>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)0);
	for (auto i = 0; i < 290; i++) {
		if (key_released(i))
			array->InsertLast(&i);
	}
	return array;
}
CScriptArray* keys_down() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<int>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)0);
	for (auto i = 0; i < 290; i++) {
		if (key_down(i))
			array->InsertLast(&i);
	}
	return array;
}
CScriptArray* keys_repeat() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<int>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)0);
	for (auto i = 0; i < 290; i++) {
		if (key_repeat(i))
			array->InsertLast(&i);
	}
	return array;
}
string number_to_words(uint64_t num, bool include_and)
{
	vector<char> buf(1024);
	int use_and = (include_and ? 1 : 0);
	size_t bufsize = bl_number_to_words(num, buf.data(), buf.size(), use_and);
	if (bufsize > buf.size())
	{
		return "";
	}
	return string(buf.data());
}
string read_environment_variable(const string& path) {
	SDL_Environment* env = SDL_GetEnvironment();
	const char* value;
	value = SDL_GetEnvironmentVariable(env, path.c_str());
	return string(value);
}
bool write_environment_variable(const string& path, const string& value) {
	SDL_Environment* env = SDL_GetEnvironment();
	int result = SDL_SetEnvironmentVariable(env, path.c_str(), value.c_str(), 1);
	return result == 0;
}
bool clipboard_copy_text(const string& text) {
	SDL_SetClipboardText(text.c_str());
	return true;
}
string clipboard_read_text() {
	return SDL_GetClipboardText();
}
string get_input() {
	string temp = inputtext;
	inputtext = "";
	return temp;
}
string input_box(const string& title, const string& text, const string& default_text, bool secure, bool multiline) {
	int user_pressed;
#ifdef _WIN32
	HWND main_window;
	HWND edit;
	HWND ok;
	HWND cancel;
	main_window = gui::show_window(wstr(title));
	if (main_window == nullptr)return "";
	gui::create_text(main_window, wstr(text).c_str(), 0, 0, 0, 0, 0);
	edit = gui::create_input_box(main_window, secure, multiline, 10, 70, 200, 30, 0);
	ok = gui::create_button(main_window, L"&OK", 50, 120, 80, 30, 0);
	cancel = gui::create_button(main_window, L"&Cancel", 150, 120, 80, 30, 0);
	gui::focus(edit);
	if (default_text != "")gui::add_text(edit, wstr(default_text).c_str());
	while (true) {
		gui::update_window(main_window);
		if (gui::try_close) {
			gui::try_close = false;
			MessageBeep(MB_ICONERROR);
		}
		if (gui::is_pressed(ok) or (gui::key_pressed(0x0d) and gui::get_current_focus() != cancel) && multiline == false) {
			user_pressed = 1;
			break;
		}
		else if (gui::is_pressed(cancel) or gui::key_pressed(0x1b)) {
			user_pressed = 2;
			break;
		}
	}
	gui::delete_control(edit);
	gui::delete_control(ok);
	gui::delete_control(cancel);
	gui::hide_window(main_window);

	if (user_pressed == 1) {
		std::string result;
		std::wstring Wtext = gui::get_text(edit);
		Poco::UnicodeConverter::convert(Wtext, result);
		return result;
	}
	else if (user_pressed == 2) {
		return "";
	}
	return "";
#else 
	std::cout << title << ": " << text << std::endl << default_text;
	std::string result;
	std::getline(std::cin, result);
	if (result.empty()) {
		result = default_text;
	}
	return result;
#endif
}
bool key_pressed(int key_code)
{
	if (keys[key_code].isDown == true and keys[key_code].isPressed == false)
	{
		keys[key_code].isPressed = true;
		return true;
	}
	return false;
}
bool key_released(int key_code)
{
	if (keys[key_code].isDown == false and keys[key_code].isReleased == false)
	{
		keys[key_code].isReleased = true;
		return true;
	}
	return false;
}
bool key_down(int key_code)
{
	return keys[key_code].isDown;
}
bool key_repeat(int key_code)
{
	if (key_pressed(key_code) || (e.key.scancode == key_code && e.key.repeat == true)) {
		return true;
	}
	return false;
}
bool force_key_down(SDL_Scancode keycode) {
	e.type = SDL_EVENT_KEY_DOWN;
	e.key.scancode = keycode;
	keys[e.key.scancode].isDown = true;
	window_event_push = true;
	return key_down(keycode);
}
bool force_key_up(SDL_Scancode keycode) {
	e.type = SDL_EVENT_KEY_UP;
	e.key.scancode = keycode;
	keys[e.key.scancode].isDown = false;
	window_event_push = true;
	return !key_down(keycode);
}
void reset_keyboard() {
	for (int i = 0; i < 512; i++) {
		keys[i].isDown = false;
	}
	window_event_keyboard_reset = true;
}
void string_pad(std::string& text) {
	int padding_size = 16 - (text.length() % 16);
	if (padding_size == 0) {
		padding_size = 16;
	}
	text.append(padding_size, (char)padding_size);
}

void string_unpad(std::string& text) {
	int padding_size = (unsigned char)text.back();
	if (padding_size > 0 && padding_size <= 16) {
		text.resize(text.size() - padding_size);
	}
}



std::string string_encrypt(const string& str, std::string& encryption_key) {
	string the_string = str;
	Poco::SHA2Engine hash;
	hash.update(encryption_key);
	const unsigned char* key_hash = hash.digest().data();

	unsigned char iv[16];
	for (int i = 0; i < 16; ++i) {
		iv[i] = key_hash[i * 2] ^ (4 * i + 1);
	}

	AES_ctx crypt;
	AES_init_ctx_iv(&crypt, key_hash, iv);
	string_pad(the_string);
	AES_CBC_encrypt_buffer(&crypt, reinterpret_cast<uint8_t*>(&the_string.front()), the_string.size());

	// Clear sensitive data
	std::fill(std::begin(iv), std::end(iv), 0);
	std::fill(reinterpret_cast<uint8_t*>(&crypt), reinterpret_cast<uint8_t*>(&crypt) + sizeof(AES_ctx), 0);

	return the_string;
}

std::string string_decrypt(const string& str, string& encryption_key) {
	if (str.size() % 16 != 0) return "";
	string the_string = str;
	Poco::SHA2Engine hash;
	hash.update(encryption_key);
	const unsigned char* key_hash = hash.digest().data();

	unsigned char iv[16];
	for (int i = 0; i < 16; ++i) {
		iv[i] = key_hash[i * 2] ^ (4 * i + 1);
	}

	AES_ctx crypt;
	AES_init_ctx_iv(&crypt, key_hash, iv);
	AES_CBC_decrypt_buffer(&crypt, reinterpret_cast<uint8_t*>(&the_string.front()), the_string.size());

	// Clear sensitive data
	std::fill(std::begin(iv), std::end(iv), 0);
	std::fill(reinterpret_cast<uint8_t*>(&crypt), reinterpret_cast<uint8_t*>(&crypt) + sizeof(AES_ctx), 0);

	string_unpad(the_string);
	return the_string;
}
string url_decode(const string& url) {
	URI uri(url);
	return uri.getPathEtc();
}

string url_encode(const string& url) {
	URI uri;
	uri.setPathEtc(url);
	return uri.toString();
}

string url_get(const string& url) {
	try {
		URI uri(url);
		string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";
		HTTPClientSession* session = nullptr;

		if (uri.getScheme() == "https") {
			SharedPtr<InvalidCertificateHandler> certHandler = new AcceptCertificateHandler(false);
			Context::Ptr sslContext = new Context(Context::CLIENT_USE, "", Context::VERIFY_NONE);
			SSLManager::instance().initializeClient(0, certHandler, sslContext);

			session = new HTTPSClientSession(uri.getHost(), uri.getPort(), sslContext);
		}
		else {
			session = new HTTPClientSession(uri.getHost(), uri.getPort());
		}

		HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		session->sendRequest(request);
		HTTPResponse response;
		istream& rs = session->receiveResponse(response);
		string result;
		StreamCopier::copyToString(rs, result);
		return result;
	}
	catch (const Poco::Exception& e) {
		asIScriptContext* ctx = asGetActiveContext();
		ctx->SetException(e.displayText().c_str());
	}
	return "";
}



string url_post(const string& url, const string& parameters) {
	try {
		HTTPClientSession session(url);
		HTTPRequest request(HTTPRequest::HTTP_POST, "/");
		request.setContentType("application/x-www-form-urlencoded");
		request.setContentLength(parameters.length());
		ostream& os = session.sendRequest(request);
		os << parameters;
		HTTPResponse response;
		istream& rs = session.receiveResponse(response);
		string result;
		StreamCopier::copyToString(rs, result);
		return result;
	}
	catch (const Poco::Exception& e) {
		asIScriptContext* ctx = asGetActiveContext();
		ctx->SetException(e.displayText().c_str());
	}
	return "";
}

string ascii_to_character(int the_ascii_code) {
	return string(1, static_cast<char>(the_ascii_code));
}

int character_to_ascii(string the_character) {
	if (the_character.length() == 1) {
		return static_cast<int>(the_character[0]);
	}
	else {
		return -1; // Error: input string must be of length 1
	}
	return -1;
}

string hex_to_string(string the_hexadecimal_sequence) {
	string decoded;
	istringstream iss(the_hexadecimal_sequence);
	ostringstream oss;
	Poco::HexBinaryDecoder decoder(iss);
	decoder >> decoded;
	return decoded;
}


string number_to_hex_string(asINT64 the_number) {
	return Poco::NumberFormatter::formatHex(the_number);
}

string string_base64_decode(string base64_string) {
	istringstream iss(base64_string);
	ostringstream oss;
	Poco::Base64Decoder decoder(iss);
	decoder >> oss.rdbuf();
	return oss.str();
}


string string_base64_encode(string the_string) {
	istringstream iss(the_string);
	ostringstream oss;
	Poco::Base64Encoder encoder(oss);
	encoder << iss.rdbuf();
	encoder.close();
	return oss.str();
}
string string_base32_decode(string base32_string) {
	istringstream iss(base32_string);
	ostringstream oss;
	Poco::Base32Decoder decoder(iss);
	decoder >> oss.rdbuf();
	return oss.str();
}


string string_base32_encode(string the_string) {
	istringstream iss(the_string);
	ostringstream oss;
	Poco::Base32Encoder encoder(oss);
	encoder << iss.rdbuf();
	encoder.close();
	return oss.str();
}

string string_to_hex(string the_string) {
	ostringstream oss;
	Poco::HexBinaryEncoder encoder(oss);
	encoder << the_string;
	encoder.close();
	return oss.str();
}
int message_box(const std::string& title, const std::string& text, const std::vector<std::string>& buttons, unsigned int mb_flags) {
	std::vector<SDL_MessageBoxButtonData> sdlbuttons;

	for (size_t i = 0; i < buttons.size(); ++i) {
		const std::string& btn = buttons[i];

		if (btn.empty() || btn[0] == '\0') {
			continue; // Skip empty buttons
		}

		unsigned int button_flag = 0;
		size_t skip = 0;

		// Check for special flags in the button text
		if (btn[0] == '`') {
			button_flag |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
			skip++;
		}
		if (skip < btn.size() && btn[skip] == '~') {
			button_flag |= SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
			skip++;
		}

		// Create button data
		SDL_MessageBoxButtonData sdlbtn;
		sdlbtn.flags = button_flag;
		sdlbtn.buttonID = static_cast<int>(i + 1); // Button IDs start from 1
		sdlbtn.text = btn.c_str() + skip;

		sdlbuttons.push_back(sdlbtn);
	}

	SDL_MessageBoxData box = {
		mb_flags,
		SDL_GetKeyboardFocus(),
		title.c_str(),
		text.c_str(),
		static_cast<int>(sdlbuttons.size()),
		sdlbuttons.data(),
		nullptr
	};

	int ret;
	if (!SDL_ShowMessageBox(&box, &ret)) {
		return -1; // Error handling
	}

	return ret;
}


int message_box_script(const std::string& title, const std::string& text, CScriptArray* buttons, unsigned int flags) {
	const unsigned int buttonCount = buttons->GetSize();
	std::vector<std::string> v_buttons;
	v_buttons.reserve(buttonCount); // Reserve space for efficiency

	for (unsigned int i = 0; i < buttonCount; ++i) {
		v_buttons.emplace_back(*(std::string*)(buttons->At(i))); // Use emplace_back for better performance
	}

	return message_box(title, text, v_buttons, flags);
}

bool alert(const string& title, const string& text, const string& button_name)
{
	return message_box(title, text, { "`OK" }, 0) == 0;
}
int question(const string& title, const string& text) {
	return message_box(title, text, { "`Yes", "~No" }, 0);
}

timer g_windowUpdater;
void wait(uint64_t time) {
	if (!window_has_renderer) {
		std::this_thread::sleep_for(std::chrono::milliseconds(time));
		return;
	}
	timer t;
	for (int64_t i = 0; i < time; ++i) {
		if (t.elapsed_millis() > time)break;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (windowRunnable != nullptr && g_windowUpdater.elapsed_millis() > update_window_freq) {
			windowRunnable->monitor();
			g_windowUpdater.restart();
		}
	}
}
void wait_event() {
	if (windowRunnable == nullptr)return;
	if (window_has_renderer) {
		wait_event_requested = true;
		windowRunnable->monitor();
	}
	else
		g_windowEvent.wait();
}
string serialize(CScriptDictionary* data) {
	if (data == nullptr) return "";
	asIScriptContext* context = asGetActiveContext();
	asIScriptEngine* engine = context->GetEngine();
	std::string serialized;
	cmp_buffer buf = { &serialized, 0 };
	cmp_ctx_t sctx = { 0, &buf, cmp_read_bytes, cmp_skip_bytes, cmp_write_bytes };
	uint64_t size = data->GetSize();
	cmp_write_u64(&sctx, size);
	for (auto it : *data) {
		string key = it.GetKey();
		uint64_t len = key.size();
		int32_t type_id = it.GetTypeId();
		cmp_write_u64(&sctx, len);
		cmp_write_str(&sctx, key.c_str(), key.size());
		cmp_write_s32(&sctx, type_id);
		const void* value = it.GetAddressOfValue();
		switch (type_id) {
		case asTYPEID_BOOL:
		{
			bool val = *reinterpret_cast<const bool*>(value);
			cmp_write_bool(&sctx, val);
			break;
		}
		case asTYPEID_FLOAT:
		{
			float val = *reinterpret_cast<const float*>(value);
			cmp_write_float(&sctx, val);
			break;
		}
		case asTYPEID_DOUBLE:
		{
			double val = *reinterpret_cast<const double*> (value);
			cmp_write_double(&sctx, val);
			break;
		}
		case asTYPEID_INT8:
		{
			int8_t val = *reinterpret_cast<const int8_t*>(value);
			cmp_write_s8(&sctx, val);
			break;
		}
		case asTYPEID_INT16:
		{
			int16_t val = *reinterpret_cast<const int16_t*>(value);
			cmp_write_s16(&sctx, val);
			break;
		}
		case asTYPEID_INT32:
		{
			int32_t val = *reinterpret_cast<const int32_t*>(value);
			cmp_write_s32(&sctx, val);
			break;
		}
		case asTYPEID_INT64:
		{
			int64_t val = *reinterpret_cast<const int64_t*>(value);
			cmp_write_s64(&sctx, val);
			break;
		}
		case asTYPEID_UINT8:
		{
			uint8_t val = *reinterpret_cast<const uint8_t*>(value);
			cmp_write_u8(&sctx, val);
			break;
		}
		case asTYPEID_UINT16:
		{
			uint16_t val = *reinterpret_cast<const uint16_t*>(value);
			cmp_write_u16(&sctx, val);
			break;
		}
		case asTYPEID_UINT32:
		{
			uint32_t val = *reinterpret_cast<const uint32_t*>(value);
			cmp_write_u32(&sctx, val);
			break;
		}
		case asTYPEID_UINT64:
		{
			uint64_t val = *reinterpret_cast<const uint64_t*>(value);
			cmp_write_u64(&sctx, val);
			break;
		}
		case 67108876:
		{
			const string& str = *static_cast<const string*>(value);
			cmp_write_u32(&sctx, str.size());
			cmp_write_str(&sctx, str.c_str(), str.size());
			break;
		}

		default:
			cmp_write_bin(&sctx, value, sizeof(value));
			break;
		}
	}
	return serialized;
}

CScriptDictionary* deserialize(const string& data) {
	asIScriptContext* context = asGetActiveContext();
	asIScriptEngine* engine = context->GetEngine();
	CScriptDictionary* dict = CScriptDictionary::Create(engine);
	cmp_buffer buf = { (string*)&data, 0 };
	cmp_ctx_t sctx = { 0, &buf, cmp_read_bytes, cmp_skip_bytes, cmp_write_bytes };
	uint64_t size;
	cmp_read_u64(&sctx, &size);
	for (int i = 0; i < size; i++)
	{
		string key;
		uint64_t len;
		int32_t type_id;
		cmp_read_u64(&sctx, &len);
		uint32_t length = len + 1;
		char* key_char = new char[len];
		cmp_read_str(&sctx, key_char, &length);
		key = string(key_char);
		delete[] key_char;
		cmp_read_s32(&sctx, &type_id);
		if (type_id == asTYPEID_BOOL) {
			bool val;
			cmp_read_bool(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_FLOAT) {
			float val;
			cmp_read_float(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_DOUBLE) {
			double val;
			cmp_read_double(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_INT8) {
			int8_t val;
			cmp_read_s8(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_INT16) {
			int16_t val;
			cmp_read_s16(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_INT32) {
			int32_t val;
			cmp_read_s32(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_INT64) {
			int64_t val;
			cmp_read_s64(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_UINT8) {
			uint8_t val;
			cmp_read_u8(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_UINT16) {
			uint16_t val;
			cmp_read_u16(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_UINT32) {
			uint32_t val;
			cmp_read_u32(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == asTYPEID_UINT64) {
			uint64_t val;
			cmp_read_u64(&sctx, &val);
			dict->Set(key, &val, type_id);
		}
		else if (type_id == 67108876) {
			string val;
			uint32_t length = 0;
			cmp_read_u32(&sctx, &length);
			char* val_char = new char[length];
			length += 1;
			cmp_read_str(&sctx, val_char, &length);
			val = string(val_char);
			delete[] val_char;
			dict->Set(key, &val, type_id);
		}
	}
	return dict;
}
string serialize_array(CScriptArray* data) {
	if (data == nullptr) return "";
	asIScriptContext* context = asGetActiveContext();
	asIScriptEngine* engine = context->GetEngine();
	std::string serialized;
	cmp_buffer buf = { &serialized, 0 };
	cmp_ctx_t sctx = { 0, &buf, cmp_read_bytes, cmp_skip_bytes, cmp_write_bytes };
	int32_t array_tid = data->GetArrayTypeId();
	cmp_write_s32(&sctx, array_tid);
	uint64_t size = data->GetSize();
	cmp_write_u64(&sctx, size);
	for (int i = 0; i < data->GetSize(); i++) {
		int32_t type_id = data->GetElementTypeId();
		cmp_write_s32(&sctx, type_id);
		const void* value = data->At(i);
		switch (type_id) {
		case asTYPEID_BOOL:
		{
			bool val = *reinterpret_cast<const bool*>(value);
			cmp_write_bool(&sctx, val);
			break;
		}
		case asTYPEID_FLOAT:
		{
			float val = *reinterpret_cast<const float*>(value);
			cmp_write_float(&sctx, val);
			break;
		}
		case asTYPEID_DOUBLE:
		{
			double val = *reinterpret_cast<const double*> (value);
			cmp_write_double(&sctx, val);
			break;
		}
		case asTYPEID_INT8:
		{
			int8_t val = *reinterpret_cast<const int8_t*>(value);
			cmp_write_s8(&sctx, val);
			break;
		}
		case asTYPEID_INT16:
		{
			int16_t val = *reinterpret_cast<const int16_t*>(value);
			cmp_write_s16(&sctx, val);
			break;
		}
		case asTYPEID_INT32:
		{
			int32_t val = *reinterpret_cast<const int32_t*>(value);
			cmp_write_s32(&sctx, val);
			break;
		}
		case asTYPEID_INT64:
		{
			int64_t val = *reinterpret_cast<const int64_t*>(value);
			cmp_write_s64(&sctx, val);
			break;
		}
		case asTYPEID_UINT8:
		{
			uint8_t val = *reinterpret_cast<const uint8_t*>(value);
			cmp_write_u8(&sctx, val);
			break;
		}
		case asTYPEID_UINT16:
		{
			uint16_t val = *reinterpret_cast<const uint16_t*>(value);
			cmp_write_u16(&sctx, val);
			break;
		}
		case asTYPEID_UINT32:
		{
			uint32_t val = *reinterpret_cast<const uint32_t*>(value);
			cmp_write_u32(&sctx, val);
			break;
		}
		case asTYPEID_UINT64:
		{
			uint64_t val = *reinterpret_cast<const uint64_t*>(value);
			cmp_write_u64(&sctx, val);
			break;
		}
		case 67108876:
		{
			const string& str = *static_cast<const string*>(value);
			cmp_write_u32(&sctx, str.size());
			cmp_write_str(&sctx, str.c_str(), str.size());
			break;
		}

		default:
			cmp_write_bin(&sctx, value, sizeof(value));
			break;
		}
	}
	return serialized;
}

CScriptArray* deserialize_array(const string& data) {
	asIScriptContext* context = asGetActiveContext();
	asIScriptEngine* engine = context->GetEngine();
	int32_t array_tid;
	cmp_buffer buf = { (string*)&data, 0 };
	cmp_ctx_t sctx = { 0, &buf, cmp_read_bytes, cmp_skip_bytes, cmp_write_bytes };
	cmp_read_s32(&sctx, &array_tid);
	asITypeInfo* arrayType = engine->GetTypeInfoById(array_tid);
	CScriptArray* array = CScriptArray::Create(arrayType);
	uint64_t size;
	cmp_read_u64(&sctx, &size);
	array->Reserve(size);
	for (int i = 0; i < size; i++)
	{
		int32_t type_id;
		cmp_read_s32(&sctx, &type_id);
		if (type_id == asTYPEID_BOOL) {
			bool val;
			cmp_read_bool(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_FLOAT) {
			float val;
			cmp_read_float(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_DOUBLE) {
			double val;
			cmp_read_double(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_INT8) {
			int8_t val;
			cmp_read_s8(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_INT16) {
			int16_t val;
			cmp_read_s16(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_INT32) {
			int32_t val;
			cmp_read_s32(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_INT64) {
			int64_t val;
			cmp_read_s64(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_UINT8) {
			uint8_t val;
			cmp_read_u8(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_UINT16) {
			uint16_t val;
			cmp_read_u16(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_UINT32) {
			uint32_t val;
			cmp_read_u32(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == asTYPEID_UINT64) {
			uint64_t val;
			cmp_read_u64(&sctx, &val);
			array->InsertAt(i, &val);
		}
		else if (type_id == 67108876) {
			string val;
			uint32_t length = 0;
			cmp_read_u32(&sctx, &length);
			char* val_char = new char[length];
			length += 1;
			cmp_read_str(&sctx, val_char, &length);
			val = string(val_char);
			delete[] val_char;
			array->InsertAt(i, &val);
		}
	}
	return array;
}


bool urlopen(const string& url) {
	return SDL_OpenURL(url.c_str()) == 0;
}
std::string c_str_to_string(const char* ptr, size_t length) {
	if (length == 0) {
		return std::string(ptr);
	}
	else {
		return std::string(ptr, length);
	}
}
std::wstring wc_str_to_wstring(const wchar_t* ptr, size_t length) {
	if (length == 0) {
		return std::wstring(ptr);
	}
	else {
		return std::wstring(ptr, length);
	}
}


uint64_t timer::elapsed_seconds() {
	return pausedNanos != 0 ? chrono::duration_cast<chrono::seconds>(chrono::nanoseconds(pausedNanos)).count()
		: chrono::duration_cast<chrono::seconds>(
			chrono::steady_clock::now() - inittime).count();
}
uint64_t timer::elapsed_minutes() {
	return pausedNanos != 0 ? chrono::duration_cast<chrono::minutes>(chrono::nanoseconds(pausedNanos)).count()
		: chrono::duration_cast<chrono::minutes>(
			chrono::steady_clock::now() - inittime).count();
}
uint64_t timer::elapsed_hours() {
	return pausedNanos != 0 ? chrono::duration_cast<chrono::hours>(chrono::nanoseconds(pausedNanos)).count()
		: chrono::duration_cast<chrono::hours>(
			chrono::steady_clock::now() - inittime).count();
}
uint64_t timer::elapsed_millis() {
	return pausedNanos != 0 ? chrono::duration_cast<chrono::milliseconds>(chrono::nanoseconds(pausedNanos)).count()
		: chrono::duration_cast<chrono::milliseconds>(
			chrono::steady_clock::now() - inittime).count();
}

uint64_t timer::elapsed_micros() {
	return pausedNanos != 0 ? chrono::duration_cast<chrono::microseconds>(chrono::nanoseconds(pausedNanos)).count()
		: chrono::duration_cast<chrono::microseconds>(
			chrono::steady_clock::now() - inittime).count();
}
uint64_t timer::elapsed_nanos() {
	return pausedNanos != 0 ? pausedNanos
		: chrono::duration_cast<chrono::nanoseconds>(
			chrono::steady_clock::now() - inittime).count();
}
void timer::force_seconds(uint64_t seconds) {
	inittime = chrono::steady_clock::now() - chrono::seconds(seconds);
	pausedNanos = 0;
}
void timer::force_minutes(uint64_t minutes) {
	inittime = chrono::steady_clock::now() - chrono::minutes(minutes);
	pausedNanos = 0;
}
void timer::force_hours(uint64_t hours) {
	inittime = chrono::steady_clock::now() - chrono::hours(hours);
	pausedNanos = 0;
}
void timer::force_millis(uint64_t millis) {
	inittime = chrono::steady_clock::now() - chrono::milliseconds(millis);
	pausedNanos = 0;
}

// Force the timer to a specific time in microseconds
void timer::force_micros(uint64_t micros) {
	inittime = chrono::steady_clock::now() - chrono::microseconds(micros);
	pausedNanos = 0;
}

// Force the timer to a specific time in nanoseconds
void timer::force_nanos(uint64_t nanos) {
	inittime = chrono::steady_clock::now() - chrono::nanoseconds(nanos);
	pausedNanos = 0;
}

void timer::restart() {
	inittime = chrono::steady_clock::now();
	pausedNanos = 0;
}
void timer::pause() {
	if (pausedNanos == 0) {
		pausedNanos = chrono::duration_cast<chrono::nanoseconds>(
			chrono::steady_clock::now() - inittime).count();
	}
}
bool timer::is_running() {
	return pausedNanos == 0;
}

void timer::resume() {
	if (pausedNanos != 0) {
		inittime = chrono::steady_clock::now() - chrono::nanoseconds(pausedNanos);
		pausedNanos = 0;
	}
}
network_event& network_event::operator=(const network_event new_event) {
	this->m_peerId = new_event.m_peerId;
	this->m_type = new_event.m_type;
	this->m_message = new_event.m_message;
	this->m_channel = new_event.m_channel;
	return *this;
}


_ENetPeer* network::get_peer(asQWORD peer_id) {
	auto it = peers.find(peer_id);
	if (it == peers.end())return 0;
	return it->second;
}

asQWORD network::connect(const string& hostAddress, int port) {
	ENetAddress enetAddress;
	enet_address_set_host(&enetAddress, hostAddress.c_str());
	enetAddress.port = port;

	int initial_timeout = 0;
	int timeout_multiplier = 2;
	int max_retries = 1000;
	int retry_count = 0;

	while (retry_count < max_retries) {
		ENetPeer* peer = enet_host_connect(host, &enetAddress, 1, 0);
		if (peer != NULL) {
			peers[current_peer_id] = peer;
			peer->data = reinterpret_cast<void*>(current_peer_id);
			current_peer_id += 1;
			return current_peer_id - 1;
		}
		else {
			retry_count++;
		}
	}

	return 0;
}

bool network::destroy() {
	if (host) {
		enet_host_destroy(host);
		host = NULL;
		current_peer_id = 1;
		peers.clear();
		return true;
	}
	return false;
}
bool network::disconnect_peer(asQWORD peer_id) {
	if (host) {
		_ENetPeer* peer = get_peer(peer_id);
		if (!peer)return false;
		enet_peer_disconnect(peer, 0);
		peers.erase(peer_id);
		return true;
	}
	return false;
}
bool network::disconnect_peer_forcefully(asQWORD peer_id) {
	if (host) {
		_ENetPeer* peer = get_peer(peer_id);
		if (!peer)return false;
		enet_peer_reset(peer);
		peers.erase(peer_id);
		return true;
	}
	return false;

}
string address_to_string(const ENetAddress* address)
{
	uint8_t a = (uint8_t)(address->host);
	uint8_t b = (uint8_t)(address->host >> 8);
	uint8_t c = (uint8_t)(address->host >> 16);
	uint8_t d = (uint8_t)(address->host >> 24);
	return to_string(a) + "." + to_string(b) + "." + to_string(c) + "." + to_string(d) + ":" + to_string(address->port);
}


string network::get_peer_address(asQWORD peer_id) {
	_ENetPeer* peer = get_peer(peer_id);
	if (!peer)return "NULL";
	return address_to_string(&peer->address);
}
double network::get_peer_average_round_trip_time(asQWORD peerId) {
	_ENetPeer* peer = get_peer(peerId);
	if (!peer)return -1;
	return peer->roundTripTime;
}
CScriptArray* network::get_peer_list() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<uint64>"));
	CScriptArray* peers_array = CScriptArray::Create(arrayType, (asUINT)0);
	for (auto it = peers.begin(); it != peers.end(); it++) {
		asQWORD peer_id = it->first;
		peers_array->InsertLast(&peer_id);
	}
	return peers_array;
}
network_event* network::request(int initial_timeout) {
	network_event* handle_ = new network_event;
	int timeout = initial_timeout;
	if (!host)return handle_;
	ENetEvent event;
	int result = enet_host_service(host, &event, timeout);
	handle_->m_type = event.type;
	handle_->m_channel = event.channelID;
	if (event.type == ENET_EVENT_TYPE_CONNECT) {
		enet_peer_timeout(event.peer, 128, 10000, 35000);

		if (this->type == NETWORK_TYPE_SERVER) {
			event.peer->data = reinterpret_cast<void*>(current_peer_id);
			peers[current_peer_id] = event.peer;
			handle_->m_peerId = current_peer_id;
			current_peer_id += 1;
		}
		else if (this->type == NETWORK_TYPE_CLIENT) {
			handle_->m_peerId = (asQWORD)(event.peer->data);
		}
	}
	if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
		asQWORD peer = (asQWORD)(event.peer->data);
		event.peer->data = 0;
		if (peer > 0)
			peers.erase(peer);
		handle_->m_peerId = peer;
	}
	if (event.type == ENET_EVENT_TYPE_RECEIVE && event.packet != nullptr) {
		handle_->m_peerId = (asQWORD)(event.peer->data);
		handle_->m_message = string(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
		enet_packet_destroy(event.packet);
	}

	return handle_;
}

bool network::send_reliable(asQWORD peer_id, const string& packet, int channel) {
	ENetPacket* p = enet_packet_create(packet.c_str(), strlen(packet.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_packet_resize(p, packet.size());
	if (peer_id > 0) {
		_ENetPeer* peer = get_peer(peer_id);
		if (!peer)return false;
		int result = enet_peer_send(peer, channel, p);
		if (result == 0)
			return true;
	}
	else
	{
		if (!host) return false;
		enet_host_broadcast(host, channel, p);
		return true;
	}
	return false;
}
bool network::send_unreliable(asQWORD peer_id, const string& packet, int channel) {
	ENetPacket* p = enet_packet_create(packet.c_str(), strlen(packet.c_str()) + 1, 0);
	enet_packet_resize(p, packet.size());
	if (peer_id > 0) {
		_ENetPeer* peer = get_peer(peer_id);
		if (!peer)return false;
		int result = enet_peer_send(peer, channel, p);
		if (result == 0)
			return true;
	}
	else
	{
		if (!host) return false;
		enet_host_broadcast(host, channel, p);
		return true;
	}
	return false;
}
bool network::set_bandwidth_limits(double incomingBandwidth, double outgoingBandwidth) {
	enet_host_bandwidth_limit(host, incomingBandwidth, outgoingBandwidth);
	return true;
}

bool network::setup_client(int channels, asQWORD maxPeers) {
	address.host = ENET_HOST_ANY;
	address.port = 0; // Let the system choose a port

	host = enet_host_create(NULL, 1, channels, 0, 0);

	if (host == NULL) {
		return false; // Failed to create host
	}

	m_active = true;
	this->type = NETWORK_TYPE_CLIENT;
	return true;
}

bool network::setup_server(int listeningPort, int channels, asQWORD maxPeers) {
	address.host = ENET_HOST_ANY;
	address.port = listeningPort;

	host = enet_host_create(&address, maxPeers, channels, 0, 0);

	if (host == NULL) {
		return false; // Failed to create host
	}

	m_active = true;
	this->type = NETWORK_TYPE_SERVER;
	return true;
}
void network::flush() {
	enet_host_flush(host);
}
asQWORD network::get_connected_peers() const {
	return host ? host->connectedPeers : 0;
}

double network::get_bytes_sent() const {
	return m_bytesSent;
}

double network::get_bytes_received() const {
	return m_bytesReceived;
}

bool network::is_active() const {
	return m_active;
}
bool library::load(const string& libname) {
	if (lib != nullptr)this->unload();
	lib = SDL_LoadObject(libname.c_str());
	return lib != NULL;
}
bool library::active()const {
	return lib != nullptr;
}
void library_call(asIScriptGeneric* gen) {
#undef GetObject
	/*
//	Soon LibFFI will not be needed. I will write on the engine itself the ability to call functions.
//The point is that there are additional build issues with FFI, so we'll use scripting::context for this.
//And this will work if you handle pointers, strings, etc. correctly.
	asIScriptContext* ctx = asGetActiveContext();
		library* lib_obj = (library*)gen->GetObject();
		void* address = nullptr;
		void* ref = gen->GetArgAddress(0);
		std::string func_name = *static_cast<std::string*>(ref);
		ffi_cif cif;
		ffi_type* return_type = nullptr;
		std::vector<ffi_type*> arg_types;
		// Function signature parser:
		std::vector<std::string> tokens;
		std::vector<std::string> paramTypes;
		std::string token;
		for (char c : func_name) {
			if (c == ' ' || c == '(' || c == ')' || c == ',' || c == ';') {
				if (!token.empty()) {
					tokens.push_back(token);
					token.clear();
				}
			}
			else {
				token += c;
			}
		}

		// First array: Return type and function name
		std::vector<std::string> first;
		first.push_back(tokens[0]);  // Return type
		first.push_back(tokens[1]);  // Function name
		auto it = lib_obj->functions.find(first[1]);
		if (it != lib_obj->functions.end()) {
			LibraryFunction func = it->second;
			return_type = func.returnType;
			arg_types = func.parameters;
			paramTypes = func.parameterTypes;
			address = (void*)func.address;
		}
		else {
			address = SDL_LoadFunction(lib_obj->lib, first[1].c_str());
			if (address == NULL) {
				const char* name = first[1].c_str();
				std::string message = "Signature parse error: GetProcAddress failed for '" + std::string(name) + "'";
				ctx->SetException(message.c_str());
				return;
			}

			// Second array: Parameters
			std::vector<std::string> last;
			for (size_t i = 2; i < tokens.size(); ++i) {
				if (!tokens[i].empty()) {
					last.push_back(tokens[i]);
				}
			}

			// Now handle keywords
			std::vector<std::string> returnType = split(first[0], " "); // Split the return type string
			for (auto& param : last) {
				paramTypes.push_back(split(param, " ")[0]); // Split the parameter type string
			}

			// Handle return type
			std::string realReturnType = returnType[returnType.size() - 1]; // Get the last part as the actual type
			for (auto& type : returnType) {
				if (type == "const" || type == "unsigned" || type == "signed") {
					realReturnType = type + " " + realReturnType; // Add keyword to the type
				}
			}

			// Handle parameter types
			for (size_t i = 0; i < paramTypes.size(); i++) {
				std::string realParamType = paramTypes[i];
				for (auto& type : split(last[i], " ")) {
					if (type == "const" || type == "unsigned" || type == "signed") {
						realParamType = type + " " + realParamType; // Add keyword to the type
					}
				}
				paramTypes[i] = realParamType;
			}

			// Prepare to call the function using libffi

			// Determine return type and argument types for libffi

			if (realReturnType.find("void") != std::string::npos) {
				return_type = &ffi_type_void;
			}
			else if (realReturnType.find("*") != std::string::npos) {
				return_type = &ffi_type_pointer;
			}
			else {
				// Handle all integer types as int64_t
				if (realReturnType.find("char") != std::string::npos ||
					realReturnType.find("int") != std::string::npos ||
					realReturnType.find("short") != std::string::npos ||
					realReturnType.find("long") != std::string::npos) {
					return_type = &ffi_type_sint64;
				}
				else if (realReturnType.find("float") != std::string::npos ||
					realReturnType.find("double") != std::string::npos) {
					return_type = &ffi_type_double;
				}
				else {
					// Handle unexpected type
					std::string message = "Unsupported return type: " + realReturnType;
					ctx->SetException(message.c_str());
					return;
				}
			}
			arg_types.resize(last.size());
			// Handle parameter types
			for (size_t i = 0; i < last.size(); ++i) {
				if (paramTypes[i].find("*") != std::string::npos) {
					arg_types[i] = &ffi_type_pointer;
				}
				else {
					// Handle all integer types as int64_t
					if (paramTypes[i].find("char") != std::string::npos ||
						paramTypes[i].find("int") != std::string::npos ||
						paramTypes[i].find("short") != std::string::npos ||
						paramTypes[i].find("long") != std::string::npos) {
						arg_types[i] = &ffi_type_sint64;
					}
					else if (paramTypes[i].find("float") != std::string::npos ||
						paramTypes[i].find("double") != std::string::npos) {
						arg_types[i] = &ffi_type_double;
					}
					else {
						// Handle unexpected type
						std::string message = "Unsupported parameter type: " + paramTypes[i];
						ctx->SetException(message.c_str());
						return;
					}
				}
			}
			LibraryFunction lf;
			lf.address = (void*)address;
			lf.returnType = return_type;
			lf.parameters = arg_types;
			lf.parameterTypes = paramTypes;
			lib_obj->functions[first[1]] = lf; // Store a copy, not a pointer
		}
		// Prepare the CIF
		if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_types.size(), return_type, arg_types.data()) != FFI_OK) {
			ctx->SetException("Failed to prepare CIF for libffi");
			return;
		}

		// Prepare arguments for the call
		std::vector<void*> args(arg_types.size());
		for (size_t i = 0; i < arg_types.size(); ++i) {
			if (arg_types[i] == &ffi_type_pointer) {
				// It's a pointer, but check if it's char
				if (paramTypes[i].find("char") != std::string::npos) {
					// It's a char - use GetArgAddress
					args[i] = gen->GetArgAddress(i + 1);
				}
				else {
					// It's a pointer other than char - use GetAddressOfArg
					args[i] = gen->GetAddressOfArg(i + 1);
				}
			}
			else {
				// Not a pointer - use GetArgAddress
				args[i] = gen->GetArgAddress(i + 1);
			}
		}	asIScriptEngine* engine = ctx->GetEngine();
		CScriptDictionary* dict = CScriptDictionary::Create(engine);
		// Call the function
		// Handle return value if necessary
		if (return_type == &ffi_type_sint64) {
			asINT64 return_value;
			ffi_call(&cif, FFI_FN(address), &return_value, args.data());
			dict->Set("0", return_value);
		}
		else if (return_type == &ffi_type_double) {
			double return_value;
			ffi_call(&cif, FFI_FN(address), &return_value, args.data());
			dict->Set("0", return_value);
		}
		else if (return_type == &ffi_type_void) {
			// No return value to handle
			ffi_call(&cif, FFI_FN(address), nullptr, args.data());
		}
		else if (return_type == &ffi_type_pointer) {
			void* retval = nullptr;
			ffi_call(&cif, FFI_FN(address), &retval, args.data());
			asINT64 ptr = *(asINT64*)retval;
			dict->Set("0", ptr);
		}
		for (size_t i = 0; i < args.size(); ++i) {
			asINT64 ptr = *(asINT64*)args[i];
			dict->Set(std::to_string(i + 1), ptr);
		}
		arg_types.clear();
		args.clear();
		return_type = nullptr;
		gen->SetReturnObject(dict);
	*/
}
void library::unload() {
	SDL_UnloadObject(lib);
	functions.clear();
}
void* library::get_function_pointer(const std::string& name) {
	if (lib == nullptr)return nullptr;
	return (void*)SDL_LoadFunction(lib, name.c_str());
}
void library::clear_functions() {
	functions.clear();
}
script_thread::script_thread(asIScriptFunction* func) {
	function = func;
	asIScriptContext* current_context = asGetActiveContext();
	if (function == 0) {
		current_context->SetException("Function is null");
	}
	asIScriptEngine* current_engine = current_context->GetEngine();
	context = current_engine->RequestContext();
	context->Prepare(function);
}
void script_thread::detach() {
	thread t([this]() {
		this->context->Execute();
		});
	t.detach();
}
void script_thread::join() {
	thread t([this]() {
		this->context->Execute();
		});
	t.join();
}
void script_thread::destroy() {
	this->context->Release();
}

bool instance::is_running() {
	if (mutex->tryLock() == true) {
		mutex->unlock();
		return false;
	}
	return true;
}

user_idle::user_idle() {}
uint64_t user_idle::elapsed_millis() {
	return get_idle_time();
}
uint64_t user_idle::elapsed_seconds() {
	return get_idle_time() / 1000;
}
uint64_t user_idle::elapsed_minutes() {
	return elapsed_seconds() / 60;
}
uint64_t user_idle::elapsed_hours() {
	return elapsed_minutes() / 60;
}
uint64_t user_idle::elapsed_days() {
	return elapsed_hours() / 24;
}
uint64_t user_idle::elapsed_weeks() {
	return elapsed_days() / 7;
}
uint64_t user_idle::get_idle_time() {
#ifdef _WIN32
	LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
	GetLastInputInfo(&lii);
	DWORD currentTick = GetTickCount();
	return (currentTick - lii.dwTime);
#endif
	return 0;
}

uint64_t get_time_stamp_millis() {
	auto now = chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
	return millis;
}

uint64_t get_time_stamp_seconds() {
	auto now = chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto seconds = chrono::duration_cast<chrono::seconds>(duration).count();
	return seconds;
}
float ngtvector::get_length()const {
	return sqrt(x * x + y * y + z * z);
}
ngtvector& ngtvector::operator=(const ngtvector new_vector) {
	this->x = new_vector.x;
	this->y = new_vector.y;
	this->z = new_vector.z;

	return *this;
}
void ngtvector::reset() {
	x = 0;
	y = 0;
	z = 0;
}
ngtvector::ngtvector() {
	this->reset();
}
int sqlite3statement::step() { return sqlite3_step(stmt); }
int sqlite3statement::reset() { return sqlite3_reset(stmt); }
string sqlite3statement::get_expanded_sql_statement() const { return sqlite3_expanded_sql(stmt); }
string sqlite3statement::get_sql_statement() const { return sqlite3_sql(stmt); }
int sqlite3statement::get_bind_param_count() const { return sqlite3_bind_parameter_count(stmt); }
int sqlite3statement::get_column_count() const { return sqlite3_column_count(stmt); }
int sqlite3statement::bind_blob(int index, const string& value, bool copy) { return sqlite3_bind_blob(stmt, index, value.c_str(), value.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC); }
int sqlite3statement::bind_double(int index, double value) { return sqlite3_bind_double(stmt, index, value); }
int sqlite3statement::bind_int(int index, int value) { return sqlite3_bind_int(stmt, index, value); }
int sqlite3statement::bind_int64(int index, int64_t value) { return sqlite3_bind_int64(stmt, index, value); }
int sqlite3statement::bind_null(int index) { return sqlite3_bind_null(stmt, index); }
int sqlite3statement::bind_param_index(const string& name) { return sqlite3_bind_parameter_index(stmt, name.c_str()); }
string sqlite3statement::bind_param_name(int index) { return sqlite3_bind_parameter_name(stmt, index); }
int sqlite3statement::bind_text(int index, const string& value, bool copy) { return sqlite3_bind_text(stmt, index, value.c_str(), value.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC); }
int sqlite3statement::clear_bindings() { return sqlite3_clear_bindings(stmt); }
string sqlite3statement::column_blob(int index) { return string(reinterpret_cast<const char*>(sqlite3_column_blob(stmt, index)), sqlite3_column_bytes(stmt, index)); }
int sqlite3statement::column_bytes(int index) { return sqlite3_column_bytes(stmt, index); }
string sqlite3statement::column_decltype(int index) { return sqlite3_column_decltype(stmt, index); }
double sqlite3statement::column_double(int index) { return sqlite3_column_double(stmt, index); }
int sqlite3statement::column_int(int index) { return sqlite3_column_int(stmt, index); }
int64_t sqlite3statement::column_int64(int index) { return sqlite3_column_int64(stmt, index); }
string sqlite3statement::column_name(int index) { return sqlite3_column_name(stmt, index); }
int sqlite3statement::column_type(int index) { return sqlite3_column_type(stmt, index); }
string sqlite3statement::column_text(int index) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, index)); }

ngtsqlite3::ngtsqlite3() {
	sqlite3_initialize();
}
int ngtsqlite3::close() { return sqlite3_close(db); }

int ngtsqlite3::open(const string& filename, int flags) { return sqlite3_open_v2(filename.c_str(), &db, flags, nullptr); }
sqlite3statement* ngtsqlite3::prepare(const string& name, int& out) {
	sqlite3statement* statement = new sqlite3statement();
	if (sqlite3_prepare_v2(db, name.c_str(), -1, &statement->stmt, 0) != SQLITE_OK)
	{
		out = sqlite3_errcode(db);
		return nullptr;
	}
	return statement;
}

int ngtsqlite3::execute(const string& sql)
{
	return sqlite3_exec(db, sql.c_str(), 0, 0, 0);
}

int64_t ngtsqlite3::get_rows_changed() const { return sqlite3_changes(db); }

int64_t ngtsqlite3::get_total_rows_changed() const { return sqlite3_total_changes(db); }

int ngtsqlite3::limit(int id, int val) { return sqlite3_limit(db, id, val); }

int ngtsqlite3::set_authorizer(sqlite3_authorizer* callback, const string& arg) { return 0; }

int64_t ngtsqlite3::get_last_insert_rowid() const { return sqlite3_last_insert_rowid(db); }

void ngtsqlite3::set_last_insert_rowid(int64_t id) { sqlite3_set_last_insert_rowid(db, id); }

int ngtsqlite3::get_last_error() { return sqlite3_errcode(db); }

string ngtsqlite3::get_last_error_text() { return sqlite3_errmsg(db); }

bool ngtsqlite3::get_active() const { return db != nullptr; }

