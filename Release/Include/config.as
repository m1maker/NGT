/*savedata class.
This class provides saving and loading data, probably used on games to save the progress.
This class supports multiple save types.
By default, the SAVEDATA_DEF type is used.
It is however not advised to use INI for saving many data, for example in game messages, because INI is used new line and = seperater.
*/
enum SaveDataTypes
{
	SAVEDATA_DEF = 0,
	SAVEDATA_INI
};

class save_data
{
	private string key;
	private string filename;
	private dictionary dataDictionary;
	private int saveType = SAVEDATA_DEF;

	// Constructor
	save_data(string filename, string encryptionKey = "", int type = SAVEDATA_DEF)
	{
		this.filename = filename;
		this.key = encryptionKey;
		this.saveType = type;
	}

	int get_type() property
	{
		return this.saveType;
	}

	// Load saved data into the dictionary
	void load()
	{
		file f;
		if (f.open(filename, "r") == 0)
		{
			dataDictionary = loadDictionary(f.read(f.get_size()));
			f.close();
		}
	}

	// Save data into the file
	void save()
	{
		file f;
		f.open(filename, "w");
		f.write(saveDictionary(dataDictionary));
		f.close();
	}

	// Add a numeric value
	void add(string name, double value)
	{
		dataDictionary.set(name, value);
	}

	// Read a numeric value
	double readNumber(string name)
	{
		if (this.saveType == SAVEDATA_INI)
			return string_to_number(this.read(name));
		
		double temp;
		return dataDictionary.get(name, temp) ? temp : -1;
	}

	// Overloaded function to add a string
	void add(string name, string value)
	{
		dataDictionary.set(name, value);
	}

	// Read a string
	string read(string name)
	{
		string temp;
		dataDictionary.get(name, temp);
		return temp;
	}

	string[] get_keys() property
	{
		return this.dataDictionary.get_keys();
	}

	bool exists(string key)
	{
		return this.dataDictionary.exists(key);
	}

	void clear()
	{
		this.dataDictionary.delete_all();
	}

	private dictionary loadDictionary(string data)
	{
		if (this.key != "")
			data = string_decrypt(data, this.key);
		
		if (this.saveType == SAVEDATA_INI)
			return parseIniData(data);
		
		return deserialize(data);
	}

	private string saveDictionary(dictionary @d)
	{
		string finalData;
		if (this.saveType == SAVEDATA_INI)
			finalData = convertToIniFormat(d);
		else
			finalData = serialize(d);
		
		if (this.key != "")
			finalData = string_encrypt(finalData, this.key);
		
		return finalData;
	}

	private dictionary parseIniData(string data)
	{
		dictionary d;
		string[] lines = data.split("\r\n");
		for (uint i = 0; i < lines.length(); i++)
		{
			int separatorIndex = lines[i].find_first("=");
			if (separatorIndex < 0)
				continue;

			string key = lines[i].substr(0, separatorIndex);
			string value = lines[i].substr(separatorIndex + 1);
			if (key != "" && value != "")
				d.set(key, value);
		}
		return d;
	}

	private string convertToIniFormat(dictionary @d)
	{
		string[] keys = this.get_keys();
		if (keys.length() < 1)
			return "";

		string finalString = "";
		for (uint i = 0; i < keys.length(); i++)
		{
			string value;
			double numberValue;
			if (d.get(keys[i], value))
				finalString += keys[i] + "=" + value;
			else if (d.get(keys[i], numberValue))
				finalString += keys[i] + "=" + numberValue;

			if (i < keys.length() - 1)
				finalString += "\r\n";
		}
		return finalString;
	}
};
