//===== Hercules Plugin ======================================
//= OnPCUseSkillEvent
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.4
//===== Compatible With: ===================================== 
//= Hercules 2019-03-25
//===== Description: =========================================
//= all it does is give a cursor, and return information of the target GID or coordinate
//= BUT you can do a hell lots of stuffs with any script command available
//===== Topic ================================================
//= http://herc.ws/board/topic/11296-onpcuseskillevent/
//===== Additional Comments: =================================
//= there are so many ' skill_[a-z_]*_unknown\(' function in skill.c  O.O
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "map/skill.h"
#include "common/conf.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"OnPCUseSkillEvent",
	SERVER_TYPE_MAP,
	"1.4",
	HPM_VERSION,
};

struct enemyskilldata {
	uint16 skillid;
	char *event;
};
struct friendskilldata {
	uint16 skillid;
	char *event;
};
struct placeskilldata {
	uint16 skillid;
	char *event;
};
struct selfskilldata {
	uint16 skillid;
	char *event;
};

VECTOR_DECL(struct enemyskilldata) enemyskill;
VECTOR_DECL(struct friendskilldata) friendskill;
VECTOR_DECL(struct placeskilldata) placeskill;
VECTOR_DECL(struct selfskilldata) selfskill;

bool skill_castend_damage_id_unknown_pre( struct block_list **src, struct block_list **bl, uint16 **skill_id, uint16 **skill_lv, int64 **tick, int **flag, struct status_data **tstatus, struct status_change **sc ) {
	if ( (*src)->type == BL_PC ) {
		int i = 0;
		struct map_session_data *sd = BL_CAST(BL_PC, *src);
		ARR_FIND( 0, VECTOR_LENGTH(enemyskill), i, **skill_id == VECTOR_INDEX(enemyskill, i).skillid );
		if ( i < VECTOR_LENGTH(enemyskill) ) {
			pc->setreg( sd, script->add_str( "@useskilllv" ), **skill_lv );
			pc->setreg( sd, script->add_str( "@useskilltarget" ), (*bl)->id );
			npc->event( sd, VECTOR_INDEX(enemyskill, i).event, 0 );
			pc->setreg( sd, script->add_str( "@useskilllv" ), 0 );
			pc->setreg( sd, script->add_str( "@useskilltarget" ), 0 );
			hookStop();
		}
	}
	return false;
}

bool skill_castend_nodamage_id_unknown_pre( struct block_list **src, struct block_list **bl, uint16 **skill_id, uint16 **skill_lv, int64 **tick, int **flag ) {
	if ( (*src)->type == BL_PC ) {
		int i = 0;
		struct map_session_data *sd = BL_CAST(BL_PC, *src);
		ARR_FIND( 0, VECTOR_LENGTH(friendskill), i, **skill_id == VECTOR_INDEX(friendskill, i).skillid );
		if ( i < VECTOR_LENGTH(friendskill) ) {
			pc->setreg( sd, script->add_str( "@useskilllv" ), **skill_lv );
			pc->setreg( sd, script->add_str( "@useskilltarget" ), (*bl)->id );
			npc->event( sd, VECTOR_INDEX(friendskill, i).event, 0 );
			pc->setreg( sd, script->add_str( "@useskilllv" ), 0 );
			pc->setreg( sd, script->add_str( "@useskilltarget" ), 0 );
			hookStop();
		}

		ARR_FIND( 0, VECTOR_LENGTH(selfskill), i, **skill_id == VECTOR_INDEX(selfskill, i).skillid );
		if ( i < VECTOR_LENGTH(selfskill) ) {
			pc->setreg( sd, script->add_str( "@useskilllv" ), **skill_lv );
			npc->event( sd, VECTOR_INDEX(selfskill, i).event, 0 );
			pc->setreg( sd, script->add_str( "@useskilllv" ), 0 );
			hookStop();
		}
	}
	return false;
}

bool skill_castend_pos2_unknown_pre( struct block_list **src, int **x, int **y, uint16 **skill_id, uint16 **skill_lv, int64 **tick, int **flag ) {
	if ( (*src)->type == BL_PC ) {
		int i = 0;
		struct map_session_data *sd = BL_CAST(BL_PC, *src);

		ARR_FIND( 0, VECTOR_LENGTH(placeskill), i, **skill_id == VECTOR_INDEX(placeskill, i).skillid );
		if ( i < VECTOR_LENGTH(placeskill) ) {
			pc->setreg( sd, script->add_str( "@useskilllv" ), **skill_lv );
			pc->setreg( sd, script->add_str( "@useskillx" ), **x );
			pc->setreg( sd, script->add_str( "@useskilly" ), **y );
			npc->event( sd, VECTOR_INDEX(placeskill, i).event, 0 );
			pc->setreg( sd, script->add_str( "@useskilllv" ), 0 );
			pc->setreg( sd, script->add_str( "@useskillx" ), 0 );
			pc->setreg( sd, script->add_str( "@useskilly" ), 0 );
			hookStop();
		}

	}
	return false;
}

void clean_skillevent(void) {
	int i;
	for ( i = 0; i < VECTOR_LENGTH(enemyskill); ++i )
		aFree( VECTOR_INDEX(enemyskill, i).event );
	VECTOR_CLEAR(enemyskill);

	for ( i = 0; i < VECTOR_LENGTH(friendskill); ++i )
		aFree( VECTOR_INDEX(friendskill, i).event );
	VECTOR_CLEAR(friendskill);

	for ( i = 0; i < VECTOR_LENGTH(placeskill); ++i )
		aFree( VECTOR_INDEX(placeskill, i).event );
	VECTOR_CLEAR(placeskill);

	for ( i = 0; i < VECTOR_LENGTH(selfskill); ++i )
		aFree( VECTOR_INDEX(selfskill, i).event );
	VECTOR_CLEAR(selfskill);
	return;
}

void read_skillevent(void) {
	const char *confpath = "conf/import/OnPCUseSkillEvent.conf";
	struct config_t OnPCUseSkillEvent_conf;
	if ( !libconfig->load_file( &OnPCUseSkillEvent_conf, confpath ) ) {
		ShowError( "OnPCUseSkillEvent: can't read %s, file not found !\n", confpath );
		return;
	}

	struct config_setting_t *config_db = libconfig->setting_get_member( OnPCUseSkillEvent_conf.root, "OnPCUseSkillEvent" );
	if ( config_db == NULL ) {
		ShowError( "OnPCUseSkillEvent: can't read %s\n", confpath );
		libconfig->destroy( &OnPCUseSkillEvent_conf );
		return;
	}

	struct config_setting_t *config = NULL;
	int i = 0, skillid = 0, flag = 0;
	const char *event = NULL, *skillname = NULL;
	VECTOR_INIT(enemyskill);
	VECTOR_INIT(friendskill);
	VECTOR_INIT(placeskill);
	VECTOR_INIT(selfskill);
	while (( config = libconfig->setting_get_elem( config_db, i++ ) )) {
		skillid = 0;
		skillname = NULL;
		if ( !libconfig->setting_lookup_int( config, "SkillID", &skillid ) && !libconfig->setting_lookup_string( config, "SkillName", &skillname ) ) {
			ShowWarning( "OnPCUseSkillEvent: Missing 'SkillID' or 'SkillName' on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", i, confpath );
			continue;
		}
		if ( !libconfig->setting_lookup_string( config, "Event_Label", &event ) ) {
			ShowWarning( "OnPCUseSkillEvent: Missing 'Event_Label' on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", i, confpath );
			continue;
		}

		if ( skillid > 0 && skill->get_index(skillid) == 0 ) {
			ShowWarning( "OnPCUseSkillEvent: Unknown skill ID "CL_WHITE"%d"CL_RESET" on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", skillid, i, confpath );
			continue;
		}
		if ( skillname != NULL ) {
			skillid = skill->name2id(skillname);
			if ( skillid == 0 ) {
				ShowWarning( "OnPCUseSkillEvent: Unknown skill name "CL_WHITE"%s"CL_RESET" on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", skillname, i, confpath );
				continue;
			}
		}
		else if ( skillid > 0 )
			skillname = skill->get_name(skillid);

		struct event_data* ev = (struct event_data*)strdb_get(npc->ev_db, event);
		if ( ev == NULL || ev->nd == NULL ) {
			ShowWarning( "OnPCUseSkillEvent: NPC label "CL_WHITE"%s"CL_RESET" does not found on skill "CL_WHITE"%s(%d)"CL_RESET" entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", event, skillname, skillid, i, confpath );
			continue;
		}

		flag = skill->get_inf(skillid);
//		ShowDebug( "Loaded skill = %d, flag = %d, event = %s\n", skillid, flag, event );
		switch(flag) {
		case INF_ATTACK_SKILL:
			struct enemyskilldata enemyskill_;
			enemyskill_.skillid = skillid;
			enemyskill_.event = aStrdup(event);
			VECTOR_ENSURE( enemyskill, 1, 1 );
			VECTOR_PUSH( enemyskill, enemyskill_ );
			break;
		case INF_SUPPORT_SKILL:
			struct friendskilldata friendskill_;
			friendskill_.skillid = skillid;
			friendskill_.event = aStrdup(event);
			VECTOR_ENSURE( friendskill, 1, 1 );
			VECTOR_PUSH( friendskill, friendskill_ );
			break;
		case INF_GROUND_SKILL:
			struct placeskilldata placeskill_;
			placeskill_.skillid = skillid;
			placeskill_.event = aStrdup(event);
			VECTOR_ENSURE( placeskill, 1, 1 );
			VECTOR_PUSH( placeskill, placeskill_ );
			break;
		case INF_SELF_SKILL:
			struct selfskilldata selfskill_;
			selfskill_.skillid = skillid;
			selfskill_.event = aStrdup(event);
			VECTOR_ENSURE( selfskill, 1, 1 );
			VECTOR_PUSH( selfskill, selfskill_ );
			break;
		default:
			ShowWarning( "OnPCUseSkillEvent: Unsupport inf type "CL_WHITE"%d"CL_RESET" on skill ID "CL_WHITE"%d"CL_RESET" entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", flag, skillid, i, confpath );
		}
	}

//	ShowDebug( "enemy : %d\n", VECTOR_LENGTH(enemyskill) );
//	for ( i = 0; i < VECTOR_LENGTH(enemyskill); ++i )
//		ShowDebug( "%d : %s\n", VECTOR_INDEX(enemyskill, i).skillid, VECTOR_INDEX(enemyskill, i).event );
//	ShowDebug( "friend : %d\n", VECTOR_LENGTH(friendskill) );
//	for ( i = 0; i < VECTOR_LENGTH(friendskill); ++i )
//		ShowDebug( "%d : %s\n", VECTOR_INDEX(friendskill, i).skillid, VECTOR_INDEX(friendskill, i).event );
//	ShowDebug( "place : %d\n", VECTOR_LENGTH(placeskill) );
//	for ( i = 0; i < VECTOR_LENGTH(placeskill); ++i )
//		ShowDebug( "%d : %s\n", VECTOR_INDEX(placeskill, i).skillid, VECTOR_INDEX(placeskill, i).event );
//	ShowDebug( "self : %d\n", VECTOR_LENGTH(selfskill) );
//	for ( i = 0; i < VECTOR_LENGTH(selfskill); ++i )
//		ShowDebug( "%d : %s\n", VECTOR_INDEX(selfskill, i).skillid, VECTOR_INDEX(selfskill, i).event );

	libconfig->destroy( &OnPCUseSkillEvent_conf );
	ShowStatus( "Done reading '"CL_WHITE"%d"CL_RESET"' entries in '"CL_WHITE"%s"CL_RESET"'.\n", VECTOR_LENGTH(enemyskill) + VECTOR_LENGTH(friendskill) + VECTOR_LENGTH(placeskill) + VECTOR_LENGTH(selfskill), confpath );
	return;
}

void skill_reload_post( void ) {
	clean_skillevent();
	read_skillevent();
	return;
}

HPExport void plugin_init( void ) {
	addHookPre( skill, castend_damage_id_unknown, skill_castend_damage_id_unknown_pre );
	addHookPre( skill, castend_nodamage_id_unknown, skill_castend_nodamage_id_unknown_pre );
	addHookPre( skill, castend_pos2_unknown, skill_castend_pos2_unknown_pre );
	addHookPost( skill, reload, skill_reload_post );
}

HPExport void server_online( void ) {
	clean_skillevent();
	read_skillevent();
}

HPExport void plugin_final( void ) {
	clean_skillevent();
}
