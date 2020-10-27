//===== Hercules Plugin ======================================
//= fixedaspd mapflag
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.2
//===== Compatible With: ===================================== 
//= Hercules 2019-03-24
//===== Description: =========================================
//= fixed players attack speed on particular map
//===== Topic ================================================
//= http://herc.ws/board/topic/11155-fixedaspd-mapflag/
//===== Additional Comments: =================================  
//= prontera   mapflag   fixedaspd   150
//= setmapflagfixedaspd "prontera", 150;
//= fixed players attack speed at 150 speed at prontera map
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "common/utils.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"fixedaspd",
	SERVER_TYPE_MAP,
	"1.2",
	HPM_VERSION,
};

struct mapflag_data {
	int fixedaspd;
	int gm_level_bypass;
};

void npc_parse_unknown_mapflag_pre( const char **name, const char **w3, const char **w4, const char **start, const char **buffer, const char **filepath, int **retval ) {
	if ( strcmp( *w3, "fixedaspd" ) )
		return;
	int value = 150, bypass_level = 100;
//	if ( sscanf( *w4, "%d\t%d", &value, &bypass_level ) < 2 ) // I'm really fed-up with sscanf now ... can't even differentiate [TAB] or [SPACE] ... never use it again
//		value = atoi(*w4);
	size_t l = strlen(*w4);
	if ( l == 0 ) {
		ShowWarning( "npc_parse_mapflag: Missing 4th param for 'fixedaspd' mapflag !\n    Removing flag at %s (file '%s', line '%d').\n",  *name, *filepath, strline( *buffer, *start - *buffer ) );
		hookStop();
		return;
	}
	char *str = aStrdup(*w4), *temp = (char*)aMalloc( l +1 );
	unsigned short i = 0, j = 0, k = 0;
	while ( i <= l ) {
		if ( str[i] == ' ' )
			ShowWarning( "npc_parse_mapflag: Detected a [SPACE] in 'fixedaspd' mapflag !\n    At %s (file '%s', line '%d').\n",  *name, *filepath, strline( *buffer, *start - *buffer ) );
		else if ( str[i] != '\t' && str[i] != '\0' ) {
			temp[j++] = str[i];
		}
		else if ( str[i-1] != '\t' ) {
			temp[j] = '\0';
			if ( k == 0 )
				value = atoi(temp);
			else {
				bypass_level = atoi(temp);
				break;
			}
			j = 0;
			++k;
		}
		++i;
	}
	value = cap_value( value, 85, 199 );
	bypass_level = cap_value( bypass_level, 0, 100 );
//	ShowDebug( "parse-> %d %d\n", value, bypass_level );
	int16 m = map->mapname2mapid( *name );
	struct mapflag_data *mf = getFromMAPD( &map->list[m], 0 );
	if ( mf == NULL ) {
		CREATE( mf, struct mapflag_data, 1 );
		mf->fixedaspd =  2000 - value *10;
		mf->gm_level_bypass = bypass_level;
		addToMAPD( &map->list[m], mf, 0, true );
	}
	else {
		mf->fixedaspd =  2000 - value *10;
		mf->gm_level_bypass = bypass_level;
	}
//	ShowDebug( "mf-> %d %d\n", mf->fixedaspd, mf->gm_level_bypass );
	aFree(str);
	aFree(temp);
	hookStop();
	return;
}


short status_calc_fix_aspd_post( short retVal, struct block_list *bl, struct status_change *sc, int aspd ) {
	if ( bl->type == BL_PC ) {
		struct map_session_data *sd = BL_CAST( BL_PC, bl );
		struct mapflag_data *mf = getFromMAPD( &map->list[sd->bl.m], 0 );
		if ( mf && mf->fixedaspd && pc_get_group_level(sd) < mf->gm_level_bypass )
			return cap_value( mf->fixedaspd, 0, 2000 );
	}
	return retVal;
}

//	flush all fixedaspd mapflag back to default upon @reloadscript
void map_flags_init_pre(void) {
	int i;
	for ( i = 0; i < map->count; i++ ) {
		struct mapflag_data *mf = getFromMAPD( &map->list[i], 0 );
		if ( mf != NULL )
			removeFromMAPD( &map->list[i], 0 );
	}
	return;
}

BUILDIN(setmapflagfixedaspd) {
	int16 m = map->mapname2mapid( script_getstr(st,2) );
	if ( m < 0 ) // already has a debug message
		return false;

	int value = script_getnum(st,3), bypass_level = 100;
	if ( script_hasdata(st,4) )
		bypass_level = script_getnum(st,4);

	value = cap_value( value, 85, 199 );
	bypass_level = cap_value( bypass_level, 0, 100 );

	struct mapflag_data *mf = getFromMAPD( &map->list[m], 0 );
	if ( mf == NULL ) {
		CREATE( mf, struct mapflag_data, 1 );
		mf->fixedaspd =  2000 - value *10;
		mf->gm_level_bypass = bypass_level;
		addToMAPD( &map->list[m], mf, 0, true );
	}
	else {
		mf->fixedaspd =  2000 - value *10;
		mf->gm_level_bypass = bypass_level;
	}

	return true;
}

BUILDIN(removemapflagfixedaspd) {
	int16 m = map->mapname2mapid( script_getstr(st,2) );
	if ( m < 0 ) // already has a debug message
		return false;

	struct mapflag_data *mf = getFromMAPD( &map->list[m], 0 );
	if ( mf != NULL )
		removeFromMAPD( &map->list[m], 0 );

	return true;
}

BUILDIN(getmapflagfixedaspd) {
	int16 m = map->mapname2mapid( script_getstr(st,2) );
	if ( m < 0 ) // already has a debug message
		return false;

	int type = 0;
	if ( script_hasdata(st,3) )
		type = script_getnum(st,3) > 0;

	struct mapflag_data *mf = getFromMAPD( &map->list[m], 0 );
	if ( mf == NULL ) {
		script_pushint( st, 0 );
		return true;
	}

	if ( type == 0 )
		script_pushint( st, mf->fixedaspd );
	else
		script_pushint( st, mf->gm_level_bypass );

	return true;
}

HPExport void plugin_init (void) {
	addHookPre( npc, parse_unknown_mapflag, npc_parse_unknown_mapflag_pre );
	addHookPost( status, calc_fix_aspd, status_calc_fix_aspd_post );
	addHookPre( map, flags_init, map_flags_init_pre );
	addScriptCommand( "setmapflagfixedaspd", "si?", setmapflagfixedaspd );
	addScriptCommand( "removemapflagfixedaspd", "s", removemapflagfixedaspd );
	addScriptCommand( "getmapflagfixedaspd", "s?", getmapflagfixedaspd );
}
