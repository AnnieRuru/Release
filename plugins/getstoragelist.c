//===== Hercules Plugin ======================================
//= getstoragelist
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= x_O
//===== Compatible With: ===================================== 
//= Hercules 2019-01-31
//===== Description: =========================================
//= 
//===== Topic ================================================
//= http://herc.ws/board/topic/16536-help-1-read-maxlevel-from-exp_group_dbconf-and-2-getstorageinventorylist/?do=findComment&comment=90326
//===== Additional Comments: =================================  
//= 
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"getstoragelist",
	SERVER_TYPE_MAP,
	"x_O",
	HPM_VERSION,
};

BUILDIN(getstoragelist) {
	struct map_session_data *sd = script->rid2sd(st);
	char card_var[SCRIPT_VARNAME_LENGTH];
	int i = 0, j = 0, k = 0;

	if ( sd == NULL )
		return true;

	for ( i = 0; i < VECTOR_LENGTH(sd->storage.item); i++ ) {
		pc->setreg( sd, reference_uid( script->add_variable("@storagelist_id"), j ), VECTOR_INDEX(sd->storage.item, i).nameid );
		pc->setreg( sd, reference_uid( script->add_variable("@storagelist_amount"), j ), VECTOR_INDEX(sd->storage.item, i).amount );
		pc->setreg( sd, reference_uid( script->add_variable("@storagelist_refine"), j ), VECTOR_INDEX(sd->storage.item, i).refine );
		pc->setreg( sd, reference_uid( script->add_variable("@storagelist_identify"), j ), VECTOR_INDEX(sd->storage.item, i).identify );
		pc->setreg( sd, reference_uid( script->add_variable("@storagelist_attribute"), j ), VECTOR_INDEX(sd->storage.item, i).attribute );
		for ( k = 0; k < MAX_SLOTS; k++ ) {
			sprintf( card_var, "@storagelist_card%d", k +1 );
			pc->setreg( sd, reference_uid( script->add_variable(card_var), j ), VECTOR_INDEX(sd->storage.item, i).card[k]);
		}
		pc->setreg( sd, reference_uid(script->add_variable("@storagelist_expire"), j ), VECTOR_INDEX(sd->storage.item, i).expire_time);
		pc->setreg( sd, reference_uid(script->add_variable("@storagelist_bound"), j ), VECTOR_INDEX(sd->storage.item, i).bound);
		for ( k = 0; k < MAX_ITEM_OPTIONS; k++ ) {
			sprintf( card_var, "@storagelist_opt_id%d", k + 1 );
			pc->setreg(sd, reference_uid(script->add_variable(card_var), j), VECTOR_INDEX(sd->storage.item, i).option[k].index);
			sprintf( card_var, "@storagelist_opt_val%d", k + 1 );
			pc->setreg(sd, reference_uid(script->add_variable(card_var), j), VECTOR_INDEX(sd->storage.item, i).option[k].value);
			sprintf( card_var, "@storagelist_opt_param%d", k + 1 );
			pc->setreg(sd, reference_uid(script->add_variable(card_var), j), VECTOR_INDEX(sd->storage.item, i).option[k].param);
		}
		j++;
	}
	pc->setreg( sd, script->add_variable("@storagelist_count") , j );
	return true;
}

HPExport void plugin_init (void) {
	addScriptCommand( "getstoragelist", "", getstoragelist );
}