## Daily Quest - use gettimetick(2) or gettimestr

Most people when thinking about Daily Quest, they think it as a quest repeatable every 24-hours

```c
prontera,155,185,5	script	kjdshfksh	1_F_MARIA,{
	if ( apple_taken + 24*60*60 > gettimetick(2) ) {
		mes "You have already taken this quest";
		close;
	}
	getitem Apple, 1;
	apple_taken = gettimetick(2);
	end;
}
```

but my experience playing other MMORPG games, it actually reset at 12am  
yes, I can finish this quest by 11.59pm, and redo it again at 12.00am  
```c
prontera,155,185,5	script	kjdshfksh	1_F_MARIA,{
	.@today = atoi( gettimestr( "%Y%m%d", 9 ) );
	if ( apple_taken == .@today ) {
		mes "You have already taken this quest";
		close;
	}
	getitem Apple, 1;
	apple_taken = .@today;
	end;
}
```

I would prefer to use `*gettimestr` method, unless being explicitly ask redeem the prize every 24 hours  
I prefer `*gettimestr` over `gettime(GETTIME_DAYOFYEAR)` because I can read the value clearly  
`gettimestr("%Y%m%d", 9)` return `20190318`

-----------------------------------

### Show the time left
```c
prontera,158,185,5	script	kjdshfksh2	1_F_MARIA,{
	.@today = atoi( gettimestr( "%Y%m%d", 9 ) );
	if ( apple_taken == .@today ) {
//		mesf "You have to wait %s to take this quest again.", Time2Str( getcalendartime(0,0) );
		mesf "You have to wait %s to take this quest again.", timeleft__( getcalendartime(0,0) - gettimetick(2) );
		close;
	}
	getitem Apple, 1;
	apple_taken = .@today;
	end;
}
```
the commented line, the `Time2Str` came from Global_Functions.txt  
although I would prefer to use my own `timeleft__` function because it wont generate too many columns  
... apparently the `Time2Str` function used in official battleground script ...

### External Link:
[OnHour00 players value recover](http://herc.ws/board/topic/11526-onhour00-players-value-recover/)  
[Daily Rewards](http://herc.ws/board/topic/11465-daily-rewards/)  
[Delete on char_reg_num on a specific time](https://rathena.org/board/topic/115355-r-delete-on-char_reg_num-on-a-specific-time/)

### See Also
[The importance of gettimestr](https://github.com/AnnieRuru/Release/blob/master/Guides/The%20importance%20of%20gettimestr.md)  
[timeleft__ function](https://github.com/AnnieRuru/Release/blob/master/scripts/Functions/timeleft__/timeleft__1.2.txt)
