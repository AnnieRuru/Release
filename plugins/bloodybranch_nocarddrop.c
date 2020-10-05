//===== Hercules Plugin ======================================
//= bloodybranch_nocarddrop
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2020-09-28
//===== Description: =========================================
//= when summon Thanatos MVP through bloody branch, the MVP will never drop card
//===== Topic ================================================
//=  https://herc.ws/board/topic/18922-bloody-branch-card-drop/
//===== Additional Comments: =================================  
//= it will still drop card normally through *monster script command and @monster atcommand
//============================================================

#include "common/hercules.h"
#include "map/mob.h"
#include "map/itemdb.h"
#include "common/timer.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"bloodybranch_nocarddrop",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

bool check_bloodybranch;

struct monster_data {
	unsigned bloodybranch : 1;
};

static int mob_once_spawn_pre(struct map_session_data **sd, int16 *m, int16 *x, int16 *y, const char **mobname, int *class_, int *amount, const char **event, unsigned int *size, unsigned int *ai) {
	if (*class_ < 0)
		check_bloodybranch = 1;
	else
		check_bloodybranch = 0;
	return 0;
}

struct mob_data *mob_once_spawn_sub_post(struct mob_data *retVal, struct block_list *bl, int16 m, int16 x, int16 y, const char *mobname, int class_, const char *event, unsigned int size, unsigned int ai, int npc_id) {
	if (check_bloodybranch == 1) {
		struct monster_data *mmd = getFromMOBDATA(retVal, 0);
		CREATE(mmd, struct monster_data, 1);
		mmd->bloodybranch = 1;
		addToMOBDATA(retVal, mmd, 0, true);
	}
	return retVal;
}

static void mob_item_drop_pre(struct mob_data **md, struct item_drop_list **dlist, struct item_drop **ditem, int *loot, int *drop_rate, unsigned short *flag) {
	if (*md == NULL || *dlist == NULL || *ditem == NULL)
		return;
	struct monster_data *mmd = getFromMOBDATA(*md, 0);
	if (mmd == NULL)
		return;
//	ShowDebug("%d[%s] %d %d %d", (*md)->db->mob_id, (*md)->name, mmd->bloodybranch, (*ditem)->item_data.nameid, it->type); // put struct item_data above this
	if ((*md)->db->mob_id == 1708 && mmd->bloodybranch == 1) {
		struct item_data *it = itemdb->exists((*ditem)->item_data.nameid);
		if (it == NULL)
			return;
		if (it->type == IT_CARD)
			hookStop();
	}
	return;
}

HPExport void plugin_init (void) {
	addHookPre(mob, once_spawn, mob_once_spawn_pre);
	addHookPost(mob, once_spawn_sub, mob_once_spawn_sub_post);
	addHookPre(mob, item_drop, mob_item_drop_pre);
}
