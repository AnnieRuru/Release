After so many years of hearing rumor of this modification,  
finally today is the day I can release this to public


### Download : 1.2
plugin

remember that `@reloadskilldb` also can reload `conf/import/OnPCUseSkillEvent.txt` file



original topic from [eathena](https://www.eathena.ws/board/index.php?showtopic=274088)

<details>
<summary>Revision History</summary>

1.0 rush release
plugin

1.1
plugin
- now @reloadskilldb will also reload OnPCUseSkillEvent file
- added clean_skillevent - credit to Ind's manner system
- add self inf type skill, probably useful for mimic monster skill

1.2
plugin
- no need to clean too much LOL
- now OnPCUseSkillEvent.conf can use more whitespace
- unsupported inf type will now display which skill ID properly
- fix a bug that inf type friend(16) when cast on self, will spam error message

</details>

---
---

### Tested with
db/re/skill_db.txt
```c
1301,15,6,1,0,0,0,9,1,no,0,0x0,0,none,0,		CUSTOM_damage,CUSTOM_damage
1302,15,6,16,0,0x1,0,9,1,no,0,0x0,0,none,0,		CUSTOM_nodamage,CUSTOM_nodamage
1303,15,0,2,0,0,0,9,0,no,0,0x0,0,none,0,		CUSTOM_setpos,CUSTOM_setpos
1304,0,0,4,0,0,0,9,0,no,0,0x0,0,none,0,		CUSTOM_self,CUSTOM_self
```

db/re/skill_cast_db.txt
```c
1301,0,0,0,0,0,100,0
1302,0,0,0,0,0,100,0
1303,0,0,0,0,0,100,0
1304,0,0,0,0,0,100,0 
```

db/re/skill_require_db.txt
```c
1301,0,0,0,0,0,0,99,0,0,none,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
1302,0,0,0,0,0,0,99,0,0,none,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
1303,0,0,0,0,0,0,99,0,0,none,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
1304,0,0,0,0,0,0,99,0,0,none,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 
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
		MaxLv = 1,
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
 CUSTOM_damage = 1301,  
 CUSTOM_nodamage = 1302,  
 CUSTOM_setpos = 1303,  
 CUSTOM_self = 1304,
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
	"MAX Lv : 1 ",
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
  },
 ```



conf/import/OnPCUseSkillEvent.txt
```c
CUSTOM_damage,"qwer::Ontarget"
CUSTOM_nodamage,"qwer::Ontarget"
CUSTOM_setpos,"qwer::Onpos"
CUSTOM_self,"qwer::Onself"
```

and finally the npc script
```c
-	script	qwer	FAKE_NPC,{
Ontarget:
	dispbottom "lv: "+ @useskilllv +" | target "+ @useskilltarget;
	unittalk @useskilltarget, "from "+ strcharinfo(0);
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
