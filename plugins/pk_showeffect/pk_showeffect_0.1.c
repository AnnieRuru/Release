//===== Hercules Plugin ======================================
//= pk_showeffect
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-03-19
//===== Description: =========================================
//= show specialeffect when hitting another player
//===== Topic ================================================
//= http://herc.ws/board/topic/16636-request-custom-item-script/?do=findComment&comment=90902
//===== Additional Comments: =================================  
//= setoption, specialeffect, status_change ... and probably hateffect ... ?
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/battle.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"pk_showeffect",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

int battle_check_target_post( int retVal, struct block_list *src, struct block_list *target, int flag ) {
	struct block_list *s_bl = src, *t_bl = target;
	if ( (t_bl = battle->get_master(target)) == NULL )
		t_bl = target;
	if ( (s_bl = battle->get_master(src)) == NULL )
		s_bl = src;

	if ( s_bl->type == BL_PC && t_bl->type == BL_PC && (flag & BCT_ENEMY) == BCT_ENEMY && retVal == 1 )
		clif->specialeffect( s_bl, 548, AREA ); // EF_RED_HIT

	return retVal;
}

HPExport void plugin_init (void) {
	addHookPost( battle, check_target, battle_check_target_post );
}
