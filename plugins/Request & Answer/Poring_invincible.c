//===== Hercules Plugin ======================================
//= Poring invincible
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0_0
//===== Compatible With: ===================================== 
//= Hercules 2018-05-17
//===== Description: =========================================
//= make poring invincible on prontera map
//===== Topic ================================================
//= http://herc.ws/board/topic/15903-how-i-do-make-a-invencible-mob/
//===== Additional Comments: =================================  
//= 
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/mob.h"
#include "map/battle.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"Poring invincible",
	SERVER_TYPE_MAP,
	"0_0",
	HPM_VERSION,
};

int64 battle_calc_damage_pre(struct block_list **src,struct block_list **bl,struct Damage **d,int64 *damage,uint16 *skill_id,uint16 *skill_lv) {
	struct block_list *s_bl = *src;
	if ( (s_bl = battle->get_master(*src)) == NULL ) {
		s_bl = *src;
	}
	if ( s_bl->type == BL_PC && (*bl)->type == BL_MOB ) {
		TBL_PC *sd = BL_UCAST( BL_PC, s_bl );
		TBL_MOB *md = BL_UCAST( BL_MOB, *bl );
		if ( !strcmp( mapindex_id2name(sd->mapindex), "prontera" ) && md->class_ == 1002 ) {
			hookStop();
			return 0;
		}
	}
	return 0;
}

HPExport void plugin_init (void) {
	addHookPre(battle, calc_damage, battle_calc_damage_pre);
}
