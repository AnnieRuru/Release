//===== Hercules Plugin ======================================
//= @sleep atcommand
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.1
//===== Compatible With: ===================================== 
//= Hercules 2019-03-19
//===== Description: =========================================
//= sleeps the player and make it invulnerable from attacks
//===== Topic ================================================
//= http://hercules.ws/board/topic/4875-sleep/
//===== Additional Comments: =================================  
//= want to take a break and go to toilet ? here comes to the rescue !
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/atcommand.h"
#include "common/timer.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"sleep",
	SERVER_TYPE_MAP,
	"1.1",
	HPM_VERSION,
};

ACMD(sleep) {
	if ( map->list[sd->bl.m].flag.pvp || map->list[sd->bl.m].flag.gvg || map->list[sd->bl.m].flag.gvg_castle || map->list[sd->bl.m].flag.battleground ) {
		clif->message( sd->fd, "Cannot use this command in pvp/gvg/battleground maps." );
		return false;
	}
	if ( map->list[sd->bl.m].flag.nowarp && map->list[sd->bl.m].flag.nowarpto ) {
		clif->message( sd->fd, "Cannot use this command in event maps." );
		return false;
	}
	if ( battle->bc->prevent_logout && ( DIFF_TICK( timer->gettick(), sd->canlog_tick ) < battle->bc->prevent_logout ) ) {
		clif->message( sd->fd, "Cannot use this command while in combat." );
		return false;
	}
	if ( sd->sc.opt1 && sd->sc.opt1 != OPT1_SLEEP ) {
		clif->message( sd->fd, "Cannot use this command during status ailment" );
		return false;
	}
	else if ( sd->sc.opt1 == OPT1_SLEEP ) {
		sd->sc.opt1 = 0;
		clif->changeoption( &sd->bl );
		clif->emotion( &sd->bl, 45 );
		status_change_end( &sd->bl, SC_TRICKDEAD, -1 );
		clif->message( sd->fd, "You have awake." );
	}
	else {
		status->change_start( NULL, &sd->bl, SC_TRICKDEAD, 10000, 1, 0, 0, 0, 1000, 0 );
		sd->sc.opt1 = OPT1_SLEEP;
		clif->changeoption( &sd->bl );
		clif->message( sd->fd, "You have fallen asleep." );
	}
	return true;
}

HPExport void plugin_init (void) {
	addAtcommand( "sleep", sleep );
}
