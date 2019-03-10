//===== Hercules Plugin ======================================
//= change equipment skin
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2019-03-10
//===== Description: =========================================
//= change equipment skin
//===== Topic ================================================
//= 
//===== Additional Comments: =================================  
//= only weapon supported, LOOK_SHIELD are fuck up ... not supported
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/clif.h"
#include "map/battle.h"
#include "map/script.h"
#include "common/conf.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

struct change_weapon_skin_data {
	int itemid;
	int skinid;
};

VECTOR_DECL(struct change_weapon_skin_data) weapon_skin_data;

HPExport struct hplugin_info pinfo = {
	"change_equipment_skin",
	SERVER_TYPE_MAP,
	"1.0",
	HPM_VERSION,
};

void clif_sendlook_pre( struct block_list **bl, int *id, int *type, int *val, int *val2, enum send_target *target ) {
	struct map_session_data *sd = BL_CAST(BL_PC, *bl);
	if ( sd != NULL ) {
		if ( *type == LOOK_WEAPON ) {
			int i = pc->checkequip( sd, script->equip[EQI_SHADOW_WEAPON -1] );
			if ( i >= 0 ) {
				struct item_data *item = sd->inventory_data[i];
				if ( item == NULL )
					return;				
				for ( i = 0; i < VECTOR_LENGTH(weapon_skin_data); i++ ) {
					if ( item->nameid == VECTOR_INDEX(weapon_skin_data, i).itemid ) {
						*val = VECTOR_INDEX(weapon_skin_data, i).skinid;
						break;
					}
				}
			}
		}
	}
}

void pc_unequipitem_pos_pre( struct map_session_data **sd, int *n, int *pos ) {
	if ( *sd == NULL )
		return;
	if ( *pos & EQP_SHADOW_WEAPON ) {
		(*sd)->weapontype1 = W_FIST;
		pc->calcweapontype(*sd);
		(*sd)->status.look.weapon = 0;
		clif->changelook( &(*sd)->bl, LOOK_WEAPON, (*sd)->status.look.weapon );
		hookStop();
	}
	return;
}

int read_change_equipment_skin(void) {
	const char *confpath = "conf/import/change_equipment_skin.conf";
	struct config_t change_equipment_skin_conf;
	if ( !libconfig->load_file( &change_equipment_skin_conf, confpath ) )
		return -1;

	struct config_setting_t *config_db = libconfig->setting_get_member( change_equipment_skin_conf.root, "change_equipment_skin" );
	if ( config_db == NULL ) {
		ShowError( "can't read %s\n", confpath );
		libconfig->destroy( &change_equipment_skin_conf );
		return -1;
	}

	struct config_setting_t *config = NULL;
	int itemid_ = 0, skinid_ = 0, i = 0;
	const char *looktype_string = NULL;
	VECTOR_INIT(weapon_skin_data);
	while ( config = libconfig->setting_get_elem( config_db, i++ ) ) {
		if ( !libconfig->setting_lookup_string( config, "Type", &looktype_string ) ) {
			ShowError( "Missing 'Type' on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", i, confpath );
			continue;
		}
		if ( !libconfig->setting_lookup_int( config, "ItemID", &itemid_ ) ) {
			ShowError( "Missing 'ItemID' on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", i, confpath );
			continue;
		}
		if ( !libconfig->setting_lookup_int( config, "SkinID", &skinid_ ) ) {
			ShowError( "Missing 'SkinID' on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", i, confpath );
			continue;
		}

		if ( !strcmp( looktype_string, "LOOK_WEAPON" ) ) {
			struct change_weapon_skin_data weapon_data_;
			weapon_data_.itemid = itemid_;
			weapon_data_.skinid = skinid_;
			VECTOR_ENSURE( weapon_skin_data, 1, 1 );
			VECTOR_PUSH( weapon_skin_data, weapon_data_ );
		}
		else
			ShowError( "Invalid 'Type' on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", i, confpath );
	}
	
//	ShowDebug( "%d\n", VECTOR_LENGTH(weapon_skin_data) );
//	for ( i = 0; i < VECTOR_LENGTH(weapon_skin_data); i++ ) {
//		ShowDebug( "%d", VECTOR_INDEX(weapon_skin_data, i).itemid );
//		ShowDebug( "%d\n", VECTOR_INDEX(weapon_skin_data, i).skinid );
//	}

	libconfig->destroy( &change_equipment_skin_conf );
	ShowStatus("Done reading '"CL_WHITE"%d"CL_RESET"' entries in '"CL_WHITE"%s"CL_RESET"'.\n", VECTOR_LENGTH(weapon_skin_data), confpath);
	return VECTOR_LENGTH(weapon_skin_data);
}

void clean_change_equipment_skin(void) {
	VECTOR_CLEAR(weapon_skin_data);
	return;
}

ACMD(reloadequipmentskin) {
	int count = 0;
	char msg[CHAT_SIZE_MAX] = "Done reloading '@reloadequipmentskin' with %d entries.";
	clean_change_equipment_skin();
	count = read_change_equipment_skin();
	safesnprintf( msg, CHAT_SIZE_MAX, msg, count );
	clif->message( sd->fd, msg );
	return true;
}

HPExport void plugin_init (void) {
	addHookPre( clif, sendlook, clif_sendlook_pre );
	addHookPre( pc, unequipitem_pos, pc_unequipitem_pos_pre );
	addAtcommand( "reloadequipmentskin", reloadequipmentskin );
	clean_change_equipment_skin();
	read_change_equipment_skin();
}
