hehehe ... I'm so happy that now mapflag also can be a plugin  
I think I can safely release all my custom mapflags for hercules users 

### Download: 1.3
plugin


Example:
```c
prontera    mapflag    fixedaspd    150
	setmf_fixedaspd "prontera", 150;
```
all players at prontera will move at default speed  
the value capped between 85~199 ... although the maximum may not actually 199,  
it can be battle_config.max_aspd for 1st/2nd jobs, or battle_config.max_third_aspd for 3rd jobs

```c
prontera    mapflag    fixedaspd    150    99
	setmf_fixedaspd "prontera", 150, 99;
```
all players except GM99 at prontera will move at default speed

```c
	removemf_fixedaspd "prontera";
```
remove the fixedaspd mapflag in prontera, without using `@reloadscript`

```c
	dispbottom getmf_fixedaspd( "prontera" ) +"";
	dispbottom getmf_fixedaspd( "prontera", 0 ) +"";
	dispbottom getmf_fixedaspd( "prontera", 1 ) +"";
```
first 2 lines return the fixedaspd value, 3rd line return the GM level bypass restriction  
Note: since the minimum value is 85, if the mapflag is off, return 0

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

1.3 - plugin
- change `*setmapflagfixedaspd` into `*setmf_fixedaspd` ... and so on

</details>
