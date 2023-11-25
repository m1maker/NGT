#include "dlg.h"
#include<SDL.h>
void dlg(std::wstring message){
	speak(message, false);
	while(true){
		update_game_window();
		if(key_pressed(SDLK_DOWN) or key_pressed(SDLK_UP) or key_pressed(SDLK_LEFT) or key_pressed(SDLK_RIGHT)){
			speak(message);
		}
		if(key_down(SDLK_ESCAPE) or key_down(SDLK_RETURN)){
			break;
		}
	}
}
