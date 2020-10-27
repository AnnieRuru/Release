//===== Hercules Plugin ======================================
//= Charms
//===== By: ==================================================
//= AnnieRuru
//= original by digitalhamster
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2015-12-27
//===== Description: =========================================
//= Give item bonus like the game DiabloII
//===== Topic ================================================
//= http://herc.ws/board/topic/11575-charms/
//===== Additional Comments: =================================  
//= no, please, this plugin DOES NOT DUP ITEM ! TESTED ! TRUST ME !!! XD
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pc.h"
#include "map/itemdb.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"charms",
	SERVER_TYPE_MAP,
	"1.0",
	HPM_VERSION,
};

struct charm_item_data {
	bool charm;
};

struct player_data { // this stupid player variable is needed to prevent item dup
	bool recalculate;
};

void itemdb_readdb_additional_fields_pre( int *itemid, config_setting_t *it, int *n_, const char *source ) {
	struct item_data *idata = itemdb->load( *itemid );
	config_setting_t *tt = libconfig->setting_get_member( it, "Charm" );
	struct charm_item_data *cidata = getFromITEMDATA( idata, 0 );
	if ( idata->type != IT_ETC )
		return;
	if ( !cidata ) {
		CREATE( cidata, struct charm_item_data, 1 );
		addToITEMDATA( idata, cidata, 0, true );
	}
	if ( tt )
		if ( libconfig->setting_get_bool(tt) )
			cidata->charm = true;
	return;
}

int itemdb_isstackable_pre( int *nameid ) {
	struct item_data *idata = itemdb->search( *nameid );
	struct charm_item_data *cidata;
	nullpo_ret(idata);
	if ( idata->type != IT_ETC )
		return 1;
	cidata = getFromITEMDATA( idata, 0 );
	if ( !cidata )
		return 1;
	if ( cidata->charm == true ) {
		hookStop();
		return 0;
	}
	return 1;
}

int itemdb_isstackable2_pre( struct item_data *data ) {
	struct charm_item_data *cidata = NULL;
	nullpo_ret(data);
	if ( data->type != IT_ETC )
		return 1;
	cidata = getFromITEMDATA( data, 0 );
	if( !cidata )
		return 1;
	if ( cidata->charm == true ) {
		hookStop();
		return 0;
	}
	return 1;
}

// TODO: Maybe should add those job/level/upper flag restriction like I did on eathena ? hmm ... but hercules omit those fields ... using default
void status_calc_pc_additional_pre( struct map_session_data *sd, enum e_status_calc_opt *opt ) {
	int i = 0;
	struct charm_item_data *cidata = NULL;
	for ( i = 0; i < MAX_INVENTORY; ++i ) {
		if ( !sd->inventory_data[i] )
			continue;
		if ( sd->inventory_data[i]->type != IT_ETC )
			continue;
		cidata = getFromITEMDATA( sd->inventory_data[i], 0 );
		if ( !cidata )
			continue;
		if ( cidata->charm == false )
			continue;
		if ( sd->inventory_data[i]->script ) {
			script->run( sd->inventory_data[i]->script, 0, sd->bl.id, 0 );
		}
	}
	return;
}

int pc_additem_post( int retVal, struct map_session_data *sd, struct item *item_data ,int *amount, e_log_pick_type *log_type ) {
	struct item_data *idata = itemdb->search( item_data->nameid );
	struct charm_item_data *cidata = NULL;
	if ( retVal != 0 )
		return retVal;
	if ( idata->type != IT_ETC )
		return retVal;
	cidata = getFromITEMDATA( idata, 0 );
	if ( !cidata )
		return retVal;
	if ( cidata->charm == true )
		status_calc_pc( sd, SCO_NONE );
	return retVal;
}

int pc_delitem_pre( struct map_session_data *sd, int *n, int *amount, int *type, short *reason, e_log_pick_type *log_type ) {
	struct charm_item_data *cidata = NULL;
	struct player_data *ssd = NULL;
	nullpo_retr( 1, sd );
	if ( sd->status.inventory[*n].nameid == 0 || *amount <= 0 || sd->status.inventory[*n].amount < *amount || sd->inventory_data[*n] == NULL ) {
		hookStop();
		return 1;
	}
	if ( sd->inventory_data[*n]->type != IT_ETC )
		return 0;
	cidata = getFromITEMDATA( sd->inventory_data[*n], 0 );
	if ( !cidata )
		return 0;
	if ( cidata->charm == true ) {
		ssd = getFromMSD( sd, 0 );
		if ( !ssd ) {
			CREATE( ssd, struct player_data, 1 );
			ssd->recalculate = 1;
			addToMSD( sd, ssd, 0, true );
		}
		else
			ssd->recalculate = 1;
	}
	return 0;
}
// maybe I should've just overload this function ...
int pc_delitem_post( int retVal, struct map_session_data *sd, int *n, int *amount, int *type, short *reason, e_log_pick_type *log_type ) {
	struct player_data *ssd = getFromMSD( sd, 0 );
	if ( ssd && ssd->recalculate == 1 ) {
//		ShowDebug( "run recalculation" );
		status_calc_pc( sd, SCO_NONE );
		ssd->recalculate = 0;
	}
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPre( "itemdb->readdb_additional_fields", itemdb_readdb_additional_fields_pre );
	addHookPre( "itemdb->isstackable", itemdb_isstackable_pre );
	addHookPre( "itemdb->isstackable2", itemdb_isstackable2_pre );
	addHookPre( "status->calc_pc_additional", status_calc_pc_additional_pre );
	addHookPost( "pc->additem", pc_additem_post );
	addHookPre( "pc->delitem", pc_delitem_pre );
	addHookPost( "pc->delitem", pc_delitem_post );
}
