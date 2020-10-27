//===== Hercules Plugin ======================================
//= checkguildemblem
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2020-10-06
//===== Description: =========================================
//= *checkguildemblem script command
//= return true(1) if guild has emblem, false(0) otherwise
//===== Topic ================================================
//= https://herc.ws/board/topic/18961-about-guild-emblems/
//===== Additional Comments: =================================
//= 
//============================================================

#include "common/hercules.h"
#include "map/guild.h"
#include "map/script.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"checkguildemblem",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

BUILDIN(checkguildemblem) {
	struct guild *g = guild->search(script_getnum(st, 2));
	if (g == NULL) {
		ShowWarning("buildin_getguildemblem: Guild ID not found or not exist yet.\n");
		script_pushint(st, 0);
		return false;
	}
	script_pushint(st, g->emblem_id? 1:0);
	return true;
}

HPExport void plugin_init( void ) {
	addScriptCommand("checkguildemblem", "i", checkguildemblem);
}
