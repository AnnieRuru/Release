//===== Hercules Plugin ======================================
//= GM_impersonate
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.6
//===== Compatible With: ===================================== 
//= Hercules 2019-04-01
//===== Description: =========================================
//= block players from using 4 spaces and " : " in chat box
//===== Topic ================================================
//= http://herc.ws/board/topic/7028-chat-space-limitationscammer-avoid-will-pay-for-help/?do=findComment&comment=91043
//===== Additional Comments: =================================  
//= get through the bottom of this
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/clif.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"GM_impersonate",
	SERVER_TYPE_MAP,
	"0.6",
	HPM_VERSION,
};

bool pc_process_chat_message_post( bool retVal, struct map_session_data *sd, const char *message ) {
	if ( retVal == false )
		return false;
	int i, l = strlen(message);
	char *temp = aStrdup(message);
	for ( i = 0; i <= l; ++i )
		if ( temp[i] == '\xA0' )
			temp[i] = '\x20'; // replace Alt+0160 into [space]
	
	if ( stristr( temp, "    " ) ) {
		clif->messagecolor_self( sd->fd, COLOR_RED, "You are only allow to type maximum of 3 spaces in a dialog." );
		aFree(temp);
		return false;
	}
	if ( stristr( temp, "\x20\x3A\x20" ) || stristr( temp, "\x20\x3B\x20" ) ) { // type " : " OR " ; " will be blocked
		clif->messagecolor_self( sd->fd, COLOR_RED, "You can't impersonate other players !");
		aFree(temp);
		return false;
	}

	aFree(temp);
	return true;
}

HPExport void plugin_init (void) {
	addHookPost( pc, process_chat_message, pc_process_chat_message_post );
}
