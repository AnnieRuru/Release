//===== Hercules Plugin ======================================
//= getmonsterlabel
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-02-21
//===== Description: =========================================
//= return the monster label in strings
//===== Topic ================================================
//= https://github.com/HerculesWS/Hercules/pull/2382#issuecomment-465926256
//===== Additional Comments: =================================
//= totally custom stuff, not really recommend
//============================================================

#include "common/hercules.h"
#include "map/mob.h"
#include "map/script.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"getmonsterlabel",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

BUILDIN(getmonsterlabel) {
	struct mob_data *md = map->id2md( script_getnum(st,2) );
	if ( !md ) {
		ShowWarning( "buildin_getmonsterlabel: Error in finding object GID or target GID is not a monster.\n" );
		return false;
	}
	script_pushconststr( st, md->npc_event );
	return true;
}

HPExport void plugin_init(void) {
	addScriptCommand( "getmonsterlabel", "i", getmonsterlabel );
}

/*
prontera,155,185,5	script	qwer	1_F_MARIA,{
	monster "this", -1,-1, "--ja--", PORING, 10, strnpcinfo(NPC_NAME)+"::Onaaa";
	monster "this", -1,-1, "--ja--", POPORING, 10, strnpcinfo(NPC_NAME)+"::Onbbb";
	end;
Onaaa:
Onbbb:
	end;
}

prontera,160,185,5	script	asdf	1_F_MARIA,{
	.@count = getunits( BL_MOB, .@mobid, false, strnpcinfo(NPC_MAP) );
	for ( .@i = 0; .@i < .@count; ++.@i ) {
//		sscanf getmonsterlabel(.@mobid[.@i]), "%s::%s", .@npc_name$, .@label$; // f*cking sscanf doesn't work
//		if ( .@npc_name$ == "qwer" ) // alright, let's use regular expression
		if ( getmonsterlabel(.@mobid[.@i]) ~= "([^:]*)::([A-Za-z_]*)" )
			if ( $@regexmatch$[1] == "qwer" )
				unitkill .@mobid[.@i];
	}
	end;
}
*/