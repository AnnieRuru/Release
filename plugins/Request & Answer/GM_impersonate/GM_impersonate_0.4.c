//===== Hercules Plugin ======================================
//= GM_impersonate
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.4
//===== Compatible With: ===================================== 
//= Hercules 2014-09-04
//===== Description: =========================================
//= block players of typing other player's name in the chatroom
//===== Topic ================================================
//= http://herc.ws/board/topic/7028-chat-space-limitationscammer-avoid-will-pay-for-help/?do=findComment&comment=42814
//===== Additional Comments: =================================  
//= I think I get it ...
//============================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../map/pc.h"
#include "../map/clif.h"
#include "../common/HPMi.h"
#include "../common/socket.h"
#include "../common/HPMDataCheck.h" // should always be the last file included! (if you don't make it last, it'll intentionally break compile time)

HPExport struct hplugin_info pinfo = {
	"GM_impersonate",	// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

bool clif_process_message_spaces( int retVal, struct map_session_data *sd, int format, char **name_, size_t *namelen_, char **message_, size_t *messagelen_) {
	if ( retVal == true ) {
		char* message = (char*)RFIFOP( sd->fd ,4) + strnlen(sd->status.name, NAME_LENGTH-1) + 3;
		int i, l = strlen(message);
		for ( i = 0; i <= l; i++ )
			if ( message[i] == 'xA0' )
				message[i] = 'x20'; // replace Alt+0160 into [space]
			if ( stristr( message, "    " ) ) {
				clif->colormes( sd->fd, COLOR_RED, "You are only allow to type maximum of 3 spaces in a dialog." );
				return false;
			}
			if ( stristr( message, "x20x3Ax20" ) || stristr( message, "x20x3Bx20" ) ) { // type " : " OR " ; " will be blocked
				clif->colormes( sd->fd, COLOR_RED, "You can't impersonate other players !" );
				return false;
			}
	}
	return true;
}

HPExport void plugin_init (void) {
	clif = GET_SYMBOL("clif");
	session = GET_SYMBOL("session");
	strlib = GET_SYMBOL("strlib");
	addHookPost("clif->process_message", clif_process_message_spaces);
}
