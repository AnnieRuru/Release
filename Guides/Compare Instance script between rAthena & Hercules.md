## Compare Instance script between rAthena & Hercules

### Hercules
```c
prontera,155,180,0	script	Test Instance	1_F_MARIA,{
	if ( has_instance2("guild_vs2") >= 0 )
		instance_destroy has_instance2("guild_vs2");
	if ( ( .@ins = instance_create( "Test Instance", getcharid(CHAR_ID_ACCOUNT), IOT_CHAR ) ) < 0 ) {
		mes "error : "+ .@ins;
		close;
	}
	if ( !getstrlen( instance_attachmap( "guild_vs2", .@ins, true, .@ins +"INST" ) ) ) {
		mes "error : -5";
		instance_destroy .@ins;
		close;
	}
	instance_set_timeout 3600, 15, .@ins;
	instance_init .@ins;
	warp has_instance("guild_vs2"), 49,49;
	end;
}
guild_vs2,49,49,5	script	test dialog	1_F_MARIA,{
	dispbottom strnpcinfo(NPC_MAP);
	mesf "%d monsters on this map", 'amount;
	next;
	select "Destroy";
	instance_destroy;
	end;
OnInstanceInit:
	monster instance_mapname("guild_vs2"), 49,49, "--ja--", PORING, 10, instance_npcname( strnpcinfo(NPC_NAME) )+"::OnMobDead";
	'amount = 10;
	end;
OnMobDead:
	--'amount;
	if ( !'amount ) {
		instance_announce -1, "all monster killed", bc_map;
		sleep 1000;
		instance_destroy instance_id();
	}
	end;
}
```
1. Hercules has more instance related script commands,  
but also make this more flexible and versatile once you learn to use them
   * Hercules has `*instance_attach` script command,  
make sure this script command is added AFTER `*instance_init`.  
This script command also useable outside instance
   * Hercules has to emulate the map name with `*instance_attachmap`  
only if the map name doesn't follow the syntax `<number>@<5 string limit>`
2. Hercules instance ID start from 0, rathena instance ID start from 1
   * Make sure the `*instance_announce` is set to -1

-----------------------------

### rAthena
```
// db/import/instance_db.txt
34,Test Instance,3600,15,guild_vs2,49,49
```
```c
prontera,155,180,0	script	Test Instance	1_F_MARIA,{
	if ( getstrlen( instance_mapname("guild_vs2") ) ) {
		instance_destroy;
	}
	if ( instance_create( "Test Instance", IM_CHAR ) < 0 ) {
		mes "failed to create instance";
		close;
	}
	instance_enter "Test Instance";
	end;
}
guild_vs2,49,49,5	script	test dialog	1_F_MARIA,{
	dispbottom strnpcinfo(4);
	mes sprintf( "%d monsters on this map", 'amount );
	next;
	select "Destroy";
	instance_destroy;
	end;
OnInstanceInit:
	monster instance_mapname("guild_vs2"), 49,49, "--ja--", 1002, 10, instance_npcname( strnpcinfo(0) )+"::OnMobDead";
	'amount = 10;
	end;
OnMobDead:
	--'amount;
	if ( !'amount ) {
		instance_announce 0, "all monster killed", bc_map;
		sleep 2000;
		instance_destroy;
	}
	end;
}
```
1. rAthena has instance_db.txt file ... make sure you add this line then `@reloadinstancedb`
2. rAthena has much less instance related script command,  
making rathena instance script easier to learn, but there are 2 drawback
   * rAthena has a bug that prevents you from entering multiple instance,  
this doesn't exist in hercules. See the spoiler below for details.
   * not as flexible as Hercules having `*instance_timeout` script command ...  
this value is hardcoded into database so all party has to use the same timeout value
3. rAthena instance ID start from 1, hercules instance ID start from 0﻿
   * make sure the `*instance_announce` is set to 0 ﻿
4. rAthena `*instance_enter` with `IM_NONE` flag has to specify with an instance ID  
`*instance_enter("<instance name>",{<x>,<y>,<char_id>,<instance id>});`


<details>
<summary>rAthena has this bug</summary>

Note: I didn't file a bug report because I was assuming they going to fix in this [pull request](https://github.com/rathena/rathena/pull/3163)  
rAthena has a bug that prevents you from entering multiple instance
```
34,Instance ONE,3600,15,guild_vs1,49,49
35,Instance TWO,3600,15,guild_vs3,49,49
```
```c
prontera,150,180,0	script	Check Instance	1_F_MARIA,{
OnClick:
	dispbottom strcharinfo(3); 
	dispbottom instance_id() +""; // this line sux ... however https://github.com/rathena/rathena/pull/3843
	end;
OnInit:
	bindatcmd "test", strnpcinfo(0)+"::OnClick";
	end;
}

prontera,155,180,0	script	Instance ONE	1_F_MARIA,{
	if ( getstrlen( instance_mapname("guild_vs1") ) ) {
		instance_enter "Instance ONE";
		end;
	}
	if ( instance_create( "Instance ONE", IM_PARTY ) < 0 ) {
		mes "failed to create instance";
		close;
	}
	instance_enter "Instance ONE";
	end;
}

prontera,158,180,0	script	Instance TWO	1_F_MARIA,{
	if ( getstrlen( instance_mapname("guild_vs2") ) ) {
		instance_enter "Instance TWO";
		end;
	}
	if ( instance_create( "Instance TWO", IM_GUILD ) < 0 ) {
		mes "failed to create instance";
		close;
	}
	instance_enter "Instance TWO";
	end;
}
```
Player A has same party with player B.  
Player A has same guild with player C.

Player B create Instance ONE for party.  
Player C create Instance TWO for guild.

Player A supposedly can go in both instance,  
but player A can only go to the party instance, not the guild  
Hercules doesn't suffer from this issue.
```c
prontera﻿,150,180,0	script	Check Instance	1_F_MARIA,{
OnClick:
	dispbottom strcharinfo(3);
	dispbottom instance_id() +""; // this script command is useless in hercules, always return -1 on non-instanced npc. An instanced npc don't need to use this script command anyway
	dispbottom has_instance2("guild_vs1") +"";
	dispbottom has_instance2("guild_vs2") +"";
	end;
OnInit:
	bindatcmd "test", strnpcinfo(0)+"::OnClick";
	end;
}

prontera,155,180,0	script	Instance ONE	1_F_MARIA,{
	if ( has_instance2("guild_vs1") >= 0 ) {
		warp has_instance("guild_vs1"), 49,49;
		end;
	}
	if ( ( .@ins = instance_create( "Instance ONE", getcharid(CHAR_ID_PARTY), IOT_PARTY ) ) < 0 ) {
		mes "error : "+ .@ins;
		close;
	}
	if ( !getstrlen( instance_attachmap( "guild_vs1", .@ins, true, .@ins +"INS1" ) ) ) {
		mes "error : 5";
		instance_destroy .@ins;
		close;
	}
	instance_set_timeout 3600, 15, .@ins;
	instance_init .@ins;
	warp has_instance("guild_vs1"), 49,49;
	end;
}

prontera,158,180,0	script	Instance TWO	1_F_MARIA,{
	if ( has_instance2("guild_vs2") >= 0 ) {
		warp has_instance("guild_vs2"), 49,49;
		end;
	}
	if ( ( .@ins = instance_create( "Instance ONE", getcharid(CHAR_ID_GUILD), IOT_GUILD ) ) < 0 ) {
		mes "error : "+ .@ins;
		close;
	}
	if ( !getstrlen( instance_attachmap( "guild_vs2", .@ins, true, .@ins +"INS2" ) ) ) {
		mes "error : 5";
		instance_destroy .@ins;
		close;
	}
	instance_set_timeout 3600, 15, .@ins;
	instance_init .@ins;
	warp has_instance("guild_vs2"), 49,49;
	end;
}﻿﻿﻿
```
</details>

### External Links
Help learning instance and how it works - [rAthena](https://rathena.org/board/topic/115701-help-learning-instance-and-how-it-works/) & [Hercules](http://herc.ws/board/topic/15909-help-learning-instance-and-how-it-works/)  
[Doubt about instance_create command](https://rathena.org/board/topic/115526-doubt-about-instance_create-command/?do=findComment&comment=345411)
