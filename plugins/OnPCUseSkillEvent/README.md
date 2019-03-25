After so many years of hearing rumor of this modification,  
finally today is the day I can release this to public


### Download : 1.4
plugin

remember that `@reloadskilldb` also can reload `conf/import/OnPCUseSkillEvent.conf` file



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



conf/import/OnPCUseSkillEvent.conf
```c
OnPCUseSkillEvent: (
{
//	SkillID: 2991
	SkillName: "CUSTOM_damage"
	Event_Label: "qwer::Ontarget"
},
{
	SkillName: "CUSTOM_nodamage"
	Event_Label: "qwer::Ontarget"
},
{

	SkillName: "CUSTOM_setpos"
	Event_Label: "qwer::Onpos"
},
{
	SkillName: "CUSTOM_self"
	Event_Label: "qwer::Onself"
},
)
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
	
Onself:	dispbottom "lv: "+ @useskilllv;
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

### Frequently Asked Question
[Why some skill ID doesn't work ?](http://herc.ws/board/topic/4925-help-skills-adding-custom-passive/)  
more to come ....
