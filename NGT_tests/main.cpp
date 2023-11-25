#include "ngt.h"
#include "dlg.h"
key_hold spacebar(SDLK_SPACE, 300, 20);
sound test;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	init_engine();
	test.load("asset/NGTTest.wav");
	show_game_window("Sound player");
	wait(1000);//screen reader is speaking title
	dlg(L"Press up and down to change music volume. Press Right or Left to change music pan. Press PageDown or PageUp to change music pitch. Press Space to pause or play music. Press Enter to start.");
	test.play_looped();
	delay(500);
	while (true) {
		delay(10);

		update_game_window();
		if (key_down(SDLK_DOWN)) {
			float musvol = test.get_volume();
			test.set_volume(musvol-=0.01);
		}
		if (key_down(SDLK_UP)) {
			float musvol = test.get_volume();
			test.set_volume(musvol+=0.01);
		}
		if (key_down(SDLK_RIGHT)) {
			float muspan = test.get_pan();
			test.set_pan(muspan+=0.01);
		}
		if (key_down(SDLK_LEFT)) {
			float muspan = test.get_pan();
			test.set_pan(muspan-=0.01);
		}
		if (key_down(SDLK_PAGEUP)) {
			int muspitch = test.get_pitch();
			test.set_pitch(muspitch -= 20);
		}
		if (key_down(SDLK_PAGEDOWN)) {
			int muspitch = test.get_pitch();
			test.set_pitch(muspitch += 20);
		}

		if (spacebar.pressing()) {
			if (!test.is_paused())test.pause();
			else BASS_ChannelPlay(test.handle_, false);
}
		if (key_down(SDLK_ESCAPE)) { return 0; }
	}
	return 0;
}