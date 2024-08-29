funcdef void dlg_callback();
void dlg(string&in message = "", uint64 time = 0, dlg_callback @cb = null, bool auto_close = false)
{
	if (get_window_handle() == 0)
		return;
	long prev_update_window_freq = update_window_freq;
	if (@cb == null)
		update_window_freq = 0;
	mixer_play_sound("dlg_open.ogg");
	bool played = false;
	timer t;
	timer tiks;
	speak(message);
	while (true)
	{
		wait(5);
		if (@cb != null)
			cb();
		if (time > 0)
		{
			if (tiks.elapsed_millis > 1000 && t.elapsed_millis < time)
			{
				mixer_play_sound("dlg_tik.ogg");
				tiks.restart();
			}
			if (t.elapsed_millis > time && !played)
			{
				played = true;
				mixer_play_sound("dlg_active.ogg");
				if (auto_close)
					break;
			}
		}
		if (key_pressed(SDLK_LEFT) || key_pressed(SDLK_RIGHT) || key_pressed(SDLK_UP) || key_pressed(SDLK_DOWN))
		{
			if (message != "")
				speak(message);
			mixer_play_sound("dlg_item.ogg");
		}
		if (key_pressed(SDLK_RETURN) || key_pressed(SDLK_ESCAPE))
		{
			if (time == 0)
				break;
			else
			{
				if (t.elapsed_millis > time)
					break;
			}
		}
	}
	update_window_freq = prev_update_window_freq;
	mixer_play_sound("dlg_close.ogg");
}
