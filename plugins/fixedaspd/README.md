hehehe ... I'm so happy that now mapflag also can be a plugin  
I think I can safely release all my custom mapflags for hercules users 

### Download: 1.2
plugin


Example:
```c
prontera    mapflag    fixedaspd    150
setmapflagfixedaspd "prontera", 150;
```
all players at prontera will move at default speed

```c
prontera    mapflag    fixedaspd    150    99
setmapflagfixedaspd "prontera", 150, 99;
```
all players except GM99 at prontera will move at default speed

```c
removemapflagfixedaspd "prontera";
```
remove the fixedaspd mapflag in prontera, without using "@reloadscript"

```c
	dispbottom getmapflagfixedaspd( "prontera" ) +"";
	dispbottom getmapflagfixedaspd( "prontera", 0 ) +"";
	dispbottom getmapflagfixedaspd( "prontera", 1 ) +"";
```
first 2 lines return the fixedaspd value, 3rd line return the GM level bypass restriction

<details>
<summary>Revision History</summary>

1.0 - plugin

1.1 - plugin
- I hook wrong function, should be map->flags_init

1.1a -
- optimized some part 

1.2 - plugin
- update to latest revision
- add GM level bypass restriction
- add `*setmapflagfixedaspd` `*removemapflagfixedaspd` `*getmapflagfixedaspd` script command

</details>
