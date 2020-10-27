//===== Hercules Plugin ======================================
//= nobonedrop mapflag
//===== By: ==================================================
//= AnnieRuru
//= Originally by Cydh
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2018-04-04
//===== Description: =========================================
//= disable bone drop on certain maps
//===== Topic ================================================
//= http://herc.ws/board/topic/11648-no-bone-drop-on-specific-map-script-for-herc/
//===== Additional Comments: =================================  
//= only if your bone_drop battleflag set as 1 or 2
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/map.h"
#include "map/npc.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"nobonedrop",
	SERVER_TYPE_MAP,
	"0.2",
	HPM_VERSION,
};

struct mapflag_data {
	unsigned nobonedrop : 1;
};

int bone_drop_value = 0;

void npc_parse_unknown_mapflag_pre( const char **name, const char **w3, const char **w4, const char **start, const char **buffer, const char **filepath, int **retval ) {
	if ( !strcmp( *w3,"nobonedrop" ) ) {
		int16 m = map->mapname2mapid( *name );
		struct mapflag_data *mf;
		if ( !( mf = getFromMAPD( &map->list[m], 0 ) ) ) {
			CREATE( mf, struct mapflag_data, 1 );
			addToMAPD( &map->list[m], mf, 0, true );
		}
		mf->nobonedrop = 1;
		hookStop();
	}
	return;
}

int pc_dead_pre( struct map_session_data **sd, struct block_list **src ) {
	struct mapflag_data *mf = getFromMAPD( &map->list[ (*sd)->bl.m ], 0 );
	if ( mf && mf->nobonedrop )
		battle->bc->bone_drop = 0;
	return 0;
}

int pc_dead_post( int retVal, struct map_session_data *sd, struct block_list *src ) {
	struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
	if ( mf && mf->nobonedrop )
		battle->bc->bone_drop = bone_drop_value;
	return retVal;
}

//	flush all nobonedrop mapflag back to default upon @reloadscript
void map_flags_init_pre(void) {
	int i;
	for ( i = 0; i < map->count; ++i ) {
		struct mapflag_data *mf = getFromMAPD( &map->list[i], 0 );
		if ( mf )
			removeFromMAPD( &map->list[i], 0 );
	}
	return;
}

HPExport void plugin_init (void) {
	bone_drop_value = battle->bc->bone_drop;

	addHookPre( npc, parse_unknown_mapflag, npc_parse_unknown_mapflag_pre );
	addHookPre( pc, dead, pc_dead_pre );
	addHookPost( pc, dead, pc_dead_post );
	addHookPre( map, flags_init, map_flags_init_pre );
}