//===== Hercules Plugin ======================================
//= OnPCUseSkillEvent
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.5
//===== Compatible With: ===================================== 
//= Hercules 2019-03-26
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
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"OnPCUseSkillEvent",
	SERVER_TYPE_MAP,
	"1.5",
	HPM_VERSION,
};

struct enemyskilldata {
	uint16 skill_id;
	char *event;
};
struct friendskilldata {
	uint16 skill_id;
	char *event;
};
struct placeskilldata {
	uint16 skill_id;
	char *event;
};
struct selfskilldata {
	uint16 skill_id;
	char *event;
};
struct tempskilldata { // this stupid temporary struct because skill_validate_additional_fields return everything empty -_-"
	uint16 skill_id;
	char *event;
};

VECTOR_DECL(struct enemyskilldata) enemyskill;
VECTOR_DECL(struct friendskilldata) friendskill;
VECTOR_DECL(struct placeskilldata) placeskill;
VECTOR_DECL(struct selfskilldata) selfskill;
VECTOR_DECL(struct tempskilldata) tempskill;

bool skill_castend_damage_id_unknown_pre( struct block_list **src, struct block_list **bl, uint16 **skill_id, uint16 **skill_lv, int64 **tick, int **flag, struct status_data **tstatus, struct status_change **sc ) {
	if ( (*src)->type == BL_PC ) {
		int i = 0, size = VECTOR_LENGTH(enemyskill);
		ARR_FIND( 0, size, i, **skill_id == VECTOR_INDEX(enemyskill, i).skill_id );
		if ( i < size ) {
			struct map_session_data *sd = BL_CAST(BL_PC, *src);
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
		int i = 0, size = VECTOR_LENGTH(friendskill);
		ARR_FIND( 0, size, i, **skill_id == VECTOR_INDEX(friendskill, i).skill_id );
		if ( i < size ) {
			struct map_session_data *sd = BL_CAST(BL_PC, *src);
			pc->setreg( sd, script->add_str( "@useskilllv" ), **skill_lv );
			pc->setreg( sd, script->add_str( "@useskilltarget" ), (*bl)->id );
			npc->event( sd, VECTOR_INDEX(friendskill, i).event, 0 );
			pc->setreg( sd, script->add_str( "@useskilllv" ), 0 );
			pc->setreg( sd, script->add_str( "@useskilltarget" ), 0 );
			hookStop();
		}
		size = VECTOR_LENGTH(selfskill);
		ARR_FIND( 0, size, i, **skill_id == VECTOR_INDEX(selfskill, i).skill_id );
		if ( i < size ) {
			struct map_session_data *sd = BL_CAST(BL_PC, *src);
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
		int i = 0, size = VECTOR_LENGTH(placeskill);
		ARR_FIND( 0, size, i, **skill_id == VECTOR_INDEX(placeskill, i).skill_id );
		if ( i < size ) {
			struct map_session_data *sd = BL_CAST(BL_PC, *src);
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

void skill_validate_additional_fields_post( struct config_setting_t *conf, struct s_skill_db *sk ) {
	nullpo_retv(sk);
	const char *event = NULL;

	if ( !libconfig->setting_lookup_string( conf, "Event_Label", &event ) )
		return;

	struct tempskilldata tempskill_;
	tempskill_.skill_id = sk->nameid;
	tempskill_.event = aStrdup(event);
	VECTOR_ENSURE( tempskill, 1, 1 );
	VECTOR_PUSH( tempskill, tempskill_ );
	return;
}

void init_skillevent(void) {
	VECTOR_INIT(enemyskill);
	VECTOR_INIT(friendskill);
	VECTOR_INIT(placeskill);
	VECTOR_INIT(selfskill);
	VECTOR_INIT(tempskill);
	return;
}

void read_skillevent(void) {
	int i, size = VECTOR_LENGTH(tempskill), skill_id, flag;
	for ( i = 0; i < size; ++i ) {
		skill_id = VECTOR_INDEX(tempskill, i).skill_id;
		struct event_data *ev = (struct event_data*)strdb_get( npc->ev_db, VECTOR_INDEX(tempskill, i).event );
		if ( ev == NULL || ev->nd == NULL ) {
			ShowWarning( "skill_readdb: NPC label "CL_WHITE"%s"CL_RESET" does not found on skill "CL_WHITE"%s(%d)"CL_RESET".\n", VECTOR_INDEX(tempskill, i).event, skill->get_name(skill_id), skill_id );
			continue;
		}

		flag = skill->get_inf(skill_id);
		switch( flag ) {
		case INF_ATTACK_SKILL:
			struct enemyskilldata enemyskill_;
			enemyskill_.skill_id = skill_id;
			enemyskill_.event = aStrdup(VECTOR_INDEX(tempskill, i).event);
			VECTOR_ENSURE( enemyskill, 1, 1 );
			VECTOR_PUSH( enemyskill, enemyskill_ );
			break;
		case INF_SUPPORT_SKILL:
			struct friendskilldata friendskill_;
			friendskill_.skill_id = skill_id;
			friendskill_.event = aStrdup(VECTOR_INDEX(tempskill, i).event);
			VECTOR_ENSURE( friendskill, 1, 1 );
			VECTOR_PUSH( friendskill, friendskill_ );
			break;
		case INF_GROUND_SKILL:
			struct placeskilldata placeskill_;
			placeskill_.skill_id = skill_id;
			placeskill_.event = aStrdup(VECTOR_INDEX(tempskill, i).event);
			VECTOR_ENSURE( placeskill, 1, 1 );
			VECTOR_PUSH( placeskill, placeskill_ );
			break;
		case INF_SELF_SKILL:
			struct selfskilldata selfskill_;
			selfskill_.skill_id = skill_id;
			selfskill_.event = aStrdup(VECTOR_INDEX(tempskill, i).event);
			VECTOR_ENSURE( selfskill, 1, 1 );
			VECTOR_PUSH( selfskill, selfskill_ );
			break;
		default:
			ShowWarning( "skill_readdb: Skill has Event_Label but unsupport inf type "CL_WHITE"%d"CL_RESET" on skill "CL_WHITE"%s(%d)"CL_RESET".\n", flag, skill->get_name(skill_id), skill_id );
			continue;
		}
	}

	for ( i = 0; i < size; ++i )
		aFree( VECTOR_INDEX(tempskill, i).event );
	VECTOR_CLEAR(tempskill);

//	ShowDebug( "enemy : %d\n", VECTOR_LENGTH(enemyskill) );
//	for ( i = 0; i < VECTOR_LENGTH(enemyskill); ++i )
//		ShowDebug( "%d : %s\n", VECTOR_INDEX(enemyskill, i).skill_id, VECTOR_INDEX(enemyskill, i).event );
//	ShowDebug( "friend : %d\n", VECTOR_LENGTH(friendskill) );
//	for ( i = 0; i < VECTOR_LENGTH(friendskill); ++i )
//		ShowDebug( "%d : %s\n", VECTOR_INDEX(friendskill, i).skill_id, VECTOR_INDEX(friendskill, i).event );
//	ShowDebug( "place : %d\n", VECTOR_LENGTH(placeskill) );
//	for ( i = 0; i < VECTOR_LENGTH(placeskill); ++i )
//		ShowDebug( "%d : %s\n", VECTOR_INDEX(placeskill, i).skill_id, VECTOR_INDEX(placeskill, i).event );
//	ShowDebug( "self : %d\n", VECTOR_LENGTH(selfskill) );
//	for ( i = 0; i < VECTOR_LENGTH(selfskill); ++i )
//		ShowDebug( "%d : %s\n", VECTOR_INDEX(selfskill, i).skill_id, VECTOR_INDEX(selfskill, i).event );

	ShowStatus( "Done reading '"CL_WHITE"%d"CL_RESET"' entries for '"CL_WHITE"OnPCUseSkillEvent"CL_RESET"'.\n", VECTOR_LENGTH(enemyskill) + VECTOR_LENGTH(friendskill) + VECTOR_LENGTH(placeskill) + VECTOR_LENGTH(selfskill) );
	return;
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

void skill_reload_pre( void ) {
	clean_skillevent();
	init_skillevent();
	return;
}

void skill_reload_post( void ) {
	read_skillevent();
	return;
}
	
HPExport void plugin_init( void ) {
	addHookPre( skill, castend_damage_id_unknown, skill_castend_damage_id_unknown_pre );
	addHookPre( skill, castend_nodamage_id_unknown, skill_castend_nodamage_id_unknown_pre );
	addHookPre( skill, castend_pos2_unknown, skill_castend_pos2_unknown_pre );
	addHookPost( skill, validate_additional_fields, skill_validate_additional_fields_post );
	addHookPre( skill, reload, skill_reload_pre );
	addHookPost( skill, reload, skill_reload_post );
	init_skillevent();
}

HPExport void server_online( void ) {
	read_skillevent();
}

HPExport void plugin_final( void ) {
	clean_skillevent();
}
