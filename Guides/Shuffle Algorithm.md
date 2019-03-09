The main point of using this function is to shuffle arrays.  
This function uses [Fisher-Yates shuffle](https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle).  
-> It generate a set of numbers in random order that are not repeated.  
This can also found in [Hercules github](https://github.com/HerculesWS/Hercules/commit/bb214d4651c9c9aa9599f50cb5de52059176a87f).
```c
//	F_ShuffleNumbers( <start num>, <last num>, <output array>{, <count>} );
function	script	F_ShuffleNumbers	{
	deletearray getarg(2);
	.@static = getarg(0);
	.@range = getarg(1) +1 - .@static;
	.@count = getarg(3, .@range);
	if (.@range <= 0 || .@count <= 0)
		return 0;
	if (.@count > .@range)
		.@count = .@range;
	for (.@i = 0; .@i < .@range; ++.@i)
		.@temparray[.@i] = .@i;
	for (.@i = 0; .@i < .@count; ++.@i) {
		.@rand = rand(.@range);
		set getelementofarray( getarg(2), .@i ), .@temparray[.@rand] + .@static;
		.@temparray[.@rand] = .@temparray[--.@range];
	}
	return .@count;
}
```
**Examples**:  
`F_ShuffleNumbers( 0, 5, .@output); // possible output 4,1,3,2,0,5`  
`F_ShuffleNumbers( -5, 1, .@output); // possible output -3,-5,-4,-2,-1,1,0`  
`F_ShuffleNumbers( 0, 100, .@output, 5); // possible output 9,55,27,84,33`  
Reminder: Use `*freeloop` script command when appropriate !


#### Snippet
```c
prontera,155,185,5	script	dfdsfsadfsdf	1_F_MARIA,{
	// here we declare some array
	setarray .@tmp, 123,234,345,456,567,678,789;
	.@size = getarraysize( .@tmp );

	// just to output the original array
	.@string$ = .@tmp[0] +"";
	for ( .@i = 1; .@i < .@size; ++.@i )
		.@string$ += ","+ .@tmp[.@i];
	dispbottom "original = "+ .@string$;

	// randomize the array ~
	callfunc "F_ShuffleNumbers", 0, .@size -1, .@r;
	for ( .@i = 0; .@i < .@size; ++.@i )
		.@tmp2[.@i] = .@tmp[ .@r[.@i] ];

	// output the result ~
	.@string$ = .@tmp2[0] +"";
	for ( .@i = 1; .@i < .@size; ++.@i )
		.@string$ += ","+ .@tmp2[.@i];
	dispbottom "shuffle = "+ .@string$;

	end;
}
```

I have used them in ...  
1. ~outdated~ [reshuffle player's registration before entering battleground](https://rathena.org/board/topic/73771-battleground-emperium-with-shuffle-team-players/?sortby=date) ([recent topic](http://herc.ws/board/topic/15913-annieruru-emp-bg/) - [script](https://github.com/AnnieRuru/customs/blob/master/scripts/instanced_bg_emp_0.2r.txt))  
2. [randomize the woe time](https://rathena.org/board/topic/115861-woe-random-castle/?sortby=date) ([script](https://github.com/AnnieRuru/customs/blob/master/scripts/portal_event_1.0c))  
3. [shuffle the portal location, make it like a maze event](https://rathena.org/board/topic/77115-request-auto-game-warp-event/?sortby=date) ([outdated script](https://github.com/AnnieRuru/customs/blob/master/scripts/portal_event_1.0c))  

#### External Links:
[Question asked on the forum](https://rathena.org/board/topic/115224-copy-random-array/)  
[How me and Haru optimize and perfect this function](https://github.com/HerculesWS/Hercules/pull/872)  
[meko's Array manipulation functions](http://herc.ws/board/topic/14817-array-manipulation-functions/)  
[Shuffle Algorithm topic](http://herc.ws/board/topic/4889-shuffle-algorithm-commited-into-github/)

#### See Also:
[Sorting Algorithm](https://github.com/AnnieRuru/Release/blob/master/Guides/Sorting%20Algorithm.md)
