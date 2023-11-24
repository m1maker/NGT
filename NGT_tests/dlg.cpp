#include "dlg.h"
void dlg(std::wstring message){
	speak(message, false);
	while(true){
		update_game_window();
		if(key_pressed(VK_DOWN) or key_pressed(VK_UP) or key_pressed(VK_LEFT) or key_pressed(VK_RIGHT)){
			speak(message);
		}
		if(key_down(VK_ESCAPE) or key_down(VK_RETURN)){
			break;
		}
	}
}
