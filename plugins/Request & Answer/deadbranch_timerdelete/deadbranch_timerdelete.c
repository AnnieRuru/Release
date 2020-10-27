// https://herc.ws/board/topic/18911-about-dead-bloody-branches/

#include "common/hercules.h"
#include "map/mob.h"
#include "common/timer.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"deadbranch_timelimit",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

bool check_deadbranch;

struct monster_data {
	unsigned deadbranch : 1;
};

static int mob_once_spawn_pre(struct map_session_data **sd, int16 *m, int16 *x, int16 *y, const char **mobname, int *class_, int *amount, const char **event, unsigned int *size, unsigned int *ai) {
	if (*class_ < 0)
		check_deadbranch = 1;
	else
		check_deadbranch = 0;
	return 0;
}

struct mob_data *mob_once_spawn_sub_post(struct mob_data *retVal, struct block_list *bl, int16 m, int16 x, int16 y, const char *mobname, int class_, const char *event, unsigned int size, unsigned int ai, int npc_id) {
	if (check_deadbranch == 1) {
		struct monster_data *mmd = getFromMOBDATA(retVal, 0);
		CREATE(mmd, struct monster_data, 1);
		mmd->deadbranch = 1;
		addToMOBDATA(retVal, mmd, 0, true);
		retVal->deletetimer = timer->add(timer->gettick() + 5000, mob->timer_delete, retVal->bl.id, 0);
	}
	return retVal;
}

static int unit_attack_post(int retVal, struct block_list *src, int target_id, int continuous) {
	if (target_id != 0) {
		struct block_list *target_bl = map->id2bl(target_id);
		if (target_bl->type == BL_MOB) {
			struct mob_data *md = map->id2md(target_id);
			struct monster_data *mmd = getFromMOBDATA(md, 0);
			if (mmd == NULL)
				return retVal;
			if (mmd->deadbranch == 1 && md->deletetimer != INVALID_TIMER) {
				timer->delete(md->deletetimer, mob->timer_delete);
				md->deletetimer = timer->add(timer->gettick() + 5000, mob->timer_delete, md->bl.id, 0);
			}
		}
	}
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPre(mob, once_spawn, mob_once_spawn_pre);
	addHookPost(mob, once_spawn_sub, mob_once_spawn_sub_post);
	addHookPost(unit, attack, unit_attack_post);
}
