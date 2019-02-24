//===== Hercules Plugin ======================================
//= OnPCStatCalcEvent
//===== By: ==================================================
//= AnnieRuru
//= originally by QQfoolsorellina
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2015-11-29
//===== Description: =========================================
//= give player permanent bonus
//===== Topic ================================================
//= http://herc.ws/board/topic/11292-onpcstatcalcevent/
//===== Additional Comments: =================================  
//= stat_recalc script command doesn't really work, I dunno why
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pc.h"
#include "map/npc.h"
//#include "map/script.h"
#include "map/status.h"
#include "common/nullpo.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"OnPCStatCalcEvent", // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

void status_calc_pc_additional_pre( struct map_session_data *sd, enum e_status_calc_opt *opt ) {
	nullpo_retv(sd);
	npc->event_doall_id( "OnPCStatCalcEvent", sd->bl.id );
	return;
}

/*	this stupid command doesn't work
BUILDIN(stat_recalc) {
	TBL_PC* sd;
	if ( script_hasdata(st,2) ) {
		if ( data_isstring( script_getdata(st,2) ) )
			sd = map->nick2sd( script_getstr(st,2) );
		else
			sd = map->id2sd( script_getnum(st,2) );
	} else
		sd = script->rid2sd(st);
	if (sd) {
//		status_calc_pc(sd, SCO_NONE);
//		status->calc_pc_(sd, SCO_NONE);
//		status->calc_pc_(sd, SCO_FORCE);
		status_calc_pc(sd, SCO_FORCE);
		ShowDebug( "run" );
	}
	return true;
}
*/
HPExport void plugin_init (void) {
	addHookPre( "status->calc_pc_additional", status_calc_pc_additional_pre );
//	addScriptCommand( "stat_recalc", "?", stat_recalc );
}