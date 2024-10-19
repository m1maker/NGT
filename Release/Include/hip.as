/*
Helpful Includes pack (HIP),. by: Harry Min Khant!

Description:
This pack contain helpful things and shorten words during making your game, with the NGT engine. If you are making on. If you are making your games on the focus of NGT, and If you want some functions to be quicker or shorten? This includes pack will enough for you! These functions have made easier and shorten to be used in the somewhere you want to use. Every function has it's own descriptions, and some functions can have an example. harry min khant has published this pack for those who want to be shorten words, or for those who want to use the easyest functions. although this pack contain lots of functions, however some of things are left to be added. though the tasks still left to be added, you can now do common tasks as per your requirement with the below functions. easy, shorten, freely and useful for all of you!
I hope you enjoy this pack, and make sure to report any bugs or any rong things If you found any.
If you think that the function should be added or If there's something to change, please contact!
email:
harrymk64@gmail.com
telegram:
@harrymktbot

Edited by M_maker || M1 Maker.
Code refactoring and some improvements.

*/
#if HIP_NAMESPACE
namespace hip
{
#endif
	bool control_is_down()
	{
		return key_down(KEY_LCTRL) || key_down(KEY_RCTRL);
	}
	bool alt_is_down()
	{
		return key_down(KEY_LALT) || key_down(KEY_RALT);
	}
	bool shift_is_down()
	{
		return key_down(KEY_LSHIFT) || key_down(KEY_RSHIFT);
	}
	bool get_control_down() property
	{
		return control_is_down();
	}
	bool get_shift_down() property
	{
		return shift_is_down();
	}
	bool get_alt_down() property
	{
		return alt_is_down();
	}
	double rdm(double minimum, double maximum, bool rounded = false, double number_of_rounding = 0)
	{
		double final = -1;
		if (rounded)
			final = round(random(minimum, maximum), number_of_rounding);
		else
			final = random(minimum, maximum);
		return final;
	}

	float clamp(float value, float min, float max) {
		return value < min ? min : (value > max ? max : value);
	}

	class lister
	{
		// list to string convertion.
		string convert(string[] arr, const string&in each = ", ")
		{
			string list;
			if (arr.length() == 0)
				return "";
			if (arr.length() == 1)
				return "" + arr[0];
			for (uint i = 0; i < arr.length(); i++)
			{
				if (i == arr.length() - 1)
					list += "and " + arr[i];
				else
					list += arr[i] + "" + each;
			}
			return list;
		}
		string convert(int[] arr, const string&in each = ", ")
		{
			string list;
			if (arr.length() == 0)
				return "";
			if (arr.length() == 1)
				return "" + arr[0];
			for (uint i = 0; i < arr.length(); i++)
			{
				if (i == arr.length() - 1)
					list += "and " + arr[i];
				else
					list += arr[i] + "" + each;
			}
			return list;
		}
		string convert(double[] arr, const string&in each = ", ")
		{
			string list;
			if (arr.length() == 0)
				return "";
			if (arr.length() == 1)
				return "" + arr[0];
			for (uint i = 0; i < arr.length(); i++)
			{
				if (i == arr.length() - 1)
					list += "and " + arr[i];
				else
					list += arr[i] + "" + each;
			}
			return list;
		}
		string convert(float[] arr, const string&in each = ", ")
		{
			string list;
			if (arr.length() == 0)
				return "";
			if (arr.length() == 1)
				return "" + arr[0];
			for (uint i = 0; i < arr.length(); i++)
			{
				if (i == arr.length() - 1)
					list += "and " + arr[i];
				else
					list += arr[i] + "" + each;
			}
			return list;
		}
		string convert(uint[] arr, const string&in each = ", ")
		{
			string list;
			if (arr.length() == 0)
				return "";
			if (arr.length() == 1)
				return "" + arr[0];
			for (uint i = 0; i < arr.length(); i++)
			{
				if (i == arr.length() - 1)
					list += "and " + arr[i];
				else
					list += arr[i] + "" + each;
			}
			return list;
		}
	} // end of lister.

	string convert_size(double size, int round_to = 2)
	{
		if (size < 1)
			return"0 B";
		if (size < 1024)
		{
			return round(size, round_to) + " B";
		}
		size = size / 1024;
		if (size < 1024)
		{
			return round(size, round_to) + " KB";
		}
		size = size / 1024;
		if (size < 1024)
		{
			return round(size, round_to) + " MB";
		}
		size = size / 1024;
		if (size < 1024)
		{
			return round(size, round_to) + " GB";
		}
		size = size / 1024;
		return round(size, round_to) + " TB";
	}
	const string[] MONTH_NAMES = {
		"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	const string[] WEEKDAY_NAMES = {
		"", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

	string convert_month_name(double value) {
		if (value >= 1 && value <= 12)
		{
			return MONTH_NAMES[int(value)];
		}
		return "";
	}

	double convert_month_number(const string&in monthname) {
		for (uint i = 0; i < MONTH_NAMES.length(); i++)
		{
			if (monthname == MONTH_NAMES[i])
			{
				return i + 1;
			}
		}
		return -1;
	}

	string convert_weekday_name(double value) {
		if (value >= 1 && value <= 7)
		{
			return WEEKDAY_NAMES[int(value)];
		}
		return "";
	}

	double convert_weekday_number(const string&in weekdayname) {
		for (uint i = 0; i < WEEKDAY_NAMES.length(); i++)
		{
			if (weekdayname == WEEKDAY_NAMES[i])
			{
				return i + 1;
			}
		}
		return -1;
	}
	double get5rating(double five, double four, double three, double two, double one, int round_to = 1)
	{
		try
		{
			double first = five + four + three + two + one;
			double second = (five * 5) + (four * 4) + (three * 3) + (two * 2) + (one * 1);
			if (first < 1)
				return 0.0;
			double result = second / first;
			result = round(result, round_to);
			return result;
		}
		catch
		{
			return round(0, round_to);
		}
	}
	string get_item_text(const string&in text, const string&in split_with = "[[]]")
	{
		string final;
		string[] ma = text.split(split_with);
		if (ma.length() > 1)
		{
			final = ma[1];
		}
		else
			final = ma[0];
		return final;
	}
	string get_item_name(const string&in text, const string&in split_with = "[[]]")
	{
		string final;
		string[] ma = text.split(split_with);
		final = ma[0];
		return final;
	}
	double percent(double n1, double n2)
	{
		try
		{
			return (n1 / n2) * 100;
		}
		catch
		{
			return 0;
		}
	}
	// these are the constants for the file_put_contents function.
	const uint8 FILE_WRITE = 250;
	const uint8 FILE_APPEND = 255;
	bool file_put_contents(const string&in filename, const string&in contents, uint8 filemode = FILE_WRITE)
	{
		int ret;
		file tempfile;
		if (filemode == FILE_WRITE)
			ret = tempfile.open(filename, "w");
		else if (filemode == FILE_APPEND) ret = tempfile.open(filename, "a");
		else
			return false;
		if (ret == -1)
			return false;
		int ret2 = tempfile.write(contents);
		tempfile.close();
		if (ret2 == -1)
			ret = -1;
		else
			ret = 0;
		if (ret == -1)
			return false;
		return true;
	}
	string file_get_contents(const string&in filename)
	{
		file tempfile;
		tempfile.open(filename, "r");
		string r = tempfile.read(tempfile.get_size());
		tempfile.close();
		return r;
	}
	bool is_over_value(string[] pars, double currentc = 0)
	{
		if (currentc < 0 or currentc > pars.length() - 1)
			return true;
		else
			return false;
	}
	bool is_over_value(double len, double currentc = 0)
	{
		if (currentc < 0 or currentc > len - 1)
			return true;
		else
			return false;
	}

	bool toggle(bool b)
	{
		return b ? false : true;
	}
	bool int_to_bool(int b)
	{
		return b == 1 ? true : false;
	}
	int toggle(int b)
	{
		return b == 1 ? 0 : 1;
	}
	int bool_to_int(bool b)
	{
		return b ? 1 : 0;
	}
	string ms_to_readable_time(double ms, int round_year_to = 0, int round_month_to = 0, int round_week_to = 0, int round_day_to = 0, int round_hour_to = 0, int round_minute_to = 0, int round_second_to = 0)
	{
		if (ms <= 0)
			return "no time at all";
		if (ms < 1000)
			return ms + " millisecond" + (ms > 1 ? "s" : "");
		double days, hours, minutes, weeks, months, seconds, years;
		seconds = floor(ms / 1000);
		minutes = floor(seconds / 60);
		seconds %= floor(60);
		hours = floor(minutes / 60);
		minutes %= floor(60);
		days = floor(hours / 24);
		hours %= floor(24);
		days = round(days, round_day_to);
		weeks = floor(days / 7);
		days %= floor(7);
		months = floor(weeks / 4.35);
		weeks %= floor(4.35);
		years = floor(months / 12);
		months %= floor(12);
		months = round(months, round_month_to);
		weeks = round(weeks, round_week_to);
		minutes = round(minutes, round_minute_to);
		hours = round(hours, round_hour_to);
		seconds = round(seconds, round_second_to);
		years = round(years, round_year_to);
		string ret;
		if (years > 0)
		{
			string y = years > 1 ? "years" : "year";
			ret += years + " " + y + ", ";
		}
		if (months > 0)
		{
			string m = months > 1 ? "months" : "month";
			ret += months + " " + m + ", ";
		}
		if (weeks > 0)
		{
			string w = weeks > 1 ? "weeks" : "week";
			ret += weeks + " " + w + ", ";
		}
		if (days > 0)
		{
			string d = days > 1 ? "days" : "day";
			ret += days + " " + d;
		}
		if (days > 0 && (hours > 0 || minutes > 0 || seconds > 0))
			ret += ", ";
		if (hours > 0)
		{
			string h = hours > 1 ? "hours" : "hour";
			ret += hours + " " + h;
		}
		if (hours > 0 && (minutes > 0 || seconds > 0))
			ret += ", ";
		if (minutes > 0)
		{
			string m = minutes > 1 ? "minutes" : "minute";
			ret += minutes + " " + m;
		}
		if (minutes > 0 && seconds > 0)
			ret += ", ";
		if (seconds > 0)
		{
			string s = seconds > 1 ? "seconds" : "second";
			ret += seconds + " " + s;
		}
		string[] ret2 = ret.split(", ");
		if (ret2.length() == 1)
			return ret2[0];
		if (ret2.length() > 1)
		{
			ret = "";
			for (uint i = 0; i < ret2.length(); i++)
			{
				string h = ret2[i];
				if (i >= ret2.length() - 1)
					ret += "and ";
				ret += h;
				if (i < ret2.length() - 2)
					ret += ", ";
				else if (i < ret2.length() - 1) ret += " ";
			}
		} // length
		return ret;
	}
	/*
	string utilities
	*/
	string string_left(const string&in str, uint count)
	{
		return str.substr(0, count);
	}
	string string_right(const string&in str, uint count)
	{
		if (count < 1)
			return"";
		return str.substr(str.length() - count);
	}
	string string_trim_left(const string&in str, uint count)
	{
		if (count > str.length())
			return str;
		if (count < 1)
			return"";
		return str.substr(count);
	}
	string string_trim_right(const string&in str, uint count)
	{
		if (count > str.length())
			return str;
		if (count < 1)
			return"";
		count = str.length() - count;
		return str.substr(0, count);
	}
	string var_replace(string&out text, string[] replacers = {}, const string&in opening = "%", const string&in closing = "%")
	{
		if (replacers.length() < 1)
			return text;
		for (uint b = 0; b < replacers.length(); b++)
		{
			if (text.contains(opening + "" + (b + 1) + "" + closing))
				text = text.replace(opening + "" + (b + 1) + "" + closing, replacers[b]);
		}
		return text;
	}
	string var_replace2(string&out text, string[] fir = {}, string[] sec = {})
	{
		if (fir.length() < 1)
			return text;
		if (fir.length() != sec.length())
			return text;
		for (uint b = 0; b < fir.length(); b++)
		{
			if (text.contains(fir[b]))
				text = text.replace(fir[b], sec[b]);
		}
		return text;
	}
	bool in_range(double x, double y, double z, double minx, double maxx, double miny, double maxy, double minz, double maxz)
	{
		if (x >= minx && x <= maxx && y >= miny && y <= maxy && z >= minz && z <= maxz)
			return true;
		return false;
	}
	bool in_range(double x, double y, double z, vector @min, vector @max)
	{
		try
		{
			return in_range(x, y, z, min.x, max.x, min.y, max.y, min.z, max.z);
		}
		catch
		{
			return false;
		}
	}
	bool in_range(vector @current, vector @min, vector @max)
	{
		try
		{
			return in_range(current.x, current.y, current.z, min.x, max.x, min.y, max.y, min.z, max.z);
		}
		catch
		{
			return false;
		}
	}
	enum gendercodes
	{
		gender_s = 1,
		gender_o,
		gender_pd,
		gender_pp,
		gender_r
	};

	string get_gender(const string&in gend, int n = 0)
	{
		if (n == 0)
			return gend;
		string gen = gend.lower();
		if (gen == "male")
		{
			if (n == gender_s)
				return "he";
			else if (n == gender_o) return "him";
			else if (n == gender_pd) return "his";
			else if (n == gender_pp) return "his";
			else if (n == gender_r) return "himself";
			else
				return "";
		}
		else if (gen == "female")
		{
			if (n == gender_s)
				return "she";
			else if (n == gender_o) return "her";
			else if (n == gender_pd) return "her";
			else if (n == gender_pp) return "hers";
			else if (n == gender_r) return "herself";
			else
				return "";
		}
		else if (n == gender_s) return "they";
		else if (n == gender_o) return "them";
		else if (n == gender_pd) return "their";
		else if (n == gender_pp) return "theirs";
		else if (n == gender_r) return "themselves";
		else
			return "";
	}
#if HIP_NAMESPACE
}
#endif