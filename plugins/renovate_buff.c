//===== Hercules Plugin ======================================
//= renovate_buff
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-02-21
//===== Description: =========================================
//= certain buff are not reapply immediately
//===== Topic ================================================
//= http://herc.ws/board/topic/16587-renovate-lighting-walk-buff/?do=findComment&comment=90618
//===== Additional Comments: =================================
//= tested with both sc_start and cast the skill SR_LIGHTNINGWALK
//============================================================

/*
prontera,155,185,5	script	sdkfjhsdkf	1_F_MARIA,{
	sc_start SC_BLESSING, 5000, 1;
	sc_start SC_LIGHTNINGWALK, 5000, 1;
}
*/

#include "common/hercules.h"
#include "map/status.h"
#include "common/timer.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"renovate_buff",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

bool status_end_sc_before_start_post( bool retVal, struct block_list *bl, struct status_data *st, struct status_change *sc, enum sc_type type, int undead_flag, int val1, int val2, int val3, int val4 ) {
	if ( bl == NULL || st == NULL || sc == NULL || retVal == true )
		return true;
	if ( type == SC_LIGHTNINGWALK )
		status_change_end(bl, SC_LIGHTNINGWALK, INVALID_TIMER);
	return false;
}

HPExport void plugin_init(void) {
	addHookPost( status, end_sc_before_start, status_end_sc_before_start_post );
}
