## What is dynamic_mob and how it affects scripts

What is dynamic_mob ?

conf/map/battle/monster.conf
```
// Dynamic Mobs Options
// Use dynamic mobs? (recommended for small-medium sized servers)
dynamic_mobs: true

// Remove Mobs even if they are hurt
mob_remove_damaged: true

// Delay before removing mobs from empty maps (default 5 min = 300 secs)
mob_remove_delay: 300000
```
This setting means, monsters only spawn on the map when there is a player warp in.  
if the map is left empty for 5 minutes, all dynamic spawned monsters,  
except spawn by boss_monster will remove from the map

it is recommend to turn on dynamic_mob because it help save memory on your live server

#### Example:
```c
guild_vs2,0,0,0,0	monster	Poring	1002,1,0,0,"asdf::On111"
guild_vs2,0,0,0,0	boss_monster	Golden Thief Bug	1086,1,20000,10000,"asdf::On222"
-    script    asdf    FAKE_NPC,{
OnInit:
	monster "guild_vs2", 0,0, "--ja--", POPORING, 1, strnpcinfo(NPC_NAME)+"::On333";
	bindatcmd "asdf", strnpcinfo(NPC_NAME)+"::Onaaa";
	end;
Onaaa:
	dispbottom mobcount( "guild_vs2", "asdf::On111" ) +"";
	dispbottom mobcount( "guild_vs2", "asdf::On222" ) +"";
	dispbottom mobcount( "guild_vs2", "asdf::On333" ) +"";
	end;
On111:
On222:
On333:
	end;
}
```
Let's say you login in Prontera town, and type "@asdf",  
poring and golden_bug are not spawn, only poporing has been spawn

"@rura guild_vs2", warp into the map, all 3 monsters are spawned  
"@go 0", return to prontera, after 5 minutes, the poring is remove from the map, only golden_bug and poporing remain  
"@reloadscript", return everything as normal, only poporing remain

This means :-
1. All monster spawn dynamically are affected by dynamic_mob, *monster script command are not affected
2. once a player warp into the map, all monsters will spawn normally
3. after 5 minutes the map left empty, all non-boss_monster will be remove

This can cause problem to the script if you want to count how many monsters left in the map  
because dynamic_mob will NOT load the monster until a player warp into the map


### External Links:
[mobs on server start](https://rathena.org/board/topic/118033-mobs-on-server-start/#comment-356654)  
[MVP board](https://rathena.org/board/topic/102715-mvp-board/?do=findComment&comment=357487)
