### Download: 1.4
plugin

tested with
```
{
	Id: 909
	AegisName: "Jellopy"
	Name: "Jellopy"
	Buy: 6
	Weight: 10
	BuyingStore: true
	Charm: true
//	Charm_Stack: true
	Script: <" bonus bStr, 10000; ">
},
```

just having Jellopy inside inventory will get bonus  
Remember: it has to use *IT_ETC* type item, which has been default to type 3 if you didn't specify  
and also `Charm: true` flag

if added the "Charm_Stack: true" flag, the charms will stack
- if the flag is false (default), having 10 same charms will give bonus 10 times, because they are separate items
- if the flag is true, having 10 same charms will only give bonus once, because they are stack together


version 1.3 onwards, added nocharms mapflag  
[http://herc.ws/board/topic/15219-modification-to-noitem-plugin/](http://herc.ws/board/topic/15219-modification-to-noitem-plugin/)  
to disable charms bonuses given in certain maps
```
prontera	mapflag	nocharms
```
disable all charms from taking effect on the map
```
prontera	mapflag	nocharms	909
prontera	mapflag	nocharms	Jellopy
```
disable only this 'Jellopy' charm from giving bonus

[credit to digitalhamster](https://www.eathena.ws/board/index.php?s=&showtopic=246304&view=findpost&p=1503344)  
in case you guys forgotten, I fixed digitalhamster's patch on eathena forum 4 years ago

and if you like this plugin, remember to also rep up [Dastgir's post in this topic](http://herc.ws/board/topic/10254-charms-item-gift-effect-while-in-inventory/?p=60659)  
because I copy paste most of his stuffs

<details>
<summary>Revision History</summary>

1.0 - plugin
- use the Charm: true flag, credit to Dastgir
- use a player variable trick to prevent duplicate the item
  - because if run status_calc_pc before the memset into NULL, the item data is not clear, thus can duplicate items

1.1 - plugin
- added Charm_Stack: true flag, credit to Litro and Kong for the idea
- however it always run the status_calc_pc even if the player still having the item in the inventory

1.2 - plugin
- fix the errors only

1.3 - plugin
- update to latest revision
- add nochams mapflag to deny giving item bonus

1.4 - plugin
- fix memory leak when do multiple `@loadnpc`

</details>
