//===== Hercules Plugin ======================================
//= GM_impersonate
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2014-09-03
//===== Description: =========================================
//= block players of using more than 3 spaces
//===== Topic ================================================
//= http://herc.ws/board/topic/7028-chat-space-limitationscammer-avoid-will-pay-for-help/?do=findComment&comment=42740
//===== Additional Comments: =================================  
//= Alt+03232 can bypass this ?
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
	"max_3_space",	// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

bool clif_process_message_spaces( int retVal, struct map_session_data *sd, int format, char **name_, size_t *namelen_, char **message_, size_t *messagelen_) {
	if ( retVal == true ) {
		char* message = (char*)RFIFOP( sd->fd ,4) + strnlen(sd->status.name, NAME_LENGTH-1) + 3;
		if ( stristr( message, " " ) != NULL ) {
			clif->message( sd->fd, "Alt+03232 ? You must be joking, right ? Don't do that !" );
			return false;
		}
		if ( stristr( message, "    " ) != NULL ) {
			clif->message( sd->fd, "You're only allowed to do max 3 spaces in a row." );
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
