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
		title_spoken = false;
		wrapp = false;
		@on_click_callback = null;
	}

	void add_item(const string&in item_title, menu_item_callback @callback) {
		menu_item @item = menu_item(item_title, callback);
		items.insert_last(item);
	}

	void add_items(array<string> @items){
		for (uint i = 0; i < items.length(); ++i)
		{
			add_item(items[i], null);
		}
	}
	void add_submenu(const string&in item_title, menu @submenu) {
		menu_item @item = menu_item(item_title, submenu);
		items.insert_last(item);
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
																  // Show the submenu
			}
			else if (items[selected_index].callback !is null)
			{
				items[selected_index].callback(); // Call the callback function
			}
		}
	}

	void monitor() {
		if (!title_spoken)
		{
			screen_reader::speak(title, false);
			speak_item(false);
			title_spoken = true;
		}
		if (key_repeat(KEY_DOWN))
		{
			this.navigate(1);
		}
		else if (key_repeat(KEY_UP)){
			this.navigate(-1);
		}
		else if (key_repeat(KEY_RETURN) || key_repeat(KEY_SPACE))this.select();
	}

	protected void speak_item(bool interrupt = true)
	{
		screen_reader::speak(
			items[selected_index].title +
				(items[selected_index].submenu !is null ? " Submenu" : ""),
			interrupt);
	}
	int get_selected_item() const property{
		return selected_index;
	}
};
