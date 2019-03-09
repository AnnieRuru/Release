`*gettimestr` is probably another underused script command,  
because members always think `*gettime` can do the same, but actually its the opposite

it can simplify an hourly announcer script
```c
OnMinute00:
	.@hour = gettime(GETTIME_HOUR);
	if ( .@hour == 0 )
		.@hour = 12;
	else if ( .@hour > 12 )
		.@hour -= 12;
	.@minute = gettime(GETTIME_MINUTE);
	if ( .@minute < 10 )
		.@minute$ = "0"+ .@minute;
	else
		.@minute$ = .@minute;
	if ( .@hour < 12 )
		.@ampm$ = "am";
	else
		.@ampm$ = "pm";
	announce "The time now is "+ .@hour +":"+ .@minute$ + .@ampm$, bc_all;
	end;
```
into
```c
OnMinute00:
	announce "The time now is "+ gettimestr("%I:%M%p", 9), bc_all;
	end;
```
This is the script tested on Microsoft Visual Studio 2017
```c
prontera,156,192,4	script	Time Sample	8W_SOLDIER,{
	mes "System Tick : "+ gettimetick(0);
	mes " Time Tick  : "+ gettimetick(1);
	mes " UNIX Tick  : "+ gettimetick(2);
	mes " GetTime(1) : "+ gettime(GETTIME_SECOND) +" (Sec)"; // 15 (second 0~59)
	mes " GetTime(2) : "+ gettime(GETTIME_MINUTE) +" (Min)"; // 04 (minute 0~59)
	mes " GetTime(3) : "+ gettime(GETTIME_HOUR) +" (Hour)"; // 5 (hour 0~23)
	mes " GetTime(4) : "+ gettime(GETTIME_WEEKDAY) +" (WeekDay)"; // 2 (Sun=0,Sat=6 0~6)
	mes " GetTime(5) : "+ gettime(GETTIME_DAYOFMONTH) +" (MonthDay)"; // 22 (day 1~31)
	mes " GetTime(6) : "+ gettime(GETTIME_MONTH) +" (Month)"; // 12 (month 1~12)
	mes " GetTime(7) : "+ gettime(GETTIME_YEAR) +" (Year)"; // 2015 (year)
	mes " GetTime(8) : "+ gettime(GETTIME_DAYOFYEAR) +" (No.day in year)"; // 356 (day of the year)
	mes " GetTimeStr : %Y-%m/%d %H:%M:%S,40";
	mes gettimestr("%Y-%m/%d %H:%M:%S",40); // 2015-12/22 05:04:15
	next;
	mes "%a : "+ gettimestr("%a",19); // Tue (short week)
	mes "%A : "+ gettimestr("%A",19); // Tuesday (long week)
	mes "%b : "+ gettimestr("%b",19); // Dec (short month)
	mes "%B : "+ gettimestr("%B",19); // December (long month)
	mes "%c : "+ gettimestr("%c",19); // this is blank
	mes "%d : "+ gettimestr("%d",19); // 22 (day 01~31)
	mes "%H : "+ gettimestr("%H",19); // 05 (hour 00~23)
	mes "%I : "+ gettimestr("%I",19); // 05 (hour 01~12)
	mes "%j : "+ gettimestr("%j",19); // 356 (day of the year)
	mes "%m : "+ gettimestr("%m",19); // 12 (month 01~12)
	mes "%M : "+ gettimestr("%M",19); // 04 (minute 00~59)
	mes "%p : "+ gettimestr("%p",19); // AM (AM / PM)
	mes "%S : "+ gettimestr("%S",19); // 15 (second 00~59)
	mes "%U : "+ gettimestr("%U",19); // 51 (Week number with the first Sunday as the first day of week one (00-53))
	mes "%w : "+ gettimestr("%w",19); // 2 (Sun=0,Sat=6 0~6)
	mes "%W : "+ gettimestr("%W",19); // 51 (Week number with the first Monday as the first day of week one (00-53))
	mes "%x : "+ gettimestr("%x",19); // 12/22/15 (== %m/%d/%y)
	mes "%X : "+ gettimestr("%X",19); // 05:04:15 (== %H:%M:%S)
	mes "%y : "+ gettimestr("%y",19); // 15 (year 00~99)
	mes "%Y : "+ gettimestr("%Y",19); // 2015 (year)
	mes "%z : "+ gettimestr("%z",19); // +0800 (timezone)
	mes "%Z : "+ gettimestr("%Z",19); // cannot determine timezone, this is blank
	mes "%% : "+ gettimestr("%%",19); // print '%' sign
	close;
}
```
Note: if you found some other format, then you have to try it out yourself  
if it crash, means not compatible

#### External Links:
[https://herc.ws/wiki/Gettimestr](https://herc.ws/wiki/Gettimestr)
