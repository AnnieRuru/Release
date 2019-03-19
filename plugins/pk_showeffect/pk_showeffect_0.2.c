//===== Hercules Plugin ======================================
//= pk_showeffect
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2019-03-19
//===== Description: =========================================
//= show specialeffect when hitting another player
//===== Topic ================================================
//= http://herc.ws/board/topic/16636-request-custom-item-script/?do=findComment&comment=90923
//===== Additional Comments: =================================  
//= use hateffect instead of specialeffect
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/battle.h"
#include "common/timer.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"pk_showeffect",
	SERVER_TYPE_MAP,
	"0.2",
	HPM_VERSION,
};

int hateffect_timerid = INVALID_TIMER;

int hateffect_countdown( int tid, int64 tick, int id, intptr data ) {
	struct map_session_data *sd = map->id2sd(id);
	if ( sd != NULL )
		clif->hat_effect_single( &sd->bl, 27, false );
	hateffect_timerid = INVALID_TIMER;
	return 0;
}

int battle_check_target_post( int retVal, struct block_list *src, struct block_list *target, int flag ) {
	struct block_list *s_bl = src, *t_bl = target;
	if ( (t_bl = battle->get_master(target)) == NULL )
		t_bl = target;
	if ( (s_bl = battle->get_master(src)) == NULL )
		s_bl = src;

	if ( s_bl->type == BL_PC && t_bl->type == BL_PC && (flag & BCT_ENEMY) == BCT_ENEMY && retVal == 1 ) {
		if ( hateffect_timerid != INVALID_TIMER ) {
			timer->delete( hateffect_timerid, hateffect_countdown );
			hateffect_timerid = INVALID_TIMER;
		}
		hateffect_timerid = timer->add( timer->gettick() + 5000, hateffect_countdown, ((struct map_session_data*)s_bl)->bl.id, 0 );
		clif->hat_effect_single( s_bl, 27, true );
	}

	return retVal;
}

HPExport void plugin_init (void) {
	addHookPost( battle, check_target, battle_check_target_post );
}
