#include "ngt.h"
#include "dlg.h"
sound test;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	sound_system_init();
	test.load(L"C:\\windows\\media\\tada.wav");
	test.play();
	alert(L"BGT alert", L"This func demo of the BGT alert");
	show_game_window(L"Game window test");
	wait(1000);//screen reader is speaking title
	dlg(L"Hello. This is dlg, ported dlg.bgt file to c++ on ngt. press enter to start.");
	speak(L"press down, up or all arrow keys. See! And press Alt+F4 or escape to exit.");
	wait(500);
	while (true) {
		update_game_window();
		if (key_down(VK_DOWN)) {
			speak(L"Down arrow is downed");
		}
		if (key_pressed(VK_RIGHT)) {
			speak(L"Right arrow is pressed");
		}
		if (key_released(VK_UP) or key_released(VK_LEFT)) {
			speak(L"Arrow is released");
		}
		if (key_down(VK_ESCAPE)) { return 0; }
	}
	return 0;
}