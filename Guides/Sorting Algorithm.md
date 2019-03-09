As the name implies, its sort the value in the array from smallest number to largest number.

### Counting Sort
Currently the best sorting algorithm for sorting integer is [counting sort](https://en.wikipedia.org/wiki/Counting_sort).
```c
//	counting_sort_index <input array>, <output index>{, <total index>};
function	script	counting_sort_index	{
	.@total = .@size = getarg( 2, getarraysize( getarg(0) ) );
	copyarray .@arr, getarg(0), .@size;
	for ( .@i = 0; .@i < .@size; ++.@i )
		setd ".@index_"+ .@arr[.@i] +"["+( .@tmp[.@arr[.@i]]++ )+"]", .@i;
	for ( ; .@size; --.@size ) {
		.@index = getarraysize(.@tmp) -1;
		.@out[.@size -1] = getd( ".@index_"+ .@index +"["+( --.@tmp[.@index] )+"]" );
	}
	copyarray getarg(1), .@out, .@total;
	return;
}
```
Snippet
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
	F_ShuffleNumbers 0, .@size -1, .@r; // Shuffle Algorithm
	for ( .@i = 0; .@i < .@size; ++.@i )
		.@tmp2[.@i] = .@tmp[ .@r[.@i] ];

	// output the result ~
	.@string$ = .@tmp2[0] +"";
	for ( .@i = 1; .@i < .@size; ++.@i )
		.@string$ += ","+ .@tmp2[.@i];
	dispbottom "shuffle = "+ .@string$;

	// sort the array
	counting_sort_index .@tmp2, .@s, .@size;
	for ( .@i = 0; .@i < .@size; ++.@i )
		.@tmp3[.@i] = .@tmp2[ .@s[.@i] ];

	// output the result ~
	.@string$ = .@tmp3[0] +"";
	for ( .@i = 1; .@i < .@size; ++.@i )
		.@string$ += ","+ .@tmp3[.@i];
	dispbottom "sort = "+ .@string$;
	end;
}
```
Note : This sort from the smallest value to the largest value  
to sort from largest to smallest value,  
simply change `.@out[.@size -1]` into `.@out[.@total - .@size]`
```c
	.@out[.@size -1] = getd( ".@index_"+ .@index +"["+( --.@tmp[.@index] )+"]" );
		<--- change into --->
	.@out[.@total - .@size] = getd( ".@index_"+ .@index +"["+( --.@tmp[.@index] )+"]" );
```

I have used in
1. [Reward to the guild captured most flags, or have highest score](https://rathena.org/board/topic/72989-capture-the-flag-woe-script/?do=findComment&comment=150735) (outdated)  
2. [Reward to the players kill the most monsters](https://rathena.org/board/topic/115299-how-to-make-when-event-ends-announce-the-top-3-winners/) ([script](https://github.com/AnnieRuru/customs/blob/master/scripts/top_hunter.txt))
3. [List Player's pet eggs - Pet ID sorted](http://herc.ws/board/topic/16567-hpet-egg-renewal-fixing/?do=findComment&comment=90504) ([script](https://github.com/AnnieRuru/Release/blob/master/scripts/Utility/pet_egg_deleter.txt))

-----------------------------------------------

### Comb Sort
And the best sorting algorithm to sort strings is [comb sort](https://en.wikipedia.org/wiki/Comb_sort).
```c
// comb_sort_index <input array>, <output index>{, <total index>};
function	script	comb_sort_index	{
	.@size = .@gap = getarg( 2, getarraysize( getarg(0) ) );
	copyarray .@arr$, getarg(0), .@size;
	for ( .@i = 1; .@i < .@size; ++.@i )
		.@idx[.@i] = .@i;
	do {
		.@gap   = ( .@gap > 1 )? .@gap * 10 / 13 : .@gap;
		for ( .@i = .@gap; .@i < .@size; ++.@i ) {
			if ( .@arr$[.@i - .@gap] > .@arr$[.@i] ) {
				swap .@arr$[.@i - .@gap], .@arr$[.@i];
				swap .@idx[.@i - .@gap], .@idx[.@i];
			}
		}
	} while ( .@gap > 1 );
	copyarray getarg(1), .@idx, .@size;
	return;
}
```
Snippet
```c
prontera,155,185,5	script	dfdsfsadfsdf	1_F_MARIA,{
	// here we declare some array
	setarray .@tmp$, "qwer","asdf","zxcv","wert","sdfg","xcvb";
	.@size = getarraysize( .@tmp$ );

	// just to output the original array
	.@string$ = .@tmp$[0] +"";
	for ( .@i = 1; .@i < .@size; ++.@i )
		.@string$ += ","+ .@tmp$[.@i];
	dispbottom "original = "+ .@string$;

	// randomize the array ~
	F_ShuffleNumbers 0, .@size -1, .@r; // Shuffle Algorithm
	for ( .@i = 0; .@i < .@size; ++.@i )
		.@tmp2$[.@i] = .@tmp$[ .@r[.@i] ];

	// output the result ~
	.@string$ = .@tmp2$[0] +"";
	for ( .@i = 1; .@i < .@size; ++.@i )
		.@string$ += ","+ .@tmp2$[.@i];
	dispbottom "shuffle = "+ .@string$;

	// sort the array ~ A-Z
	comb_sort_index .@tmp2$, .@s, .@size;
	for ( .@i = 0; .@i < .@size; ++.@i )
		.@tmp3$[.@i] = .@tmp2$[ .@s[.@i] ];

	// output the result ~
	.@string$ = .@tmp3$[0] +"";
	for ( .@i = 1; .@i < .@size; ++.@i )
		.@string$ += ","+ .@tmp3$[.@i];
	dispbottom "sort = "+ .@string$;
	end;
}
```
This one sort the string alphabetically, A-Z

I have use in .... **NONE**  
I prepared this function, but never find any chance to use it  
because most of the time when needed to sort alphabets, it was done in SQL, in a large table
```sql
select * from `char` order by name;
```

#### External Links:
[Merge sort algorithm improvement](http://herc.ws/board/topic/4321-help-me-improve-this-merge-sort-algorithm/) - Credit to Keyworld and Haru for helping me ~  
[meko's Array manipulation functions](http://herc.ws/board/topic/14817-array-manipulation-functions/)

#### See also:
[Shuffle Algorithm](https://github.com/AnnieRuru/Release/blob/master/Guides/Shuffle%20Algorithm.md)
