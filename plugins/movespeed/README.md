this is useful for certain events when you want your players to move at a fixed speed  
eg: even if the player has agi up, mount peco or using Authoritative Badge,  
all players movement speed are fixed at your desire value

### Download: 1.2
plugin


Example:
```c
prontera    mapflag    movespeed    150
setmapflagmovespeed "prontera", 150;
```
all players at prontera will move at default speed

```c
prontera    mapflag    movespeed    150    99
setmapflagmovespeed "prontera", 150, 99;
```
all players except GM99 at prontera will move at default speed

```c
removemapflagmovespeed "prontera";
```
remove the movespeed mapflag in prontera, without using "@reloadscript"

```c
	dispbottom getmapflagmovespeed( "prontera" ) +"";
	dispbottom getmapflagmovespeed( "prontera", 0 ) +"";
	dispbottom getmapflagmovespeed( "prontera", 1 ) +"";
```
first 2 lines return the movespeed value, 3rd line return the GM level bypass restriction

<details>
<summary>Revision History</summary>

1.0 - plugin

1.1 - plugin
- I hook wrong function, should be map->flags_init

1.2 - plugin
- update to latest revision
- add GM level bypass restriction
- add `*setmapflagmovespeed` `*removemapflagmovespeed` `*getmapflagmovespeed` script command

</details>
