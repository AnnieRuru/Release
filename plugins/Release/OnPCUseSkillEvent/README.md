After so many years of hearing rumor of this modification,  
finally today is the day I can release this to public

### Download : 1.5
plugin

original topic from [eathena](https://www.eathena.ws/board/index.php?showtopic=274088)

<details>
<summary>Revision History</summary>

1.0 rush release
plugin

1.1 - plugin
- now @reloadskilldb will also reload OnPCUseSkillEvent file
- added clean_skillevent - credit to Ind's manner system
- add self inf type skill, probably useful for mimic monster skill

1.2 - plugin
- no need to clean too much LOL
- now OnPCUseSkillEvent.conf can use more whitespace
- unsupported inf type will now display which skill ID properly
- fix a bug that inf type friend(16) when cast on self, will spam error message

1.3 - plugin
- update to latest revision
- fix server crash when parse the string in OnPCUseSkillEvent.txt
- help clean the temporary player variable

1.4 - plugin
- remove the 100 array limit by utilize VECTOR
- drop CSV format and use Hercules standard libconfig format

1.5 - plugin
- remove the OnPCUseSkillEvent.conf file

</details>

---
---

### Tested with
db/re/skill_db.conf
```c
{
	Id: 2991
	Name: "CUSTOM_damage"
	Description: "CUSTOM_damage"
	MaxLevel: 1
	Range: 15
	SkillType: {
		Enemy: true
	}
	Event_Label: "qwer::Ontarget"
},
{
	Id: 2992
	Name: "CUSTOM_nodamage"
	Description: "CUSTOM_nodamage"
	MaxLevel: 1
	Range: 15
	SkillType: {
		Friend: true
	}
	Event_Label: "qwer::Ontarget"
},
{
	Id: 2993
	Name: "CUSTOM_setpos"
	Description: "CUSTOM_setpos"
	MaxLevel: 3
	Range: 15
	SkillType: {
		Place: true
	}
	Event_Label: "qwer::Onpos"
},
{
	Id: 2994
	Name: "CUSTOM_self"
	Description: "CUSTOM_self"
	MaxLevel: 1
	Range: 15
	SkillType: {
		Self: true
	}
	Event_Label: "qwer::Onself"
},
```

data/luafiles514/lua files/skillinfoz/skillinfolist.lub
```lua
	[SKID.CUSTOM_damage] = {
		"CUSTOM_damage";
		SkillName = "Get Target enemy",
		MaxLv = 1,
		Type = "Quest",
		SpAmount = { 0 },
		bSeperateLv = true,
		AttackRange = { 15 },
	},
	[SKID.CUSTOM_nodamage] = {	
		"CUSTOM_nodamage";	
		SkillName = "Get Target friend",
		MaxLv = 1,
		Type = "Quest",
		SpAmount = { 0 },
		bSeperateLv = true,
		AttackRange = { 15 },
	},
	[SKID.CUSTOM_setpos] = {
		"CUSTOM_setpos";		
		SkillName = "Get Position",
		MaxLv = 3,
		Type = "Quest",
		SpAmount = { 0 },
		bSeperateLv = true,
		AttackRange = { 15 },
	},
	[SKID.CUSTOM_self] = {		
		"CUSTOM_self";	
		SkillName = "Self Cast",
		MaxLv = 1,
		Type = "Quest",
		SpAmount = { 0 },
		bSeperateLv = true,
		AttackRange = { 1 },
	},
```

data/luafiles514/lua files/skillinfoz/skillid.lub
```lua
	CUSTOM_damage = 2991,  
	CUSTOM_nodamage = 2992,  
	CUSTOM_setpos = 2993,  
	CUSTOM_self = 2994,
```

data/luafiles514/lua files/skillinfoz/skilldescript.lub
```lua
	[SKID.CUSTOM_damage] = {    
		"Test Target Enemy",
		"MAX Lv : 1 ",
		"push these variables :-",
		"'@useskilllv' for the skill level.",
		"'@useskilltarget' for the GID.",
	},
	[SKID.CUSTOM_nodamage] = {    
		"Test Target Friend",
		"MAX Lv : 1 ",
		"push these variables :-",
		"'@useskilllv' for the skill level.",
		"'@useskilltarget' for the GID.",
	},
	[SKID.CUSTOM_setpos] = {   
		"Test Coordinate",
		"MAX Lv : 3 ",
		"push these variables :-",
		"'@useskilllv' for the skill level.",
		"'@useskillx' for the X coordinate.",
		"'@useskilly' for the Y coordinate.",
	},
	[SKID.CUSTOM_self] = {  
		"Test Self",
		"MAX Lv : 1 ",
		"push these variables :-",
		"'@useskilllv' for the skill level.",
	}
```

and finally the npc script
```c
-	script	qwer	FAKE_NPC,{
Ontarget:
	dispbottom "lv: "+ @useskilllv +" | target "+ @useskilltarget;
	unittalk @useskilltarget, "from "+ strcharinfo(PC_NAME);
	end;
Onpos:	
	dispbottom "lv: "+ @useskilllv +" | x: "+ @useskillx +" | y: "+ @useskilly;
	end;
Onself:
	dispbottom "lv: "+ @useskilllv;
	end;
OnPCStatCalcEvent:	
	skill CUSTOM_damage, 1;
	skill CUSTOM_nodamage, 1;
	skill CUSTOM_setpos, 1;
	skill CUSTOM_self, 1;
	end;
} 
```

fuh !

so, yes, this is basically making a new skill, so read this wiki on how to make a new skill  
[Adding_new_skills](http://herc.ws/wiki/Adding_new_skills)

---

### Frequently Asked Question
[Why some skill ID doesn't work ?](http://herc.ws/board/topic/4925-help-skills-adding-custom-passive/)  
```
    On 3/17/2014 at 5:45 AM, AnnieRuru said:

    the problem is because of http://herc.ws/board/topic/512-skill-id-processing-overhaul/
    hercules implement the skill ID reading in such a way that its harder to add more skill ID ( to save memory )
    so we have to use the skill ID that is within the range of the gap that gravity not using
```
see this inside skill_get_index function
```c
	//[Ind/Hercules] GO GO GO LESS! - http://herc.ws/board/topic/512-skill-id-processing-overhaul/
	else if( skill_id > 1019 && skill_id < 8001 ) {
		if( skill_id < 2058 ) // 1020 - 2000 are empty
			skill_id = 1020 + skill_id - 2001;
		else if( skill_id < 2549 ) // 2058 - 2200 are empty - 1020+57
			skill_id = (1077) + skill_id - 2201;
		else if ( skill_id < 3036 ) // 2549 - 3000 are empty - 1020+57+348
			skill_id = (1425) + skill_id - 3001;
		else if ( skill_id < 5044 ) // 3036 - 5000 are empty - 1020+57+348+35
			skill_id = (1460) + skill_id - 5001;
		else
			ShowWarning("skill_get_index: skill id '%d' is not being handled!\n",skill_id);
	}
```
 

because eathena forum down, let me rephrase again what this modification does

if the skill inf type is INF_ATTACK_SKILL (target enemy only) - Enemy: true
- kill the unit with *unitkill
- zap another player's health with *heal -1000, 0;
- apply curse status with *sc_start
- make the target *unittalk ...
- etc etc etc...

if the skill inf type is INF_SUPPORT_SKILL (target friends and enemy) - Friend: true
- check if the player is party members, give buff by *sc_start
- check if the player is guild members, give buff by *sc_start during events
- make the player show emotion
- warp the target player to somewhere else
- give players item/stat/cashpoints .... by *getitem/*statusup  ... *attachrid + #CASHPOINTS

if the skill inf type is INF_GROUND_SKILL (target ground) - Place: true
- use *monster script command to summon monsters
- *makeitem to rain items ... skill level determine the item ID ...
- create a temporary npc .... using duplicatenpc plugin
- etc etc etc ...

this is just things I can think of, basically you can do ANYTHING with any script commands available

2nd thing is, when the skill type is INF_SUPPORT_SKILL, (Friend: true)  
you have to hold shift to target players  
this is client side limitation, require client hexing ... which I dunno how to do

---

### History

**Question**: Why there is no OnPCUseSkillEvent label in the npc script, but using the title OnPCUseSkillEvent ?

... eathena forum down

because the original modification made during eathena was something like this
```c
OnPCUseSkillEvent:
	switch ( @useskillid ) {
	case 2991:
		switch ( @useskilllv ) {
		case 1:
		case 2:
		case 3:
			...
		}
		break;
	case 2992:
		switch ( @useskilllv ) {
			...
		}
		break;
	...
	default:
		end;
	}
```
which runs this label every time a player use ANY skill  
and the original custom modification tax very heavily on server resources

that's why now this (revamp) version only pick which skill ID to run, along with your configurable event label
