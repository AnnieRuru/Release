rAthena and Hercules has different approach on writing mob controller system

#### No.1: searching for monster's location.
rAthena uses `*getunitdata`, Hercules use `*getmapxy`

rAthena
```c
prontera,155,185,5	script	skdjfksjdf	1_F_MARIA,{
	if ( !.mobid ) {
		monster "this", -1,-1, "--ja--", 1002,1, strnpcinfo(0)+"::Onkill";
		.mobid = $@mobid;
	}
	else {
		getunitdata .mobid, .@unitdata;
		dispbottom "X: "+ .@unitdata[UMOB_X] +" - Y: "+ .@unitdata[UMOB_Y];
	}
	end;
Onkill:
	.mobid = 0;
	end;
}
```
Hercules
```c
prontera,155,185,5	script	skdjfksjdf	1_F_MARIA,{
	if ( !.mobid )
		.mobid = monster( "this", -1,-1, "--ja--", PORING,1, strnpcinfo(NPC_NAME)+"::Onkill" );
	else {
		getmapxy .@map$, .@x, .@y, UNITTYPE_MOB, .mobid;
		dispbottom sprintf( _$( "X: %d - Y: %d" ), .@x, .@y );
	}
	end;
Onkill:
	.mobid = 0;
	end;
}
```

#### No.2: rAthena push getunitdata into array, Hercules output the value
rAthena
```c
prontera,155,185,5	script	skdjfksjdf	1_F_MARIA,{
	if ( !.mobid ) {
		monster "this", -1,-1, "--ja--", 1002,1, strnpcinfo(0)+"::Onkill";
		.mobid = $@mobid;
	}
	else {
		getunitdata .mobid, .@unitdata;
		dispbottom "HP: "+ .@unitdata[UMOB_HP] +"/"+ .@unitdata[UMOB_MAXHP];
	}
	end;
Onkill:
	.mobid = 0;
	end;
}
```
Hercules
```c
prontera,155,185,5	script	skdjfksjdf	1_F_MARIA,{
	if ( !.mobid )
		.mobid = monster( "this", -1,-1, "--ja--", PORING,1, strnpcinfo(NPC_NAME)+"::Onkill" );
	else
		dispbottom sprintf( _$( "HP: %d/%d" ), getunitdata( .mobid, UDT_HP ), getunitdata( .mobid, UDT_MAXHP ) );
	end;
Onkill:
	.mobid = 0;
	end;
}
```

My thought on this .... hercules doing better job on simplifying this command  
however, hercules mob controller system currently very bug ....
