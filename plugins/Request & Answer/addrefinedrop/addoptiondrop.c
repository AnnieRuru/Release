//===== Hercules Plugin ======================================
//= addrefinedrop
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.?
//===== Compatible With: ===================================== 
//= Hercules 2020-11-20
//===== Description: =========================================
//= add refine from monster drop
//===== Topic ================================================
//= http://herc.ws/board/topic/16659-item-drop-with-random-refine-levelcard/
//===== Additional Comments: =================================
//= note: I failed at adding more than 1 random option, this is the best I could do
//============================================================

#include "common/hercules.h"
#include "map/mob.h"
#include "map/battle.h"
#include "map/itemdb.h"
#include "common/random.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"addrefinedrop",
	SERVER_TYPE_MAP,
	"0.?",
	HPM_VERSION,
};

static struct item_drop *mob_setdropitem_post(struct item_drop* retVal, int nameid, struct optdrop_group *options, int qty, struct item_data *data) {
	struct item_data *i_data = itemdb->exists(nameid);
	if (i_data->flag.no_refine)
		return retVal;
	if (i_data->type == IT_WEAPON || i_data->type == IT_ARMOR) {
		retVal->item_data.refine = rnd() % 11; // 10 +1, rand(11) is random number between 0-10
		retVal->item_data.option->index = 3;
		retVal->item_data.option->value = 5;
	}
	return retVal;
}

HPExport void plugin_init( void ) {
	addHookPost(mob, setdropitem, mob_setdropitem_post);
	battle->bc->item_rate_equip = 10000;
}
