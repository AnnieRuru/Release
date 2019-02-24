//===== Hercules Plugin ======================================
//= favorite_tab
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-02-15
//===== Description: =========================================
//= any blue_gemstone(717) pick up by the player, immediately go to favorite tab
//===== Topic ================================================
//= https://rathena.org/board/topic/118227-consumables-in-favorites/?do=findComment&comment=357635
//===== Additional Comments: =================================  
//= 
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/clif.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"favorite_tab",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

static int pc_additem_post( int retVal, struct map_session_data *sd, const struct item *item_data, int amount, e_log_pick_type log_type ) {
	int i = pc->search_inventory(sd, 717);
	if ( i == INDEX_NOT_FOUND )
		return retVal;
	if ( sd->status.inventory[i].favorite == 0 ) {
		sd->status.inventory[i].favorite = 1;
		clif->favorite_item(sd, i);
	}
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPost( pc, additem, pc_additem_post );
}