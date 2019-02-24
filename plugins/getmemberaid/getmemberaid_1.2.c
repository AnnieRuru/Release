//===== Hercules Plugin ======================================
//= *getmemberaid Script Command
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.2
//===== Compatible With: ===================================== 
//= Hercules 2014-09-05
//===== Description: =========================================
//= *getmemberaid
//= supported TYPES are :-
//= - ALL_CLIENT
//= - ALL_SAMEMAP
//= - AREA
//= - PARTY
//= - GUILD
//= - BG
//= *checkmes
//===== Topic ================================================
//= http://hercules.ws/board/topic/4593-
//===== Additional Comments: =================================  
//= getmemberaid ALL_CLIENT;
//= getmemberaid ALL_SAMEMAP;
//= getmemberaid ALL_SAMEMAP, "<map name>";
//= getmemberaid AREA;
//= getmemberaid AREA, "<map name>", x1, y1, x2, y2;
//= getmemberaid PARTY;
//= getmemberaid PARTY, <party ID>;
//= getmemberaid GUILD;
//= getmemberaid GUILD, <guild ID>;
//= getmemberaid BG;
//= getmemberaid BG, <battleground ID>;
//= checkmes <account_id>;
//============================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../common/HPMi.h"
#include "../map/script.h"
#include "../map/pc.h"
#include "../map/party.h"
#include "../map/guild.h"
#include "../map/mapreg.h"
#include "../map/battleground.h"

#include "../common/HPMDataCheck.h" // should always be the last file included! (if you don't make it last, it'll intentionally break compile time)

HPExport struct hplugin_info pinfo = {
	"getmemberaid",		// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

int buildin_getmemberaid_ALL_pc(struct map_session_data* sd, va_list ap) {
	uint16 *i = va_arg( ap, uint16 * );
	mapreg->setreg( reference_uid( script->add_str("$@onlineaid"), *i ), sd->status.account_id );
	(*i)++;
	return true;
}

int buildin_getmemberaid_TYPE_bl(struct block_list *bl, va_list ap) {
	TBL_PC *sd = BL_CAST( BL_PC, bl );
	uint16 *i = va_arg( ap, uint16 * );
	mapreg->setreg( reference_uid( script->add_str("$@onlineaid"), *i ), sd->status.account_id );
	(*i)++;
	return true;
}

BUILDIN(getmemberaid) {
	uint16 i = 0;
	switch ( script_getnum(st,2) ) { // type
	case ALL_CLIENT:
		map->foreachpc( buildin_getmemberaid_ALL_pc, &i );
		mapreg->setreg( script->add_str("$@onlinecount"), i );
		return true;
	case ALL_SAMEMAP:
		{
			int16 m;
			if ( script_hasdata(st,3) ) {
				if ( ( m = map->mapname2mapid( script_getstr(st,3) ) ) < 0 ) {
					ShowError( "buildin_getmemberaid: using type ALL_SAMEMAP but map not found !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
			}
			else {
				TBL_PC *sd = script->rid2sd(st);
				if ( !sd ) {
					ShowError( "buildin_getmemberaid: using type ALL_SAMEMAP but no map given and no rid attached !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
				m = sd->bl.m;
			}
			map->foreachinmap( buildin_getmemberaid_TYPE_bl, m, BL_PC, &i );
			mapreg->setreg( script->add_str("$@onlinecount"), i );
			return true;
		}
	case AREA:
		if ( script_hasdata(st,3) ) {
			if ( script_hasdata(st,4) && script_hasdata(st,5) && script_hasdata(st,6) && script_hasdata(st,7) ) {
				int16 m;
				if ( ( m = map->mapname2mapid( script_getstr(st,3) ) ) < 0 ) {
					ShowError( "buildin_getmemberaid: using type AREA but map not found !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
				map->foreachinarea( buildin_getmemberaid_TYPE_bl, m, script_getnum(st,4), script_getnum(st,5), script_getnum(st,6), script_getnum(st,7), BL_PC, &i );
				mapreg->setreg( script->add_str("$@onlinecount"), i );
				return true;
			}
			else {
				ShowError( "buildin_getmemberaid: using type AREA but not enough arguments given !\n" );
				mapreg->setreg( script->add_str("$@onlinecount"), 0 );
				return false;
			}
		}
		else {
			TBL_PC *sd = script->rid2sd(st);
			if ( !sd ) {
				ShowError( "buildin_getmemberaid: using type AREA but no coordinate given and no rid attached !\n" );
				mapreg->setreg( script->add_str("$@onlinecount"), 0 );
				return false;
			}
			else {
				map->foreachinrange( buildin_getmemberaid_TYPE_bl, &sd->bl, battle->bc->area_size, BL_PC, &i );
				mapreg->setreg( script->add_str("$@onlinecount"), i );
				return true;
			}
		}
	case PARTY:
		{
			struct party_data *p;
			uint16 j = 0;
			if ( script_hasdata(st,3) ) {
				p = party->search( script_getnum(st,3) );
				if ( !p ) {
					ShowError( "buildin_getmemberaid: using type PARTY but party ID not found !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
			}
			else {
				TBL_PC *sd = script->rid2sd(st);
				if ( !sd ) {
					ShowError( "buildin_getmemberaid: using type PARTY but no party ID given and no rid attached !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
				else if ( sd->status.party_id == 0 ) {
					ShowError( "buildin_getmemberaid: using type PARTY but RID attached has no party ID !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
				p = party->search( sd->status.party_id );
			}
			for ( i = 0; i < MAX_PARTY; i++ ) {
				if ( p->party.member[i].account_id && p->party.member[i].online ) {
					mapreg->setreg( reference_uid( script->add_str("$@onlineaid"), i ), p->party.member[i].account_id );
					j++;
				}
			}
			mapreg->setreg( script->add_str("$@onlinecount"), j );
			return true;
		}
	case GUILD:
		{
			struct guild *g;
			uint16 j = 0;
			if ( script_hasdata(st,3) ) {
				g = guild->search( script_getnum(st,3) );
				if ( !g ) {
					ShowError( "buildin_getmemberaid: using type GUILD but guild ID not found !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
			}
			else {
				TBL_PC *sd = script->rid2sd(st);
				if ( !sd ) {
					ShowError( "buildin_getmemberaid: using type GUILD but no guild ID given and no rid attached !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
				else if ( sd->status.guild_id == 0 ) {
					ShowError( "buildin_getmemberaid: using type GUILD but RID attached has no guild ID !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
				g = guild->search( sd->status.guild_id );
			}
			for ( i = 0; i < MAX_GUILD; i++ ) {
				if ( g->member[i].account_id && g->member[i].online ) {
					mapreg->setreg( reference_uid( script->add_str("$@onlineaid"), i ), g->member[i].account_id );
					j++;
				}
			}
			mapreg->setreg( script->add_str("$@onlinecount"), j );
			return true;
		}
	case BG:
		{
			struct battleground_data *bgd;
			uint16 j = 0;
			if ( script_hasdata(st,3) ) {
				bgd = bg->team_search( script_getnum(st,3) );
				if ( !bgd ) {
					ShowError( "buildin_getmemberaid: using type BG but battleground ID not found !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
			}
			else {
				TBL_PC *sd = script->rid2sd(st);
				if ( !sd ) {
					ShowError( "buildin_getmemberaid: using type BG but no battleground ID given and no rid attached !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
				else if ( sd->bg_id == 0 ) {
					ShowError( "buildin_getmemberaid: using type BG but RID attached has no battleground ID !\n" );
					mapreg->setreg( script->add_str("$@onlinecount"), 0 );
					return false;
				}
				bgd = bg->team_search( sd->bg_id );
			}
			for ( i = 0; i < MAX_BG_MEMBERS; i++ ) {
				if ( bgd->members[i].sd ) {
					mapreg->setreg( reference_uid( script->add_str("$@onlineaid"), j ), bgd->members[i].sd->bl.id );
					j++;
				}
			}
			mapreg->setreg( script->add_str("$@onlinecount"), j );
			return true;
		}
	default:
		ShowError( "buildin_getmemberaid: invalid type !\n" );
		mapreg->setreg( script->add_str("$@onlinecount"), 0 );
		return false;
	}
	return true;
}

BUILDIN(checkmes) {
	TBL_PC *sd = map->id2sd( script_getnum(st,2) );
	if ( sd ) {
		script_pushint(st, ( sd->npc_id || sd->npc_shopid )? 1 : 0 );
		return true;
	}
	else {
		script_pushint(st, 0);
		ShowWarning( "buildin_checkmes: invalid player ID or not online !\n" );
		return false;
	}
}

HPExport void plugin_init (void) {
	script = GET_SYMBOL("script");
	party = GET_SYMBOL("party");
	guild = GET_SYMBOL("guild");
	mapreg = GET_SYMBOL("mapreg");
	bg = GET_SYMBOL("battlegrounds");
	map = GET_SYMBOL("map");
	battle = GET_SYMBOL("battle");

	addScriptCommand( "getmemberaid", "i?????", getmemberaid );
	addScriptCommand( "checkmes", "i", checkmes );
}