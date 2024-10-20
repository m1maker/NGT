enum tiletype
{
	WALKABLE,
	BLOCKED,
	WATER,
	GRASS,
	AIR, // Represents empty space
	WALL
}

class map
{
	private int width;
	private int height;
	private int depth;
	private array<tiletype> voxels;
	private array<string> tileNames; // Array to store tile names

	// Default constructor (empty)
	map()
	{
		width = 0;
		height = 0;
		depth = 0;
		voxels.resize(0); // No voxels
		tileNames.resize(0); // No tile names
	}

	// Parameterized constructor
	map(int w, int h, int d)
	{
		width = w;
		height = h;
		depth = d;
		voxels.resize(w * h * d);
		tileNames.resize(w * h * d); // Initialize tile names
	}

	// Copy constructor
	map(const map&in other)
	{
		width = other.width;
		height = other.height;
		depth = other.depth;
		voxels = other.voxels; // Use the array's built-in copy mechanism
		tileNames = other.tileNames; // Copy tile names
	}

	// Assignment operator
	map& opAssign(map&in other)
	{
		if (this is other)
		{
			return this; // Handle self-assignment
		}

		width = other.width;
		height = other.height;
		depth = other.depth;
		voxels = other.voxels; // Use the array's built-in copy mechanism
		tileNames = other.tileNames; // Copy tile names
		return this;
	}

	// Resize method
	void resize(int newWidth, int newHeight, int newDepth) {
		width = newWidth;
		height = newHeight;
		depth = newDepth;
		voxels.resize(newWidth * newHeight * newDepth); // Default all new voxels to AIR
		tileNames.resize(newWidth * newHeight * newDepth); // Resize tile names
	}

	// Set the type of a specific voxel
	void set_tile(int x, int y, int z, tiletype type) {
		if (is_in_bounds(x, y, z))
		{
			voxels[z * (width * height) + y * width + x] = type;
		}
	}

	// Set the name of a specific tile
	void set_tile_name(int x, int y, int z, const string&in name) {
		if (is_in_bounds(x, y, z))
		{
			tileNames[z * (width * height) + y * width + x] = name;
		}
	}

	// Get the type of a specific voxel
	tiletype get_tile(int x, int y, int z) {
		if (is_in_bounds(x, y, z))
		{
			return voxels[z * (width * height) + y * width + x];
		}
		return tiletype::BLOCKED; // Return BLOCKED if out of bounds
	}

	// Get the name of a specific tile
	string get_tile_name(int x, int y, int z) {
		if (is_in_bounds(x, y, z))
		{
			return tileNames[z * (width * height) + y * width + x];
		}
		return ""; // Return empty string if out of bounds
	}

	// Check if coordinates are within the map bounds
	bool is_in_bounds(int x, int y, int z) {
		return (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth);
	}

	// Get the dimensions of the map
	void get_dimensions(int&out w, int&out h, int&out d) {
		w = width;
		h = height;
		d = depth;
	}

	// Print the map for debugging (prints slices)
	void print() {
		for (int z = 0; z < depth; z++)
		{
			printf("Layer " + z + ":\n");
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					switch (get_tile(x, y, z))
					{
						case tiletype::WALKABLE:
							printf(".");
							break;
						case tiletype::BLOCKED:
							printf("#");
							break;
						case tiletype::WATER:
							printf("~");
							break;
						case tiletype::GRASS:
							printf("^");
							break;
						case tiletype::AIR:
							printf(" ");
							break;
						case tiletype::WALL:
							printf("|"); // Represent wall with '|'
							break;
					}
				}
				printf("\n");
			}
			printf("\n");
		}
	}

	// Method to create a wall between two points
	void add_wall(int startX, int startY, int startZ, int endX, int endY, int endZ) {
		for (int x = startX; x <= endX; x++)
		{
			for (int y = startY; y <= endY; y++)
			{
				for (int z = startZ; z <= endZ; z++)
				{
					set_tile(x, y, z, tiletype::WALL);
				}
			}
		}
	}

	// Method to serialize the map into a string
	string serialize() {
		string result = width + "," + height + "," + depth + ";"; // First, add dimensions

		for (int z = 0; z < depth; z++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					result += int(get_tile(x, y, z)) + ",";		// Add tile type
					result += get_tile_name(x, y, z) + ","; // Add tile name
				}
				result = result.substr(0, result.length() - 1); // Remove the last comma
				result += ";";									// Separate layers
			}
		}

		return result;
	}

	// Method to deserialize the map from a string
	bool deserialize(const string&in data) {
		array<string> layers = data.split(";"); // Split by layers
		if (layers.length() < 1)
			return false;

		array<string> dimensions = layers[0].split(","); // Get dimensions
		if (dimensions.length() != 3)
			return false;

		width = string_to_number(dimensions[0]);
		height = string_to_number(dimensions[1]);
		depth = string_to_number(dimensions[2]);
		voxels.resize(width * height * depth);
		tileNames.resize(width * height * depth); // Resize tile names

		for (int z = 0; z < depth; z++)
		{
			if (z + 1 >= layers.length())
				return false;								// Check for layer existence

			array<string> tiles = layers[z + 1].split(","); // Get tiles for the current layer
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int index = y * width + x;
					if (index < tiles.length())
					{
						voxels[z * (width * height) + index] = tiletype(string_to_number(tiles[index])); // Set tile
						tileNames[z * (width * height) + index] = tiles[index]; // Set tile name
					}
				}
			}
		}

		return true;
	}
};
