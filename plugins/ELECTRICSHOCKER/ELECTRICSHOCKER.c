//===== Hercules Plugin ======================================
//= ELECTRICSHOCKER
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= x.x
//===== Compatible With: ===================================== 
//= Hercules 2019-04-11
//===== Description: =========================================
//= Allow RA_ELECTRICSHOCKER to imprison boss monsters
//===== Topic ================================================
//= http://herc.ws/board/topic/16690-ranger-electric-shock/?do=findComment&comment=91215
//===== Additional Comments: =================================
//= 
//============================================================

#include "common/hercules.h"
#include "map/mob.h"
#include "map/clif.h"
#include "map/skill.h"
#include "map/status.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"ELECTRICSHOCKER",
	SERVER_TYPE_MAP,
	"x.x",
	HPM_VERSION,
};

int skill_unit_onplace_timer_pre( struct skill_unit **src, struct block_list **bl, int64 *tick ) {
	if ( *src == NULL || *bl == NULL )
		return 0;
	struct skill_unit_group *sg = (*src)->group;
	struct block_list *ss = map->id2bl(sg->src_id);
	if ( sg == NULL || ss == NULL )
		return 0;
	enum sc_type type = status->skill2sc(sg->skill_id);
	uint16 skill_id = sg->skill_id;
	if ( sg->unit_id == UNT_ELECTRICSHOCKER && (status_get_mode(*bl) & MD_BOSS) ) {
		if ( (*bl)->id != ss->id ) {
			if ( status->change_start( ss, *bl, type, 10000, sg->skill_lv, sg->group_id, 0, 0, skill->get_time2( sg->skill_id, sg->skill_lv), SCFLAG_FIXEDRATE ) ) {
				map->moveblock( *bl, (*src)->bl.x, (*src)->bl.y, *tick );
				clif->fixpos(*bl);
			}
			skill->trap_do_splash( &(*src)->bl, sg->skill_id, sg->skill_lv, sg->bl_flag, *tick );
			sg->unit_id = UNT_USED_TRAPS; //Changed ID so it does not invoke a for each in area again.
		}
		if ( (*bl)->type == BL_MOB && ss != *bl )
			mob->skill_event(BL_UCAST(BL_MOB, *bl), ss, *tick, MSC_SKILLUSED|(skill_id<<16));
		hookStop();
	}
	return skill_id;
}
			
bool status_is_boss_resist_sc_post( bool retVal, enum sc_type type ) {
	if ( retVal == true && type == SC_ELECTRICSHOCKER )
		retVal = false;
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPre( skill, unit_onplace_timer, skill_unit_onplace_timer_pre );
	addHookPost( status, is_boss_resist_sc, status_is_boss_resist_sc_post );
}
