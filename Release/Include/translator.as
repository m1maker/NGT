/*
Translation class.
This class is useful for integrating translation into your game.
The lang folder must be created.
By default, a folder named 'lang' is used. However, you can change it by setting the property 'lfolder'.
The language extension is .lng, but you can change it with the property 'lex'.
*/
class translator
{
	// The languages folder.
	string lfolder = "lang";
	string base_lang = "English";
	string base_lang_flag = "🇺🇸";
	string base_lang_code = "Eng";

	// The language extension.
	string lex = ".lng";

	private string langdata, m_langname = this.base_lang;

	string get_langname() property
	{
		return this.m_langname;
	}

	void refresh(string&in l, string&in folder = "", string&in ex = "")
	{
		if (folder == "")
			folder = this.lfolder;
		if (ex == "")
			ex = this.lex;
		if (l == "")
			l = this.m_langname;
		if (l == "")
			l = this.base_lang;

		this.lfolder = folder;
		this.lex = ex;
		this.langdata = "";

		if (l != this.base_lang)
		{
			file f;
			if (f.open(this.lfolder + "/" + l + this.lex, "r") == 0)
			{
				this.langdata = f.read(f.get_size());
				f.close();
			}
		}
		this.m_langname = l;
	}

	string translate(const string&in l, string&in text, const string&in firstr = "", const string&in secondr = "")
	{
		if (l != this.m_langname)
			this.refresh(l);
		string c = this.langdata;
		if (c == "")
			c = text;

		string[] lines = c.split("\r\n");
		for (uint i = 0; i < lines.length(); i++)
		{
			string[] t = lines[i].split("=");
			if (t.length() < 2)
				continue;
			text = text.replace(t[0], t[1]);
		}

		if (firstr != "" && secondr != "")
		{
			string[] fir = firstr.split(" ");
			string[] sec = secondr.split("```=```");
			if (fir.length() == sec.length())
			{
				for (uint b = 0; b < fir.length(); b++)
				{
					text = text.replace(fir[b], sec[b]);
				}
			}
		}
		return text;
	}

	string get_flag(string&in l = "")
	{
		if (l == "")
			l = this.m_langname;
		if (l == "")
			l = this.base_lang;
		if (l == this.base_lang)
			return this.base_lang_flag;

		string[] lines = this.load_language_file(l);
		for (uint i = 0; i < lines.length(); i++)
		{
			string[] d = lines[i].split(":");
			if (d[0] == "flag" && d.length() > 1)
				return d[1];
		}
		return "";
	}

	string get_flag() property
	{
		return this.get_flag(this.m_langname);
	}

	string get_code(string&in l = "")
	{
		if (l == "")
			l = this.m_langname;
		if (l == "")
			l = this.base_lang;
		if (l == this.base_lang)
			return this.base_lang_code;

		string[] lines = this.load_language_file(l);
		for (uint i = 0; i < lines.length(); i++)
		{
			string[] d = lines[i].split(":");
			if (d[0] == "code" && d.length() > 1)
				return d[1];
		}
		return "";
	}

	string get_code() property
	{
		return this.get_code(this.m_langname);
	}

	private string[] load_language_file(const string l)
	{
		file f;
		if (f.open(this.lfolder + "/" + l + this.lex, "r") == 0)
		{
			string[] lines = f.read(f.get_size()).split("\r\n");
			f.close();
			return lines;
		}
		return {};
	}
};
// end class
