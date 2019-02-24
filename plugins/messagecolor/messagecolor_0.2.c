//===== Hercules Plugin ======================================
//= messagecolor
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2019-02-11
//===== Description: =========================================
//= make the unit talk in colorize message
//===== Topic ================================================
//= https://rathena.org/board/topic/118229-npctalk-color-in-script/?do=findComment&comment=357623
//===== Additional Comments: =================================  
//= Note: this does not work on players ... you can't see your own chat message, but can see other players' message
//============================================================

#include "common/hercules.h"
#include "map/clif.h"
#include "map/script.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"messagecolor",
	SERVER_TYPE_MAP,
	"0.2",
	HPM_VERSION,
};

BUILDIN(messagecolor) {
	struct block_list *bl = map->id2bl( script_getnum(st,2) );
	if (!bl) {
		ShowWarning("buildin_messagecolor: Error in finding object GID %d!\n", script_getnum(st,2));
		return false;
	}
	clif->messagecolor( bl, script_getnum(st,4), script_getstr(st,3) );
	return true;
}

HPExport void plugin_init (void) {
	addScriptCommand( "messagecolor", "isi", messagecolor );
}