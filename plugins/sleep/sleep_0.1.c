//===== Hercules Plugin ======================================
//= @sleep atcommand
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2014-02-23
//===== Description: =========================================
//= sleeps the player and make it invulnerable from attacks
//===== Topic ================================================
//= http://herc.ws/board/topic/4563-need-a-sleep-script-for-hercules/?do=findComment&comment=29566
//===== Additional Comments: =================================  
//= want to take a break and go to toilet ? here comes to the rescue !
//============================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../map/pc.h"
#include "../common/HPMi.h"

#include "../common/HPMDataCheck.h" // should always be the last file included! (if you don't make it last, it'll intentionally break compile time)

HPExport struct hplugin_info pinfo = {
	"sleep",		// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

/*
*===================================
* Sleep (@sleep)
*-----------------------------------
*/

ACMD(sleep) {
	if ( map->agit_flag || map->agit2_flag ) { // skill not useable in WOE [A17kaliva]
		clif->message( fd, "Cannot use this command during WOE." );
		return false;
	}
	if ( !battle->bc->prevent_logout || DIFF_TICK( timer->gettick(), sd->canlog_tick) < battle->bc->prevent_logout ) {
		if ( sd->sc.opt1 != 0 && sd->sc.opt1 != OPT1_SLEEP ) {
			clif->message( fd, msg_txt(807) );
			return false;
		}
		if ( sd->sc.opt1 != OPT1_SLEEP) {
			status->change_start( NULL, &sd->bl, SC_TRICKDEAD, 10000, 1, 0, 0, 0, 1000, 0);
			sd->sc.opt1 = OPT1_SLEEP;
			clif->message( fd, msg_txt(805) ); // sleeping
		} else {
			sd->sc.opt1 = 0;
			clif->emotion(&sd->bl,45);
			status_change_end(&sd->bl, SC_TRICKDEAD, -1);
			clif->message( fd, msg_txt(806) ); // awake
			
		}
		clif->changeoption( &sd->bl );
		return true;
	}
	clif->message( fd, msg_txt(807) );
	return false;
}

HPExport void plugin_init (void) {
	atcommand = GET_SYMBOL("atcommand");
	clif = GET_SYMBOL("clif");
	map = GET_SYMBOL("map");
	timer = GET_SYMBOL("timer");
	status = GET_SYMBOL("status");
	battle = GET_SYMBOL("battle");
	addAtcommand("sleep",sleep);
}
