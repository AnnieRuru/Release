//===== Hercules Plugin ======================================
//= die_delay
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= x_0
//===== Compatible With: ===================================== 
//= Hercules 2019-03-19
//===== Description: =========================================
//= players unable to use @die when getting hit or attacking
//===== Topic ================================================
//= http://herc.ws/board/topic/16635-die-delay-on-hit/?do=findComment&comment=90893
//===== Additional Comments: =================================  
//= this plugin assume your prevent_logout in your battle_config has default setting 
//= prevent_logout_trigger have to at least has 2+8 
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/atcommand.h"
#include "common/timer.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"die_delay",
	SERVER_TYPE_MAP,
	"x_O",
	HPM_VERSION,
};

ACMD(kill) {
	if ( DIFF_TICK( timer->gettick(), sd->canlog_tick ) < battle->bc->prevent_logout ) {
		char msg[CHAT_SIZE_MAX];
		safesnprintf( msg, CHAT_SIZE_MAX, "There is a %d seconds delay of using this command to kill the character again", battle->bc->prevent_logout /1000 );
		clif->message( fd, msg );
		return false;
	}
	status_kill( &sd->bl );
	clif->message( sd->fd, msg_fd( fd, 13 )); // A pity! You've died.
	if ( fd != sd->fd )
		clif->message( fd, msg_fd( fd, 14 )); // Character killed.
	return true;
}

HPExport void plugin_init (void) {
	addAtcommand( "kill", kill );
}
