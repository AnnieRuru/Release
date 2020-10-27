## Credit to [Epoque](https://rathena.org/board/topic/118545-release-stormbreaker/) for the idea

### Download : 1.1
plugin

while reading the description,

 > *inventoryselect();  
 Opens a prompt window which display a configurable list of items which may be selected (as shown in the screenshots).

I thought the menu items can be manipulate, but clif_item_identify_list said otherwise ...  
can only show the list from your inventory ...

which makes this kinda useless in my opinion

no, don't do stuffs like adding item temporary, player can find ways to dup item


tested with
```c
prontera,155,185,5	script	djksfhsd	1_F_MARIA,{
	mes "select an item";
	next;
	.@s = inventoryselect;
	if ( .@s == -1 ) {
		mes "Cancel";
		close;
	}
	dispbottom .@s +""; // return inventory index
	mes .@inventoryselect_id +"";
	mes getitemname2( .@inventoryselect_id, .@inventoryselect_identify, .@inventoryselect_refine, .@inventoryselect_attribute, .@inventoryselect_card1, .@inventoryselect_card2, .@inventoryselect_card3, .@inventoryselect_card4 );
	close;
}

prontera,158,185,5	script	djk2sfhsd	1_F_MARIA,{
	getitem Spectacles, 1;
	getitem2 1501, 1, 0,0,0, 0,0,0,0;
	close;
}
```

![inventoryselect](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/screen2019Hercules038.jpg)

<details>
	<summary>Revision History</summary>

1.0 - plugin

1.0a
- fix struct player_data twice

1.1 - plugin
- fix server crash when using magnifier

</details>
