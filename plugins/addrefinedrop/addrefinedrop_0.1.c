//===== Hercules Plugin ======================================
//= addrefinedrop
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-03-27
//===== Description: =========================================
//= add refine from monster drop
//===== Topic ================================================
//= http://herc.ws/board/topic/16659-item-drop-with-random-refine-levelcard/
//===== Additional Comments: =================================
//= 
//============================================================

#include "common/hercules.h"
#include "map/mob.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"addrefinedrop",
	SERVER_TYPE_MAP,
	"x.x",
	HPM_VERSION,
};

struct item_drop *mob_setdropitem_post( struct item_drop* retVal, int nameid, int qty, struct item_data *data ) {
	if ( retVal->item_data.nameid == 1202 )
		retVal->item_data.refine = 2;
	return retVal;
}

HPExport void plugin_init( void ) {
	addHookPost( mob, setdropitem, mob_setdropitem_post );
}