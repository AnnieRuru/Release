## Allow OnNPCKillEvent to increase kill counter for party members

I still see a lot members struggle with this issue,  
however this is entirely done with scripting alone

this is a normal template to increase kill counter normally for the killer alone
```c
prontera,155,185,5	script	kjsfhksh	1_F_MARIA,{
	dispbottom sprintf( _$("You have total kill %d poring"), poring_kill );
	end;
OnNPCKillEvent:
	if ( killedrid == PORING )
		++poring_kill;
	end;
}
```

and this is for party support
```c
prontera,155,185,5	script	kjsfhksh	1_F_MARIA,{
	dispbottom sprintf( _$("You have total kill %d poring"), poring_kill );
	end;
OnNPCKillEvent:
	.@pid = getcharid(CHAR_ID_PARTY);
	if ( !.@pid ) {
		if ( killedrid == PORING )
			++poring_kill;
	}
	else {
		.@killedrid = killedrid;
		getpartymember .@pid, 1;
		getpartymember .@pid, 2;
		getmapxy .@map1$, .@x1, .@y1, UNITTYPE_PC;
		for ( .@i = 0; .@i < $@partymembercount; ++.@i ) {
			if ( isloggedin( $@partymemberaid[.@i], $@partymembercid[.@i] ) ) {
				attachrid $@partymemberaid[.@i];
				getmapxy .@map2$, .@x2, .@y2, UNITTYPE_PC;
				if ( .@map1$ == .@map2$ && distance( .@x1, .@y1, .@x2, .@y2 ) <= 30 && Hp ) {
					if ( .@killedrid == PORING ) {
						++poring_kill;
					}
				}
			}
		}
	}
	end;
}
```

### Now to break it down part by part
.
```c
	.@pid = getcharid(CHAR_ID_PARTY);
	if ( !.@pid ) {
		if ( killedrid == PORING )
			++poring_kill;
	}
```
this part is obvious, prevent *getpartymember throw error when the player doesn't own a party

---
.
```c
		.@killedrid = killedrid;
```
change the scope of the killedrid from player based to script state  
so there is no need to use `*getvariableofpc` (hercules) or `*getvar` (rAthena)  

---
.
```c
		getpartymember .@pid, 1;
		getpartymember .@pid, 2;
		getmapxy .@map1$, .@x1, .@y1, UNITTYPE_PC;
		for ( .@i = 0; .@i < $@partymembercount; ++.@i ) {
			if ( isloggedin( $@partymemberaid[.@i], $@partymembercid[.@i] ) ) {
				attachrid $@partymemberaid[.@i];
```
[copy paste this part from script commands.txt](https://github.com/HerculesWS/Hercules/blob/52360f0cee7916f58da6eda0399f74199f957c44/doc/script_commands.txt#L2708-L2716)
```
// loop through both and use 'isloggedin' to count online party members
    for (.@i = 0; .@i < $@partymembercount; ++.@i)
        if (isloggedin($@partymemberaid[.@i], $@partymembercid[.@i]))
            .@count_online++;
// We search accountID & charID because a single party can have
// multiple characters from the same account. Without searching
// through the charID, if a player has 2 characters from the same
// account inside the party but only 1 char online, it would count
// their online char twice.
```
---
.
```c
		if ( .@map1$ == .@map2$ && distance( .@x1, .@y1, .@x2, .@y2 ) <= 30 && Hp ) {
```
the first 2 conditions is to check the party members are within range of the killer  
means if the attached party member having same map with the killer, and within 30 cells of the killer

Why **30** ?? this is actually a *magical number*  
you see, because the `OnNPCKillEvent:` return the mob ID (1002) instead of game ID (11000021)  
we can't really use mob controller system to retrieve the coordinate of the monster  
so we use the range from the killer instead  
I have actually tried to use `getbattleflag("area_size")` before, but players complained it doesn't work  
so I increased the distance to 30 (about twice the length), and they say the script is fixed  
I know this is weird ... so the number 30 is kinda stuck in most of my scripts  

1 possible explanation is this  
![long_range_shooting](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/long_range_shooting.jpg)

if the Archer shoot long distances, like using a bow, throw arrow, or mage firebolt, crusader throw shield  
then the party member from across the screen should gets the kill  
`area_size` is 14, so 14+14 = 28, so `30` works for them

`Hp` ... you don't want the counter to increase for dead party member
so if that player is dead, Hp is 0, and the condition will become false there
this is to prevent having players increase the kill count by just AFK there

---

### External Links:
[Euphy's Hunting Mission](https://rathena.org/board/topic/72779-hunting-mission-with-party-and-individual-quest/?do=findComment&comment=149217) ( [check this line](https://github.com/HerculesWS/Hercules/blob/610ae47b9d0250ef9df5abd86c29f0f72c584910/npc/custom/quests/hunting_missions.txt#L281) )  
[Party Share Kill](http://herc.ws/board/topic/4453-party-share-kill/?do=findComment&comment=28815) ( tr0n's Quest Board ) 
