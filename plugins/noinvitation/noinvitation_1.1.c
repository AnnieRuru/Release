//===== Hercules Plugin ======================================
//= noinvitation mapflag
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.1
//===== Compatible With: ===================================== 
//= Hercules 2019-03-24
//===== Description: =========================================
//= block players from invite friends
//===== Topic ================================================
//= http://herc.ws/board/topic/11553-noinvitation-mapflag/
//===== Additional Comments: =================================  
//= this mapflag only blocks friend invitation
//= to block others like trade/party/guild invitation
//= use mf_notrade, mf_partylock, mf_guildlock
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "map/map.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"noinvitation",
	SERVER_TYPE_MAP,
	"1.1",
	HPM_VERSION,
};

struct mapflag_data {
	unsigned noinvitation : 1;
	int gm_level_bypass;
};

void npc_parse_unknown_mapflag_pre( const char **name, const char **w3, const char **w4, const char **start, const char **buffer, const char **filepath, int **retval ) {
	if ( strcmp( *w3, "noinvitation" ) )
		return;
	int16 m = map->mapname2mapid( *name );
	struct mapflag_data *mf = getFromMAPD( &map->list[m], 0 );
	int bypass_level = atoi(*w4);
	if ( bypass_level == 0 )
		bypass_level = 100;
	if ( mf == NULL ) {
		CREATE( mf, struct mapflag_data, 1 );
		mf->noinvitation = true;
		mf->gm_level_bypass = bypass_level;
		addToMAPD( &map->list[m], mf, 0, true );
	}
	else {
		mf->noinvitation = true;
		mf->gm_level_bypass = bypass_level;
	}
	hookStop();
	return;
}

void clif_friendlist_req_pre( struct map_session_data **sd, int *account_id, int *char_id, const char **name ) {
	struct map_session_data *tsd = map->id2sd( *account_id );
	if ( tsd == NULL )
		return;
	struct mapflag_data *mf = getFromMAPD( &map->list[tsd->bl.m], 0 );
	if ( mf && mf->noinvitation == true && pc_get_group_level(tsd) < mf->gm_level_bypass ) {
		clif->messagecolor_self( tsd->fd, COLOR_RED, "You can't invite friends in this map." );
		hookStop();
	}
	return;
}

//	flush all noinvitation mapflag back to default upon @reloadscript
void map_flags_init_pre(void) {
	int i;
	for ( i = 0; i < map->count; ++i ) {
		struct mapflag_data *mf = getFromMAPD( &map->list[i], 0 );
		if ( mf != NULL )
			removeFromMAPD( &map->list[i], 0 );
	}
	return;
}

HPExport void plugin_init (void) {
	addHookPre( npc, parse_unknown_mapflag, npc_parse_unknown_mapflag_pre );
	addHookPre( clif, friendlist_req, clif_friendlist_req_pre );
	addHookPre( map, flags_init, map_flags_init_pre );
}
