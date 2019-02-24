//===== Hercules Plugin ======================================
//= King of Emperium Hill
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.2
//===== Compatible With: ===================================== 
//= Hercules 2018-04-08
//===== Description: =========================================
//= stop guild owner from hitting the emperium over and over
//===== Topic ================================================
//= http://herc.ws/board/topic/4495-king-of-emperium-hill/
//===== Additional Comments: =================================  
//= finally a plugin for this popular script
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/mob.h"
#include "map/mapreg.h"
#include "map/battle.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"koe",
	SERVER_TYPE_MAP,
	"1.2",
	HPM_VERSION,
};

int battle_check_target_post( int retVal, struct block_list *src, struct block_list *target, int flag ) {
	if ( retVal == 1 ) {
		struct block_list *s_bl = src;
		if( (s_bl = battle->get_master(src)) == NULL )
			s_bl = src;
		if ( s_bl->type == BL_PC && target->type == BL_MOB ) {
			TBL_PC *sd = BL_CAST( BL_PC, s_bl );
			TBL_MOB *md = BL_CAST( BL_MOB, target );
			if ( ( md->class_ == MOBID_EMPELIUM && !strcmp( mapindex_id2name(sd->mapindex), "guild_vs1" ) ) &&
				( sd->status.guild_id == mapreg->readreg( script->add_str("$koegid") ) || battle->get_current_skill(src) > 0 ) )
				return 0;
		}
	}
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPost( battle, check_target, battle_check_target_post );
}