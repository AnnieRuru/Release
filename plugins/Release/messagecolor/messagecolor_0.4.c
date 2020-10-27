//===== Hercules Plugin ======================================
//= messagecolor
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.4
//===== Compatible With: ===================================== 
//= Hercules 2020-10-22
//===== Description: =========================================
//= make the unit talk in colorize message
//===== Topic ================================================
//= https://rathena.org/board/topic/118229-npctalk-color-in-script/?do=findComment&comment=357623
//===== Additional Comments: =================================  
//= Note: this one use some trick to get the message display on self ... just not in color
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/clif.h"
#include "map/script.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"messagecolor",
	SERVER_TYPE_MAP,
	"0.4",
	HPM_VERSION,
};

BUILDIN(messagecolor) {
	int gid = script_getnum(st,2);

	struct block_list *bl = map->id2bl(gid);
	if (bl == NULL) {
		ShowWarning("buildin_messagecolor: Error in finding object GID %d!\n", gid);
		return false;
	}
	
	char msg[CHAT_SIZE_MAX];
	safestrncpy(msg, script_getstr(st,3), CHAT_SIZE_MAX);
	int colorcode = script_getnum(st,4);

	if (bl->type == BL_PC) {
		struct map_session_data *tsd = map->id2sd(gid);
		clif->messagecolor(bl, colorcode, msg);
		clif->messagecolor_self(tsd->fd, colorcode, msg);
		clif->ShowScript(bl, msg, AREA);
	}
	else 
		clif->messagecolor(bl, colorcode, msg);
	return true;
}

HPExport void plugin_init (void) {
	addScriptCommand("messagecolor", "isi", messagecolor);
}
