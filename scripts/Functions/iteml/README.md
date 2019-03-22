## Credits goes to [Cydh](https://rathena.org/board/topic/118548-release-script-command-itemlink-to-generate/) and [members at opencore](https://github.com/OpenKore/openkore/issues/2477)

### Download : 1.1
script


### [Link Item System!!!!!](http://herc.ws/board/topic/10701-link-item-system/)

item link system, shift-click on the item to display the item description

thanks to Cydh posting the topic, I also get a crack on the formula,  
although this is a script function ... well not a script command like cydh did ... but I guess it also gets the job done


`F_ITEML( <ItemID>{, <refine>{, <card1>, <card2>, <card3>, <card4>{, <item option ID>, <item option value>, <item option param> }}} );`

```c
prontera,155,180,5	script	final test	1_F_MARIA,{
//	getnameditem 501, getcharid(0);
	npctalk F_ITEML(501);
	npctalk F_ITEML(1201);
	npctalk F_ITEML(5083);
	npctalk F_ITEML(19543);
	npctalk F_ITEML(501, 0, 254, 0, 150000 & 65535, 2); // this is currently bug in hercules
	debugmes F_ITEML(501, 0, 254, 0, 150000 & 65535, 2);
	npctalk F_ITEML(1501, 13);
	npctalk F_ITEML(1501, 13, 4001, 4002, 4003, 4004);
	npctalk F_ITEML(5083, 0, 4001,4001,4001,4001, 1,2000,0);
	npctalk F_ITEML(5083, 0, 4001,4001,4001,4001, VAR_ATTPOWER,2000,0);
	setarray .@opt_id, VAR_MAXHPAMOUNT, VAR_MAXSPAMOUNT;
	setarray .@opt_value, 2000, 2000;
//	setarray .@opt_param, 0, 0;
	npctalk F_ITEML(5083, 0, 4001,4001,4001,4001, .@opt_id, .@opt_value, .@opt_param);
//	debugmes F_ITEML(5083, 0, 4001,4001,4001,4001, .@opt_id, .@opt_value, .@opt_param);
	end;
}
```

<details>
<summary>Revision History</summary>

script for testing

0.1 - script

0.2 - script
- fix the base62 value, now it breaks 64*64*64 value

0.3 - script
- add show costume option for equipments

0.4 - script
- add refine and cards

1.0 - script
- add item options

1.1 - script
- fix item options when using constants

</details>
