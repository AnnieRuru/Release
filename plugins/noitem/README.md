### Download: 1.6
plugin

tested with
```
prontera	mapflag	noitem	0,4,5
```
```
prontera	mapflag	noitem	IT_HEALING, IT_WEAPON, IT_ARMOR
```
disable player from using healing items, and preventing equip weapon and armor.

.
```
prontera	mapflag	noitem	Poring_Card
```
```
prontera	mapflag	noitem	4001
```
disable player from using Poring Card


<details>
<summary>Revision History</summary>

1.0  
noitem_20140310

1.1  
-- fix a bug which messed up counter, should be [k] in npc.c  
----- counter is for string size (w4), [k] counter is the index of the array map->list[m].noitemlist  
noitem_20140828.  
noitem_20140908

1.2  
-- forgot to add aFree(temp), which cause memory leak  
noitem_20140911.  
noitem_20151030

1.3  
plugin  
-- remove MAX_RESTRICTED_LIST and hardcoded it as 50  
-- and change noitem from state, into number of items restricted, to make it loop lesser

1.4  
plugin  
-- fix a rare bug if the user put multiple noitem on the same map, it spam non-sensible error  
---- although this will overwrite the previous one without warning this time

1.5  
plugin  
--- remove the 50 items restriction limit by utilizing vector

1.6  
plugin  
--- fix server crash on certain compiler

</details>
