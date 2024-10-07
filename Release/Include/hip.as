/*
Helpful Includes pack (HIP),. by: Harry Min Khant!

Description:
This pack contain helpful things and shorten words during making your game, with the NGT language If you are making on. If you are making your games on the focus of NGT, and If you want some functions to be quicker or shorten? This includes pack will enough for you! These functions have made easier and shorten to be used in the somewhere you want to use. Every function has it's own descriptions, and some functions can have an example. harry min khant has published this pack for those who want to be shorten words, or for those who want to use the easyest functions. although this pack contain lots of functions, however some of things are left to be added. though the tasks still left to be added, you can now do common tasks as per your requirement with the below functions. easy, shorten, freely and useful for all of you!
I hope you enjoy this pack, and make sure to report any bugs or any rong things If you found any.
If you think that the function should be added or If there's something to change, please contact!
email:
harrymk64@gmail.com
telegram:
@harrymktbot
*/
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
/*string get_modified_time(double y,double m,double d,double h,double mn,double s)
{
if(y<1 and m<1 and d<1 and mn<1 and s<1) return"just now";
string final;
datetime now;
datetime past;
past.set(y,m,d,h,mn,s);
double year=past.diff_years(now);
year=round(year,0);
double month=past.diff_months(now);
double day=past.diff_days(now);
double hour=past.diff_hours(now);
double minute=past.diff_minutes(now);
double second=past.diff_seconds(now);
second=round(second,0);
minute=round(minute,0);
hour=round(hour,0);
month=round(month,0);
day=round(day,0);
if(year>0) final=year+" year"+(year>1?"s":"")+" ago";
else if(month>0) final=month+" month"+(month>1?"s":"")+" ago";
else if(day>0) final=day+" day"+(day>1?"s":"")+" ago";
else if(hour>0) final=hour+" hour"+(hour>1?"s":"")+" ago";
else if(minute>0) final=minute+" minute"+(minute>1?"s":"")+" ago";
else if(second>0) final=second+" second"+(second>1?"s":"")+" ago";
return final;
}*/
class lister
{
	// list to string convertion.
	string convert(string[] arr, string each = ", ")
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
	string convert(int[] arr, string each = ", ")
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
	string convert(double[] arr, string each = ", ")
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
	string convert(float[] arr, string each = ", ")
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
	string convert(uint[] arr, string each = ", ")
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
string convert_month_name(double value)
{
	if (value == 1)
		return"January";
	else if (value == 2) return"Febuary";
	else if (value == 3) return"March";
	else if (value == 4) return"April";
	else if (value == 5) return"May";
	else if (value == 6) return"June";
	else if (value == 7) return"July";
	else if (value == 8) return"August";
	else if (value == 9) return"September";
	else if (value == 10) return"October";
	else if (value == 11) return"November";
	else if (value == 12) return"December";
	else
		return"";
}
double convert_month_number(string monthname)
{
	if (monthname == "January")
		return 1;
	else if (monthname == "Febuary") return 2;
	else if (monthname == "March") return 3;
	else if (monthname == "April") return 4;
	else if (monthname == "May") return 5;
	else if (monthname == "June") return 6;
	else if (monthname == "July") return 7;
	else if (monthname == "August") return 8;
	else if (monthname == "September") return 9;
	else if (monthname == "October") return 10;
	else if (monthname == "November") return 11;
	else if (monthname == "December") return 12;
	else
		return -1;
}
string convert_weekday_name(double value)
{
	if (value == 1)
		return"Sunday";
	else if (value == 2) return"Monday";
	else if (value == 3) return"Tuesday";
	else if (value == 4) return"Wednesday";
	else if (value == 5) return"Thursday";
	else if (value == 6) return"Friday";
	else if (value == 7) return"Saturday";
	else
		return"";
}
double convert_weekday_number(string weekdayname)
{
	if (weekdayname == "Sunday")
		return 1;
	else if (weekdayname == "Monday") return 2;
	else if (weekdayname == "Tuesday") return 3;
	else if (weekdayname == "Wednesday") return 4;
	else if (weekdayname == "Thursday") return 5;
	else if (weekdayname == "Friday") return 6;
	else if (weekdayname == "Saturday") return 7;
	else
		return -1;
}
/*
tone_synth progress_synth;
sound@ progress_sound;
void beep_percentage(double val)
{
val=round(val,0);
if(val<0) val=0;
if(val>100) val=100;
double freq=110*2**(val/25);
progress_synth.waveform_type=4;
progress_synth.set_edge_fades(1,1);
progress_synth.freq_ms(""+freq,40);
@progress_sound=progress_synth.write_wave_sound();
progress_sound.volume=-10;
progress_synth.reset();
progress_sound.play();
}*/
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
string get_item_text(string text, string split_with = "[[]]")
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
string get_item_name(string text, string split_with = "[[]]")
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
bool file_put_contents(string filename, string contents, uint8 filemode = FILE_WRITE)
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
string file_get_contents(string filename)
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
/*string get_time_in_format(string format="<dd>/<mm>/<y>, <hh>:<nn>:<ss> <TT>",double a=DATE_YEAR,double b=DATE_MONTH,double c=DATE_DAY,double d=TIME_HOUR,double e=TIME_MINUTE,double f=TIME_SECOND)
{
string result=format;
datetime cl;
cl.setDate(a,b,c);
cl.setTime(d,e,f);
string date, month, year, hour12, hour12x0, hour24, minute, second, daytime_lc, daytime;
date=c;
if(c<10)
{
date="0"+date;
}
month=""+b;
if(b<10)
{
month="0"+month;
}
year=""+a;
year=string_trim_left(year, 2);
hour24=""+d;
if(d<10)
{
hour24="0"+hour24;
}
if(d>12)
{
hour12=""+(d-12);
hour12x0=""+(d-12);
daytime="PM";
daytime_lc="pm";
}
if(d==0)
{
hour12="12";
hour12x0="12";
daytime="AM";
daytime_lc="am";
}
if(d<=12)
{
if(d>0)
{
hour12=""+d;
hour12x0=""+d;
daytime="AM";
daytime_lc="am";
}
}
if(d<10)
{
if(d>0)
{
hour12="0"+hour12;
}
}
if(d<22)
{
if(d>12)
{
hour12="0"+hour12;
}
}
minute=e;
if(e<10)
{
minute="0"+minute;
}
second=f;
if(f<10)
{
second="0"+second;
}
if(result!="")
{
result=result.replace("<d>", ""+c+"");
result=result.replace("<D>", ""+c+"");
result=result.replace("<dd>", date);
result=result.replace("<DD>", date);
result=result.replace("<w>", ""+cl.weekday);
result=result.replace("<W>", cl.weekday_name);
result=result.replace("<m>", ""+b);
result=result.replace("<M>", cl.month_name);
result=result.replace("<mm>", month);
result=result.replace("<y>", ""+a);
result=result.replace("<Y>", year);
result=result.replace("<h>", hour12x0);
result=result.replace("<hh>", hour12);
result=result.replace("<H>", ""+d);
result=result.replace("<HH>", hour24);
result=result.replace("<n>", ""+e);
result=result.replace("<N>", ""+e);
result=result.replace("<nn>", minute);
result=result.replace("<NN>", minute);
result=result.replace("<s>", ""+f);
result=result.replace("<S>", ""+f);
result=result.replace("<ss>", second);
result=result.replace("<SS>", second);
result=result.replace("<tt>", daytime_lc);
result=result.replace("<TT>", daytime);
}
return result;
}*/
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
string stringleft(string str, uint count)
{
	return str.substr(0, count);
}
string stringright(string str, uint count)
{
	if (count < 1)
		return"";
	return str.substr(str.length() - count);
}
string string_trimleft(string str, uint count)
{
	if (count > str.length())
		return str;
	if (count < 1)
		return"";
	return str.substr(count);
}
string string_trimright(string str, uint count)
{
	if (count > str.length())
		return str;
	if (count < 1)
		return"";
	count = str.length() - count;
	return str.substr(0, count);
}
string var_replace(string text, string[] replacers = {}, string opening = "%", string closing = "%")
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
string var_replace2(string text, string[] fir = {}, string[] sec = {})
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
bool inrange(double x, double y, double z, double minx, double maxx, double miny, double maxy, double minz, double maxz)
{
	if (x >= minx && x <= maxx && y >= miny && y <= maxy && z >= minz && z <= maxz)
		return true;
	return false;
}
bool inrange(double x, double y, double z, vector @min, vector @max)
{
	try
	{
		return inrange(x, y, z, min.x, max.x, min.y, max.y, min.z, max.z);
	}
	catch
	{
		return false;
	}
}
bool inrange(vector @current, vector @min, vector @max)
{
	try
	{
		return inrange(current.x, current.y, current.z, min.x, max.x, min.y, max.y, min.z, max.z);
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
} string get_gender(string gend, int n = 0)
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