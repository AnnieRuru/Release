//===== Hercules Plugin ======================================
//= bAddMaxWeight
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2015-12-30
//===== Description: =========================================
//= sample bonus plugin
//===== Topic ================================================
//= http://herc.ws/board/topic/11594-show-error-item-on-pc-bonus2/?p=68173
//===== Additional Comments: =================================  
//= I just use existing bonus command in our repo and turn it as a plugin
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map/pc.h"
#include "map/map.h"
#include "common/nullpo.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"baddweight",
	SERVER_TYPE_MAP,
	">.<",
	HPM_VERSION,
};

int SP_ADDWEIGHT = 0;

int pc_bonus_pre( struct map_session_data *sd, int *type, int *val ) {
	nullpo_ret(sd);
	if ( *type == SP_ADDWEIGHT ) {
		if ( sd->state.lr_flag != 2 ) {
			sd->max_weight += *val;
			hookStop();
		}
	}
	return 0;
}

HPExport void plugin_init (void) {
	SP_ADDWEIGHT = map->get_new_bonus_id();
	script->set_constant( "baddweight", SP_ADDWEIGHT, false );
	addHookPre( "pc->bonus", pc_bonus_pre );
}