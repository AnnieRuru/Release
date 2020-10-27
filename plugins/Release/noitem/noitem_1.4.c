//===== Hercules Plugin ======================================
//= noitem mapflag
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.4
//===== Compatible With: ===================================== 
//= Hercules 2015-11-12
//===== Description: =========================================
//= block players from using items
//===== Topic ================================================
//= http://herc.ws/board/topic/4830-noitem-mapflag/
//===== Additional Comments: =================================  
//= prontera   mapflag   noitem   IT_WEAPON,IT_ARMOR
//= block players from equipping weapon and armor
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/map.h"
#include "map/npc.h"
#include "common/conf.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"noitem", // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.4",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

struct mapflag_data {
	int noitem;
	int noitemlist[50];
};

//	flush all noitem mapflag back to default upon @reloadscript
void map_flags_init_pre(void) {
	int i;
	for ( i = 0; i < map->count; ++i ) {
		struct mapflag_data *mf = getFromMAPD( &map->list[i], 0 );
		if ( mf )
			removeFromMAPD( &map->list[i], 0 );
	}
	return;
}

void npc_parse_unknown_mapflag_pre( const char **name, const char **w3, const char **w4, const char **start, const char **buffer, const char **filepath, int **retval ) {
	if ( !strcmp( *w3, "noitem" ) ) {
		int id = 0, i = 0, j = 0, k = 0, l = strlen(*w4);
		char *temp = "\0", *str = aStrdup(*w4);
		struct item_data *i_data;
		struct mapflag_data *mf;
		if ( l ) {
			int16 m = map->mapname2mapid( *name );
			if (( mf = getFromMAPD( &map->list[m], 0 )))
				removeFromMAPD( &map->list[m], 0 );
			CREATE( mf, struct mapflag_data, 1 );
			while ( i <= l && k < 50 ) {
				if ( str[i] != ' ' && str[i] != '	' && str[i] != ',' && str[i] != '\0' ) {
					temp[j++] = str[i];
				}
				else if ( str[i-1] != ' ' && str[i-1] != '	' && str[i-1] != ',' ) {
					temp[j] = '\0';
					if ( !strcmp( temp, "IT_HEALING" ) || !strcmp( temp, "0" ) )
						mf->noitemlist[k] = 0;
					else if ( !strcmp( temp, "IT_USABLE" ) || !strcmp( temp, "2" ) )
						mf->noitemlist[k] = 2;
					else if ( !strcmp( temp, "IT_WEAPON" ) || !strcmp( temp, "4" ) )
						mf->noitemlist[k] = 4;
					else if ( !strcmp( temp, "IT_ARMOR" ) || !strcmp( temp, "5" ) )
						mf->noitemlist[k] = 5;
					else if ( !strcmp( temp, "IT_CARD" ) || !strcmp( temp, "6" ) )
						mf->noitemlist[k] = 6;
					else if ( !strcmp( temp, "IT_DELAYCONSUME" ) || !strcmp( temp, "11" ) )
						mf->noitemlist[k] = 11;
					else if ( !strcmp( temp, "IT_CASH" ) || !strcmp( temp, "18" ) )
						mf->noitemlist[k] = 18;
					else if ( atoi(temp) == 0 ) {
						i_data = itemdb->search_name( temp );
						if ( i_data )
							mf->noitemlist[k] = i_data->nameid;
						else {
							ShowWarning( "npc_parse_mapflag: Item name \"%s\" does not exist.\n    Mapflag noitem: At %s (file '%s', line '%d').\n", temp, *name, *filepath, strline( *buffer, *start - *buffer ) );
							mf->noitemlist[k] = -1;
						}
					}
					else {
						id = atoi(temp);
						if ( itemdb->exists(id) )
							mf->noitemlist[k] = id;
						else {
							ShowWarning( "npc_parse_mapflag: Item ID \"%s\" does not exist.\n    Mapflag noitem: At %s (file '%s', line '%d').\n", temp, *name, *filepath, strline( *buffer, *start - *buffer ) );
							mf->noitemlist[k] = -1;
						}
					}
					++k;
					j = 0;
				}
				++i;
			}
			mf->noitem = k;
			addToMAPD( &map->list[m], mf, 0, true );
		}
		else
			ShowWarning( "npc_parse_mapflag: no Item ID/type input.\n           Mapflag noitem: At %s (file '%s', line '%d').\n", *name, *filepath, strline( *buffer, *start - *buffer ) );
		hookStop();
	}
	return;
}

int pc_isequip_post( int retVal, struct map_session_data *sd, int n ) {
	if ( retVal == 1 ) {
		struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
		if ( mf && mf->noitem ) {
			struct item_data *item = sd->inventory_data[n];
			int i = 0, slot = 0;
			ARR_FIND( 0, mf->noitem, i, item->nameid == mf->noitemlist[i] || item->type == mf->noitemlist[i] );
			if ( i < mf->noitem )
				return 0;
			if ( !itemdb_isspecial( sd->status.inventory[n].card[0] ) ) {
				for ( slot = 0; slot < MAX_SLOTS; ++slot ) {
					if ( sd->status.inventory[n].card[slot] ) {
						struct item_data *i_data = itemdb->exists( sd->status.inventory[n].card[slot] );
						ARR_FIND( 0, mf->noitem, i, i_data->nameid == mf->noitemlist[i] || i_data->type == mf->noitemlist[i] );
						if ( i < mf->noitem )
							return 0;
					}
				}
			}
		}
	}
	return retVal;
}

int pc_isUseitem_post( int retVal, struct map_session_data *sd, int n ) {
	if ( retVal == 1 ) {
		struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
		if ( mf && mf->noitem ) {
			struct item_data *item = sd->inventory_data[n];
			int i = 0;
			ARR_FIND( 0, mf->noitem, i, mf->noitemlist[i] == sd->status.inventory[n].nameid || item->type == mf->noitemlist[i] );
			if ( i < mf->noitem )
				return 0;
		}
	}
	return retVal;
}

int pc_checkitem_post( int retVal, struct map_session_data *sd ) {
	struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
	int i, calc_flag = 0;
	if ( sd->state.vending )
		return 0;
	if ( mf && mf->noitem ) {
		for ( i = 0; i < MAX_INVENTORY; ++i ) {
			int j = 0, slot = 0;
			struct item_data *i_data = itemdb->exists( sd->status.inventory[i].nameid );
			if ( sd->status.inventory[i].nameid == 0 || !sd->status.inventory[i].equip )
				continue;
			ARR_FIND( 0, mf->noitem, j, mf->noitemlist[j] == i_data->type || mf->noitemlist[j] == sd->status.inventory[i].nameid );
			if ( j < mf->noitem ) {
				pc->unequipitem(sd, i, 2);
				calc_flag = 1;
				continue;
			}
			if ( !itemdb_isspecial( sd->status.inventory[i].card[0] ) ) {
				for ( slot = 0; slot < MAX_SLOTS; ++slot ) {
					if ( sd->status.inventory[i].card[slot] ) {
						struct item_data *i_datac = itemdb->exists( sd->status.inventory[i].card[slot] );
						ARR_FIND( 0, mf->noitem, j, mf->noitemlist[j] == i_datac->type || mf->noitemlist[j] == sd->status.inventory[i].card[slot] );
						if ( j < mf->noitem ) {
							pc->unequipitem(sd, i, 2);
							calc_flag = 1;
							break;
						}
					}
				}
			}
		}
		if ( calc_flag && sd->state.active ) {
			pc->checkallowskill(sd);
			status_calc_pc( sd,SCO_NONE );
		}
	}
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPre( map, flags_init, map_flags_init_pre );
	addHookPre( npc, parse_unknown_mapflag, npc_parse_unknown_mapflag_pre );
	addHookPost( pc, isequip, pc_isequip_post );
	addHookPost( pc, isUseitem, pc_isUseitem_post );
	addHookPost( pc, checkitem, pc_checkitem_post );
}