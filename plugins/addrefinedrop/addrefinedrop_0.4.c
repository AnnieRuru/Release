//===== Hercules Plugin ======================================
//= addrefinedrop
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.4
//===== Compatible With: ===================================== 
//= Hercules 2020-10-05
//===== Description: =========================================
//= add refine from monster drop
//===== Topic ================================================
//= http://herc.ws/board/topic/16659-item-drop-with-random-refine-levelcard/
//===== Additional Comments: =================================
//= 
//============================================================

#include "common/hercules.h"
#include "map/mob.h"
//#include "map/battle.h"
#include "map/itemdb.h"
#include "common/random.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"addrefinedrop",
	SERVER_TYPE_MAP,
	"0.4",
	HPM_VERSION,
};

static struct item_drop *mob_setdropitem_post(struct item_drop* retVal, int nameid, struct optdrop_group *options, int qty, struct item_data *data) {
	struct item_data *i_data = itemdb->exists(nameid);
	if (i_data->flag.no_refine)
		return retVal;
	if (i_data->type == IT_WEAPON || i_data->type == IT_ARMOR) {
//		retVal->item_data.refine = rnd() % 11; // 10 +1, rand(11) is random number between 0-10
		int random_number = rnd() % 100;
		if (random_number < 5)
			retVal->item_data.refine = 5;
		else if (random_number < 15)
			retVal->item_data.refine = 4;
		else if (random_number < 25)
			retVal->item_data.refine = 3;
		else if (random_number < 50)
			retVal->item_data.refine = 2;
		else if (random_number < 75)
			retVal->item_data.refine = 1;
		else
			retVal->item_data.refine = 0;
	}
	return retVal;
}

HPExport void plugin_init( void ) {
	addHookPost(mob, setdropitem, mob_setdropitem_post);
//	battle->bc->item_rate_equip = 10000;
}
