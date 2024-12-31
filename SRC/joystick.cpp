#include "joystick.h"
#include "scriptarray/scriptarray.h"
#include <angelscript.h>
#include <atomic>
#include <mutex>
#include <SDL3/SDL.h>
#include <string>
#include <vector>

std::vector<SDL_JoystickID> GetJoystickList() {
	std::vector<SDL_JoystickID> joysticks;
	if (!SDL_HasJoystick())return joysticks;
	int count;
	SDL_JoystickID* j = SDL_GetJoysticks(&count);
	if (count == 0 || !j)return joysticks;
	for (int i = 0; i < count; ++i) {
		joysticks.push_back(j[i]);
	}
	SDL_free(j);
	return joysticks;
}


CScriptArray* GetJoystickListScript() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<uint32>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)0);
	std::vector<SDL_JoystickID> joysticks = GetJoystickList();
	if (joysticks.empty())return array;
	for (size_t i = 0; i < joysticks.size(); ++i) {
		array->InsertLast(&joysticks[i]);
	}
	return array;
}

class Joystick {
public:
	Joystick(SDL_JoystickID instance_id) : m_instance_id(instance_id), m_ref_count(1) {
		SDL_LockJoysticks();
		m_joystick = SDL_OpenJoystick(m_instance_id);
		if (!m_joystick) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open joystick: %s", SDL_GetError());
		}
		SDL_UnlockJoysticks();
	}

	~Joystick() {
		SDL_LockJoysticks();
		if (m_joystick) {
			SDL_CloseJoystick(m_joystick);
			m_joystick = nullptr;
		}
		SDL_UnlockJoysticks();
	}

	Joystick(const Joystick& other) : m_instance_id(other.m_instance_id), m_ref_count(1) {
		SDL_LockJoysticks();
		m_joystick = SDL_OpenJoystick(m_instance_id);
		if (!m_joystick) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open joystick: %s", SDL_GetError());
		}
		SDL_UnlockJoysticks();
	}

	Joystick& operator=(const Joystick& other) {
		if (this != &other) {
			SDL_LockJoysticks();
			if (m_joystick) {
				SDL_CloseJoystick(m_joystick);
				m_joystick = nullptr;
			}
			m_instance_id = other.m_instance_id;
			m_ref_count = 1;
			m_joystick = SDL_OpenJoystick(m_instance_id);
			if (!m_joystick) {
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open joystick: %s", SDL_GetError());
			}
			SDL_UnlockJoysticks();
		}
		return *this;
	}

	bool IsValid() const {
		return m_joystick != nullptr;
	}

	SDL_Joystick* GetPointer() const {
		return m_joystick;
	}

	std::string GetName()const {
		return !IsValid() ? "" : std::string(SDL_GetJoystickName(m_joystick));
	}

	std::string GetPath()const {
		return !IsValid() ? "" : std::string(SDL_GetJoystickPath(m_joystick));
	}

	std::string GetSerial()const {
		return !IsValid() ? "" : std::string(SDL_GetJoystickSerial(m_joystick));
	}

	SDL_JoystickType GetType()const {
		return SDL_GetJoystickType(m_joystick);
	}

	bool Connected()const {
		return SDL_JoystickConnected(m_joystick);
	}

	int GetNumAxes()const {
		return SDL_GetNumJoystickAxes(m_joystick);
	}

	int GetNumBalls()const {
		return SDL_GetNumJoystickBalls(m_joystick);
	}

	int GetNumHats() const {
		return SDL_GetNumJoystickHats(m_joystick);
	}

	int GetNumButtons() const {
		return SDL_GetNumJoystickButtons(m_joystick);
	}

	void SetEventsEnabled(bool enabled) {
		SDL_SetJoystickEventsEnabled(enabled);
	}

	bool EventsEnabled() const {
		return SDL_JoystickEventsEnabled();
	}

	Sint16 GetAxis(int axis) const {
		return SDL_GetJoystickAxis(m_joystick, axis);
	}

	bool GetAxisInitialState(int axis, Sint16& state) const {
		return SDL_GetJoystickAxisInitialState(m_joystick, axis, &state);
	}

	bool GetBall(int ball, int& dx, int& dy) const {
		return SDL_GetJoystickBall(m_joystick, ball, &dx, &dy);
	}

	Uint8 GetHat(int hat) const {
		return SDL_GetJoystickHat(m_joystick, hat);
	}

	bool GetButton(int button) const {
		return SDL_GetJoystickButton(m_joystick, button);
	}

	bool Rumble(Uint16 low_frequency_rumble, Uint16 high_frequency_rumble, Uint32 duration_ms) const {
		return SDL_RumbleJoystick(m_joystick, low_frequency_rumble, high_frequency_rumble, duration_ms);
	}

	bool RumbleTriggers(Uint16 left_rumble, Uint16 right_rumble, Uint32 duration_ms) const {
		return SDL_RumbleJoystickTriggers(m_joystick, left_rumble, right_rumble, duration_ms);
	}

	bool SetLED(Uint8 red, Uint8 green, Uint8 blue) const {
		return SDL_SetJoystickLED(m_joystick, red, green, blue);
	}

	bool SendEffect(const void* data, int size) const {
		return SDL_SendJoystickEffect(m_joystick, data, size);
	}

	SDL_JoystickConnectionState GetConnectionState() const {
		return SDL_GetJoystickConnectionState(m_joystick);
	}

	SDL_PowerState GetPowerInfo(int& percent) const {
		return SDL_GetJoystickPowerInfo(m_joystick, &percent);
	}

	void AddRef() {
		++m_ref_count;
	}

	void Release() {
		if (--m_ref_count == 0) {
			delete this;
		}
	}

private:
	SDL_JoystickID m_instance_id;
	std::atomic<int> m_ref_count;
	SDL_Joystick* m_joystick;
};

Joystick* JoystickFactory(SDL_JoystickID instance) {
	return new Joystick(instance);
}

void RegisterJoystick(asIScriptEngine* engine) {
	engine->RegisterEnum("joysticktype");

	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_UNKNOWN", SDL_JOYSTICK_TYPE_UNKNOWN);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_GAMEPAD", SDL_JOYSTICK_TYPE_GAMEPAD);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_WHEEL", SDL_JOYSTICK_TYPE_WHEEL);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_ARCADE_STICK", SDL_JOYSTICK_TYPE_ARCADE_STICK);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_FLIGHT_STICK", SDL_JOYSTICK_TYPE_FLIGHT_STICK);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_DANCE_PAD", SDL_JOYSTICK_TYPE_DANCE_PAD);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_GUITAR", SDL_JOYSTICK_TYPE_GUITAR);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_DRUM_KIT", SDL_JOYSTICK_TYPE_DRUM_KIT);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_ARCADE_PAD", SDL_JOYSTICK_TYPE_ARCADE_PAD);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_THROTTLE", SDL_JOYSTICK_TYPE_THROTTLE);
	engine->RegisterEnumValue("joysticktype", "JOYSTICK_TYPE_COUNT", SDL_JOYSTICK_TYPE_COUNT);

	engine->RegisterGlobalFunction("bool get_has_joystick() property", asFUNCTION(SDL_HasJoystick), asCALL_CDECL);
	engine->RegisterGlobalFunction("array<uint32>@ get_joystick_list() property", asFUNCTION(GetJoystickListScript), asCALL_CDECL);

	engine->RegisterObjectType("joystick", sizeof(Joystick), asOBJ_REF);
	engine->RegisterObjectBehaviour("joystick", asBEHAVE_FACTORY, "joystick@ joy(uint32 id)", asFUNCTION(JoystickFactory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("joystick", asBEHAVE_ADDREF, "void f()", asMETHOD(Joystick, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("joystick", asBEHAVE_RELEASE, "void f()", asMETHOD(Joystick, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "bool get_valid() const property", asMETHOD(Joystick, IsValid), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "string get_name() const property", asMETHOD(Joystick, GetName), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "string get_path() const property", asMETHOD(Joystick, GetPath), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "string get_serial() const property", asMETHOD(Joystick, GetSerial), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "int get_num_axes() const property", asMETHOD(Joystick, GetNumAxes), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "int get_num_balls() const property", asMETHOD(Joystick, GetNumBalls), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "int get_num_hats() const property", asMETHOD(Joystick, GetNumHats), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "int get_num_buttons() const property", asMETHOD(Joystick, GetNumButtons), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "int16 get_axis(int axis) const", asMETHOD(Joystick, GetAxis), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "bool get_button(int button) const", asMETHOD(Joystick, GetButton), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "joysticktype get_type() const property", asMETHOD(Joystick, GetType), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "bool get_connected() const property", asMETHOD(Joystick, Connected), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "void set_events_enabled(bool enabled) property", asMETHOD(Joystick, SetEventsEnabled), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "bool get_events_enabled() const property", asMETHOD(Joystick, EventsEnabled), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "bool rumble(uint16 low_frequency_rumble, uint16 high_frequency_rumble, uint32 duration_ms) const",
		asMETHOD(Joystick, Rumble), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "bool rumble_triggers(uint16 left_rumble, uint16 right_rumble, uint32 duration_ms) const",
		asMETHOD(Joystick, RumbleTriggers), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "bool get_axis_initial_state(int axis, int16 &out state) const",
		asMETHOD(Joystick, GetAxisInitialState), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "bool get_ball(int ball, int &out dx, int &out dy) const",
		asMETHOD(Joystick, GetBall), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "uint8 get_hat(int hat) const",
		asMETHOD(Joystick, GetHat), asCALL_THISCALL);

	engine->RegisterObjectMethod("joystick", "bool set_led(uint8 red, uint8 green, uint8 blue) const",
		asMETHOD(Joystick, SetLED), asCALL_THISCALL);
	engine->RegisterObjectMethod("joystick", "int get_power_info(int &out percent) const",
		asMETHOD(Joystick, GetPowerInfo), asCALL_THISCALL);


}
