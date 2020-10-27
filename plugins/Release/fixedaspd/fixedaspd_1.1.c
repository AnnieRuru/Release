//===== Hercules Plugin ======================================
//= fixedaspd mapflag
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.1
//===== Compatible With: ===================================== 
//= Hercules 2015-11-12
//===== Description: =========================================
//= fixed players attack speed on particular map
//===== Topic ================================================
//= http://herc.ws/board/topic/11155-fixedaspd-mapflag/
//===== Additional Comments: =================================  
//= prontera   mapflag   fixedaspd   150
//= fixed players attack speed at 150 speed at prontera map
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
	"fixedaspd", // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

struct mapflag_data {
	int fixedaspd;
};

void npc_parse_unknown_mapflag_pre( const char *name, char *w3, char *w4, const char* start, const char* buffer, const char* filepath, int *retval ) {
	if ( !strcmp(w3,"fixedaspd") ) {
		int16 m = map->mapname2mapid( name );
		int fixedaspd = atoi(w4);
		struct mapflag_data *mf;
		if ( fixedaspd > 0 ) {
			if ( fixedaspd < 85 )
				fixedaspd = 85;
			else if ( fixedaspd > 199 )
				fixedaspd = 199;
			if ( !( mf = getFromMAPD( &map->list[m], 0 ) ) ) {
				CREATE( mf, struct mapflag_data, 1 );
				mf->fixedaspd = 2000 - fixedaspd *10;
				addToMAPD( &map->list[m], mf, 0, true );
			}
			else
				mf->fixedaspd = 2000 - fixedaspd *10;
		}
		else
			ShowWarning( "npc_parse_mapflag: Missing 4th param for 'fixedaspd' flag ! removing flag from %s in file '%s', line '%d'.\n",  name, filepath, strline(buffer,start-buffer) );
		hookStop();
	}
	return;
}
	
unsigned short status_calc_fix_aspd_post( int retVal, struct block_list *bl, struct status_change *sc, int *speed ) {
	if ( bl->type == BL_PC ) {
		TBL_PC *sd = BL_CAST( BL_PC, bl );
		struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
		if ( mf && mf->fixedaspd )
			return cap_value( mf->fixedaspd, 0, 2000 );
	}
	return retVal;
}

//	flush all fixedaspd mapflag back to default upon @reloadscript
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
	addHookPost( "status->calc_fix_aspd", status_calc_fix_aspd_post );
	addHookPre( "map->flags_init", map_flags_init_pre );
}