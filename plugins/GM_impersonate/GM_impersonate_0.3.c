//===== Hercules Plugin ======================================
//= GM_impersonate
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.3
//===== Compatible With: ===================================== 
//= Hercules 2014-09-03
//===== Description: =========================================
//= block players of typing other player's name in the chatroom
//===== Topic ================================================
//= http://herc.ws/board/topic/7028-chat-space-limitationscammer-avoid-will-pay-for-help/?do=findComment&comment=42746
//===== Additional Comments: =================================  
//= hmm ... not really sure it works ...
//============================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../map/pc.h"
#include "../map/clif.h"
#include "../map/chat.h"
#include "../common/HPMi.h"
#include "../common/socket.h"
#include "../common/HPMDataCheck.h" // should always be the last file included! (if you don't make it last, it'll intentionally break compile time)

HPExport struct hplugin_info pinfo = {
	"chatroom_impersonate",	// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

bool clif_process_message_spaces( int retVal, struct map_session_data *sd, int format, char **name_, size_t *namelen_, char **message_, size_t *messagelen_) {
	if ( retVal == true && sd->chatID ) {
		char* message = (char*)RFIFOP( sd->fd ,4) + strnlen(sd->status.name, NAME_LENGTH-1) + 3;
		if ( stristr( message, ":" ) ) {
			struct chat_data *cd = (struct chat_data *)map->id2bl(sd->chatID);
			int i;
			for ( i = 0; i < cd->users; i++ ) {
				if ( stristr( message, cd->usersd[i]->status.name ) ) {
					clif->colormes( sd->fd, COLOR_RED, "You can't impersonate others in a chatroom !" );
					return false;
				}
			}
		}
	}
	return true;
}

HPExport void plugin_init (void) {
	map = GET_SYMBOL("map");
	clif = GET_SYMBOL("clif");
	session = GET_SYMBOL("session");
	strlib = GET_SYMBOL("strlib");
	addHookPost("clif->process_message", clif_process_message_spaces);
}
