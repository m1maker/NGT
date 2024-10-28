#include "hip"
funcdef void menu_item_callback();
funcdef void menu_on_click_callback();

class menu_item
{
	string title;
	menu @submenu; // Pointer to a submenu
	menu_item_callback @callback;

	menu_item(const string&in title, menu_item_callback @cb)
	{
		this.title = title;
		@ this.callback = cb;
		@ this.submenu = null; // Initialize with no submenu
	}

	// Constructor for submenu items
	menu_item(const string&in title, menu @submenu)
	{
		this.title = title;
		@ this.submenu = submenu;
		@ this.callback = null;
	}
};

class menu
{
	private array<menu_item @> items;
	string title;
	protected int selected_index;
	bool is_submenu; // Flag to indicate if this is a submenu
	protected bool title_spoken = false;
	bool wrapp = false;
	menu_on_click_callback @on_click_callback = null;
	bool in_submenu = false;
	bool speak_index = false;
	bool enable_tab_order = true;

	menu(const string&in title, bool is_submenu = false)
	{
		this.title = title;
		this.is_submenu = is_submenu;
	}

	menu()
	{
		reset();
	}

	void reset() {
		items.resize(0);
		title = "";
		selected_index = 0;
		is_submenu = false;
		in_submenu = false;
		title_spoken = false;
		wrapp = false;
		@on_click_callback = null;
		speak_index = false;
		enable_tab_order = true;
	}

	int add_item(const string&in item_title, menu_item_callback @callback) {
		menu_item @item = menu_item(item_title, callback);
		items.insert_last(item);
		return items.length() - 1;
	}

	void add_items(array<string> @items){
		for (uint i = 0; i < items.length(); ++i)
		{
			add_item(items[i], null);
		}
	}

	int add_submenu(const string&in item_title, menu @submenu) {
		menu_item @item = menu_item(item_title, submenu);
		items.insert_last(item);
		return items.length() - 1;
	}

	void navigate(int direction) {
		selected_index += direction;
		if (selected_index < 0)
		{
			selected_index = !wrapp ? 0 : items.length() - 1; // Prevent going above the first item
		}
		else if (selected_index >= items.length())
		{
			selected_index = !wrapp ? items.length() - 1 : 0; // Prevent going below the last item
		}
		speak_item();
	}

	void select() {
		if (selected_index >= 0 && selected_index < items.length())
		{
			if (on_click_callback !is null)
				on_click_callback();
			if (items[selected_index].submenu !is null)
			{
				items[selected_index].submenu.selected_index = 0; // Reset the submenu selection
				items[selected_index].submenu.title_spoken = false;
				items[selected_index].submenu.in_submenu = true;
			}
			else if (items[selected_index].callback !is null)
			{
				items[selected_index].callback(); // Call the callback function
			}
		}
	}

	void monitor() {
		if(items.is_empty())return;
		if (items[selected_index].submenu !is null && items[selected_index].submenu.in_submenu)
		{
			items[selected_index].submenu.speak_index = this.speak_index;
			items[selected_index].submenu.monitor();
			if (title_spoken)
				this.title_spoken = false;
			return;
		}
		if (!title_spoken)
		{
			screen_reader::speak(title, false);
			speak_item(false);
			title_spoken = true;
		}
		if (key_repeat(KEY_DOWN) || (enable_tab_order && !shift_down && key_repeat(KEY_TAB)))
		{
			this.navigate(1);
		}
		else if (key_repeat(KEY_UP) || (enable_tab_order && shift_down && key_repeat(KEY_TAB))){
			this.navigate(-1);
		}
		else if (key_pressed(KEY_HOME)){
			selected_index = 0;
			speak_item();
		}
		else if (key_pressed(KEY_END)){
			selected_index = items.length() - 1;
			speak_item();
		}

		else if (key_repeat(KEY_RETURN) || key_repeat(KEY_SPACE) || (key_repeat(KEY_RIGHT) && items[selected_index].submenu !is null))this.select();
		else if (this.in_submenu && (key_repeat(KEY_ESCAPE) || key_repeat(KEY_BACK) || key_repeat(KEY_LEFT))){
			this.in_submenu = false;
		}
	}

	protected void speak_item(bool interrupt = true)
	{
		// Start with the title of the selected item
		string announcement = items[selected_index].title;

		// Check if the item has a submenu
		if (items[selected_index].submenu != null)
		{
			announcement += " Submenu"; // Append " Submenu" if it has a submenu
		}
		// If speak_index is true, append the index information
		if (this.speak_index)
		{
			announcement += " " + (selected_index + 1) + " of " + items.length();
		}

		// Call the screen reader to speak the announcement
		screen_reader::speak(announcement, interrupt);
	}

	int get_selected_item() const property{
		return selected_index;
	}

	int get_item_count() const property{
		return items.length();
	}
};
