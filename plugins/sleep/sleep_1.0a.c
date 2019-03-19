//===== Hercules Plugin ======================================
//= @sleep atcommand
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0a
//===== Compatible With: ===================================== 
//= Hercules 2014-08-28
//===== Description: =========================================
//= sleeps the player and make it invulnerable from attacks
//===== Topic ================================================
//= http://hercules.ws/board/topic/4875-sleep/
//===== Additional Comments: =================================  
//= want to take a break and go to toilet ? here comes to the rescue !
//============================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../map/pc.h"
#include "../common/HPMi.h"
#include "../common/timer.h"

#include "../common/HPMDataCheck.h" // should always be the last file included! (if you don't make it last, it'll intentionally break compile time)

HPExport struct hplugin_info pinfo = {
	"sleep",		// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0a",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

ACMD(sleep) {
	if ( map->list[sd->bl.m].flag.pvp || map->list[sd->bl.m].flag.gvg || map->list[sd->bl.m].flag.gvg_castle || map->list[sd->bl.m].flag.battleground ) {
		clif->message( fd, "Cannot use this command in pvp/gvg/battleground maps." );
		return false;
	}
	else if ( map->list[sd->bl.m].flag.nowarp && map->list[sd->bl.m].flag.nowarpto ) {
		clif->message( fd, "Cannot use this command in event maps." );
		return false;
	}
	else if ( battle->bc->prevent_logout && ( DIFF_TICK( timer->gettick(), sd->canlog_tick ) < battle->bc->prevent_logout ) ) {
		clif->message( fd, "Cannot use this command while in combat." );
		return false;
	}
	else if ( sd->sc.opt1 && sd->sc.opt1 != OPT1_SLEEP ) {
		clif->message( fd, "Cannot use this command during status ailment" );
		return false;
	}
	else if ( !sd->sc.opt1 ) {
		status->change_start( NULL, &sd->bl, SC_TRICKDEAD, 10000, 1, 0, 0, 0, 1000, 0 );
		sd->sc.opt1 = OPT1_SLEEP;
		clif->message( fd, "You have fallen asleep." );
	}
	else {
		sd->sc.opt1 = 0;
		clif->emotion( &sd->bl, 45 );
		status_change_end( &sd->bl, SC_TRICKDEAD, -1 );
		clif->message( fd, "You have awake." );
	}
	return true;
}

HPExport void plugin_init (void) {
	clif = GET_SYMBOL("clif");
	map = GET_SYMBOL("map");
	timer = GET_SYMBOL("timer");
	status = GET_SYMBOL("status");
	battle = GET_SYMBOL("battle");
	addAtcommand("sleep",sleep);
}
