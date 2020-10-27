//===== Hercules Plugin ======================================
//= rare item drop announce
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2020-10-05
//===== Description: =========================================
//= reintroduce the rare item drop announcement that was removed
//===== Topic ================================================
//= https://herc.ws/board/topic/18955-two-small-requests-about-mob_dead-function/
//===== Additional Comments: =================================
//= https://github.com/HerculesWS/Hercules/commit/b767de877460a7fb55c4c8a6bd8e21bcbe079b07
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/mob.h"
//#include "map/battle.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"rare_drop_announce",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

int announce_this_when_the_drop_rate_is_below_this_value = 1000; // set this, please don't mind the long name as I heard the limit is 2048 characters (not sure)... and I don't think anyone makes a long variable name over 200 characters anyway

struct map_session_data *killer;

static int mob_dead_pre(struct mob_data **md, struct block_list **src, int *type) {
	if (*md == NULL)
		return 3;

	if (*src != NULL && (*src)->type == BL_PC)
		killer = BL_CAST(BL_PC, *src);
	else
		killer = NULL;
	return 0;
}

static void mob_item_drop_post(struct mob_data *md, struct item_drop_list *dlist, struct item_drop *ditem, int loot, int drop_rate, unsigned short flag) {
	if (md == NULL || dlist == NULL || ditem == NULL)
		return;
	if (killer != NULL && drop_rate < announce_this_when_the_drop_rate_is_below_this_value) {
		struct item_data *i_data = itemdb->exists(ditem->item_data.nameid);
		nullpo_retv(i_data);
		char output[CHAT_SIZE_MAX];
		safesnprintf(output, CHAT_SIZE_MAX, "'%s' won %s's %s (chance: %0.02f%%)", killer->status.name, md->name, i_data->jname, (float)drop_rate/100);
		clif->broadcast2(NULL, output, strlen(output) +1, 0xFFFF00, 0x190, 12, 0, 0, ALL_CLIENT);
	}
	return;
}

HPExport void plugin_init( void ) {
	addHookPre(mob, dead, mob_dead_pre);
	addHookPost(mob, item_drop, mob_item_drop_post);
//	battle->bc->item_rate_equip = 10000;
}
