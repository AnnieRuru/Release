//===== Hercules Plugin ======================================
//= @allowobbdrop
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2020-10-14
//===== Description: =========================================
//= an atcommand to allow old blue box to drop on the floor
//= this is toggle, can allow to drop or not allow to drop depend on the player
//===== Topic ================================================
//= https://herc.ws/board/topic/18988-opening-obb-and-any-similar-consumables/
//===== Additional Comments: =================================  
//= actually just have remove the line 5215 in pc_isUseitem function
//= if (!pc->inventoryblank(sd)) { <-- this condition
//= but it become unsafe for other players, so better do it as atcommand
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"allowobbdrop",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

struct player_data {
	int use_item_id;
	unsigned allowobbdrop_flag : 1;
};

static int pc_reg_received_post(int retVal, struct map_session_data *sd) {
	if (retVal == 0)
		return 0;
	struct player_data *ssd;
	CREATE(ssd, struct player_data, true);
	ssd->use_item_id = 0;
	addToMSD(sd, ssd, 0, true);
	return 1;
}

static int pc_isUseitem_pre(struct map_session_data **sd, int *n) {
	struct player_data *ssd = getFromMSD(*sd, 0);
	nullpo_ret(ssd);
	ssd->use_item_id = (*sd)->status.inventory[*n].nameid;
	return 1;
}

static int pc_inventoryblank_post(int retVal, struct map_session_data *sd) {
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if (retVal == 0 && ssd->allowobbdrop_flag == 1 && ssd->use_item_id == 603)
		return 1; // hack it to tell still has space
	else
		return retVal;
}

ACMD(allowobbdrop) {
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if (ssd->allowobbdrop_flag) {
		ssd->allowobbdrop_flag = 0;
		clif->message(sd->fd, "@allowobbdrop is now turn OFF.");
	}
	else {
		ssd->allowobbdrop_flag = 1;
		clif->message(sd->fd, "@allowobbdrop is now turn ON.");
	}
	return true;
}

HPExport void plugin_init (void) {
	addHookPost(pc, reg_received, pc_reg_received_post);
	addHookPre(pc, isUseitem, pc_isUseitem_pre);
	addHookPost(pc, inventoryblank, pc_inventoryblank_post);

	addAtcommand("allowobbdrop", allowobbdrop);
}
