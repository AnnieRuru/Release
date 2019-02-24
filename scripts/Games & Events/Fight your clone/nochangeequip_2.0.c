//===== Hercules Plugin ======================================
//= nochangeequip mapflag
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 2.0
//===== Compatible With: ===================================== 
//= Hercules 2015-12-21
//===== Description: =========================================
//= block players from changing equipments
//===== Topic ================================================
//= http://herc.ws/board/topic/4850-fight-your-clone/
//===== Additional Comments: =================================  
//= guild_vs2   mapflag   nochangeequip
//= block players from changing equipment on this map
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pc.h"
#include "map/npc.h"
#include "common/memmgr.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"nochangeequip",
	SERVER_TYPE_MAP,
	"2.0",
	HPM_VERSION,
};

struct mapflag_data {
	unsigned nochangeequip : 1;
};

void npc_parse_unknown_mapflag_pre( const char *name, char *w3, char *w4, const char* start, const char* buffer, const char* filepath, int *retval ) {
	if ( !strcmp(w3,"nochangeequip") ) {
		int16 m = map->mapname2mapid( name );
		struct mapflag_data *mf;
		if ( !( mf = getFromMAPD( &map->list[m], 0 ) ) ) {
			CREATE( mf, struct mapflag_data, 1 );
			mf->nochangeequip = 1;
			addToMAPD( &map->list[m], mf, 0, true );
		}
		else
			mf->nochangeequip = 1;
		hookStop();
	}
	return;
}

int pc_equipitem_pre( struct map_session_data *sd, int *n, int *req_pos ) {
	struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
	if ( mf && mf->nochangeequip ) {
		clif->equipitemack( sd, *n, 0, EIA_FAIL );
		hookStop();
		return 0;
	}
	return 1;
}

int pc_unequipitem_pre( struct map_session_data *sd, int *n, int *flag ) {
	struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
	if ( mf && mf->nochangeequip && !( *flag & PCUNEQUIPITEM_FORCE ) ) {
		clif->unequipitemack( sd, *n ,0, UIA_FAIL );
		hookStop();
		return 0;
	}
	return 1;
}

//	flush all nochangeequip mapflag back to default upon @reloadscript
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
	addHookPre( "npc->parse_unknown_mapflag", npc_parse_unknown_mapflag_pre );
	addHookPre( "pc->equipitem", pc_equipitem_pre );
	addHookPre( "pc->unequipitem", pc_unequipitem_pre );
	addHookPre( "map->flags_init", map_flags_init_pre );
}