//===== Hercules Plugin ======================================
//= movespeed mapflag
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.1
//===== Compatible With: ===================================== 
//= Hercules 2015-11-12
//===== Description: =========================================
//= fixed players movement speed on particular map
//===== Topic ================================================
//= http://herc.ws/board/topic/11153-movespeed-mapflag/
//===== Additional Comments: =================================  
//= prontera   mapflag   movespeed   150
//= make players move at default speed at prontera map
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pc.h"
#include "map/npc.h"
#include "common/utils.h"
#include "common/memmgr.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"movespeed", // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

struct mapflag_data {
	int movespeed;
};

void npc_parse_unknown_mapflag_pre( const char *name, char *w3, char *w4, const char* start, const char* buffer, const char* filepath, int *retval ) {
	if ( !strcmp(w3,"movespeed") ) {
		int16 m = map->mapname2mapid( name );
		int movespeed = atoi(w4);
		struct mapflag_data *mf;
		if ( movespeed > 0 ) {
			if ( movespeed < MIN_WALK_SPEED )
				movespeed = MIN_WALK_SPEED;
			else if ( movespeed > MAX_WALK_SPEED )
				movespeed = MAX_WALK_SPEED;
			if ( !( mf = getFromMAPD( &map->list[m], 0 ) ) ) {
				CREATE( mf, struct mapflag_data, 1 );
				mf->movespeed = movespeed;
				addToMAPD( &map->list[m], mf, 0, true );
			}
			else
				mf->movespeed = movespeed;
		}
		else
			ShowWarning( "npc_parse_mapflag: Missing 4th param for 'movespeed' flag ! removing flag from %s in file '%s', line '%d'.\n",  name, filepath, strline(buffer,start-buffer) );
		hookStop();
	}
	return;
}

unsigned short status_calc_speed_post( int retVal, struct block_list *bl, struct status_change *sc, int *speed ) {
	if ( bl->type == BL_PC ) {
		TBL_PC *sd = BL_CAST( BL_PC, bl );
		struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
		if ( mf && mf->movespeed )
			return (unsigned short)cap_value( mf->movespeed, MIN_WALK_SPEED, MAX_WALK_SPEED );
	}
	return retVal;
}

//	flush all movespeed mapflag back to default upon @reloadscript
void map_flags_init_pre(void) {
	int i;
	for ( i = 0; i < map->count; i++ ) {
		struct mapflag_data *mf = getFromMAPD( &map->list[i], 0 );
		if ( mf )
			removeFromMAPD( &map->list[i], 0 );
	}
	return;
}

HPExport void plugin_init (void) {
	addHookPre( "npc->parse_unknown_mapflag", npc_parse_unknown_mapflag_pre );
	addHookPost( "status->calc_speed", status_calc_speed_post );
	addHookPre( "map->flags_init", map_flags_init_pre );
}