this is useful for certain events when you want your players to move at a fixed speed  
eg: even if the player has agi up, mount peco or using Authoritative Badge,  
all players movement speed are fixed at your desire value

### Download: 1.3
plugin


Example:
```c
prontera    mapflag    movespeed    150
	setmf_movespeed "prontera", 150;
```
all players at prontera will move at default speed  
the value capped between 20~1000

```c
prontera    mapflag    movespeed    150    99
	setmf_movespeed "prontera", 150, 99;
```
all players except GM99 at prontera will move at default speed

```c
	removemf_movespeed "prontera";
```
remove the movespeed mapflag in prontera, without using `@reloadscript`

```c
	dispbottom getmf_movespeed( "prontera" ) +"";
	dispbottom getmf_movespeed( "prontera", 0 ) +"";
	dispbottom getmf_movespeed( "prontera", 1 ) +"";
```
first 2 lines return the movespeed value, 3rd line return the GM level bypass restriction  
Note: since the minimum value is 20, if the mapflag is off, return 0

<details>
<summary>Revision History</summary>

1.0 - plugin

1.1 - plugin
- I hook wrong function, should be map->flags_init

1.2 - plugin
- update to latest revision
- add GM level bypass restriction
- add `*setmapflagmovespeed` `*removemapflagmovespeed` `*getmapflagmovespeed` script command

1.3 - plugin
- change `*setmapflagmovespeed` into `*setmf_movespeed` ... and so on

</details>
