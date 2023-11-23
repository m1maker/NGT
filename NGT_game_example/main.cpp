#include "ngt.h"
sound test;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	sound_system_init();
	test.load(L"C:\\Windows\\media\\tada.wav");
	test.play();
	alert(L"Hello", L"This is test message");
	exit(0);
}