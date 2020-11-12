rathena already has this by default --> [https://github.com/rathena/rathena/commit/27a0f3f](https://github.com/rathena/rathena/commit/27a0f3f)

someone else has tried to pull request in hercules, --> [https://github.com/HerculesWS/Hercules/pull/351](https://github.com/HerculesWS/Hercules/pull/351)

but it was denied, so have to leave this as plugin

### Download : 1.3
plugin


Tested with:
```c
prontera,155,185,4	script	Test	1_F_MARIA,{
	@a ^= 1;
	mes "hmm...";
	close2;
	recalculatestat(); // if player under a dialog, MUST use close2; then only recalculate
	end;
OnPCStatCalcEvent:
	if ( @a )
		bonus bStr, 1000;
	end;
}
```

#### OnPCStatCalcEvent can't run during npc dialog, and spam map-server error along with it
during the npc dialog, keep change the equipment, you'll lose the str bonus and map-server spam error
```
[Warning]: npc_event: player's event queue is full, can't add event 'Test::OnPCStatCalcEvent' !
```
conf/map/battle/items.conf
```
// Enable all NPC to allow changing of equipments while interacting? (Note 1)
// Script commands 'enable_items/disable_items' will not be override. (see doc/script_commands.txt)
// Default: true (official)
item_enabled_npc: true
```
change this to false should stop it


but none the less, its still a great feature
<details>
<summary>Revision History</summary>
1.0 - plugin

1.1 - plugin
- found a way to get *recalculatestat working, although its not thread-safe, but meh ~

1.2 - plugin
- remove some nullpo

1.3 - plugin
- update to latest hercules revision
</details>
