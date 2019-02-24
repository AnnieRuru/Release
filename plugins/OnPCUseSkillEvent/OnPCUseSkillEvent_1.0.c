//===== Hercules Plugin ======================================
//= OnPCUseSkillEvent revamp
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2015-11-29
//===== Description: =========================================
//= all it does is give a cursor, and return information of the target GID or coordinate
//= BUT you can do a hell lots of stuffs with any script command available
//===== Topic ================================================
//= http://herc.ws/board/topic/11296-onpcuseskillevent/
//===== Additional Comments: =================================  
//= I wonder I should add SELF ?
//= and ... there are so many ' skill_[a-z_]*_unknown\(' function in skill.c  O.O
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/pc.h"
#include "map/npc.h"
#include "common/memmgr.h"
#include "common/HPMDataCheck.h"

uint16 enemyskillid[100], friendskillid[100], placeskillid[100];
char **enemyevent = NULL, **friendevent = NULL, **placeevent = NULL;
uint16 enemyskillcount = 0, friendskillcount = 0, placeskillcount = 0;

HPExport struct hplugin_info pinfo = {
	"OnPCUseSkillEvent", // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

bool skill_castend_damage_id_unknown_pre( struct block_list *src, struct block_list *bl, uint16 *skill_id, uint16 *skill_lv, int64 *tick, int *flag, struct status_data *tstatus, struct status_change *sc ) {
	if ( src->type == BL_PC ) {
		int i = 0;
		TBL_PC *sd = BL_CAST(BL_PC, src);
		ARR_FIND( 0, enemyskillcount, i, *skill_id == enemyskillid[i] );
		if ( i < enemyskillcount ) {
			pc->setreg( sd, script->add_str( "@useskilllv" ), *skill_lv );
			pc->setreg( sd, script->add_str( "@useskilltarget" ), bl->id );
		//	pc->setregstr( sd, script->add_str( "@useskilltype$" ), "skill_castend_damage_id_unknown" );
			npc->event( sd, enemyevent[i], 0 );
			hookStop();
		}
	}
	return false;
}

bool skill_castend_nodamage_id_unknown_pre( struct block_list *src, struct block_list *bl, uint16 *skill_id, uint16 *skill_lv, int64 *tick, int *flag ) {
	if ( src->type == BL_PC ) {
		int i = 0;
		TBL_PC *sd = BL_CAST(BL_PC, src);
		ARR_FIND( 0, friendskillcount, i, *skill_id == friendskillid[i] );
		if ( i < friendskillcount ) {
			pc->setreg( sd, script->add_str( "@useskilllv" ), *skill_lv );
			pc->setreg( sd, script->add_str( "@useskilltarget" ), bl->id );
		//	pc->setregstr( sd, script->add_str( "@useskilltype$" ), "skill_castend_nodamage_id_unknown" );
			npc->event( sd, friendevent[i], 0 );
			hookStop();
		}
	}
	return false;
}

bool skill_castend_pos2_unknown_pre(struct block_list *src, int *x, int *y, uint16 *skill_id, uint16 *skill_lv, int64 *tick, int *flag) {
	if ( src->type == BL_PC ) {
		int i = 0;
		TBL_PC *sd = BL_CAST(BL_PC, src);
		ARR_FIND( 0, placeskillcount, i, *skill_id == placeskillid[i] );
		if ( i < placeskillcount ) {
			pc->setreg( sd, script->add_str( "@useskilllv" ), *skill_lv );
			pc->setreg( sd, script->add_str( "@useskillx" ), *x );
			pc->setreg( sd, script->add_str( "@useskilly" ), *y );
		//	pc->setregstr( sd, script->add_str( "@useskilltype$" ), "skill_castend_pos2_unknown" );
			npc->event( sd, placeevent[i], 0 );
			hookStop();
		}
	}
	return false;
}

bool read_skillevent(const char *confpath) {
	FILE* fp = fopen( confpath, "r" );
	if (fp) {
		uint16 skillid = 0, flag = 0, linenumber = 0;
		char line[100], event[50];
		while( fgets( line, sizeof(line), fp ) ) {
			linenumber++;
			if ( ( line[0] == '/' && line[1] == '/' ) || line[0] == '\n' || line[1] == '\n' )
				continue;
			else {
				int i = 0, j = 0, l = strlen( line ) +1;
				char npcname[24], eventname[24];
				char *temp = (char*)aMalloc( strlen( line ) +1 );

				while ( i <= l ) {
					if ( line[i] != ' ' && line[i] != '	' && line[i] != ',' && line[i] != '\0' )
						temp[j++] = line[i];
					else if ( line[i-1] != ' ' && line[i-1] != '	' && line[i-1] != ',' ) {
						temp[j] = '\0';
						break;
					}
					i++;
				}
				if ( atoi(temp) == 0 ) {
					skillid = skill->name2id(temp);
					if ( skillid == 0 ) {
						ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    Unknown skill name '%s'.\n", linenumber, temp );
						aFree( temp );
						continue;
					}
				}
				else {
					skillid = atoi(temp);
					if ( skill->get_index(skillid) == 0 ) {
						ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    Unknown skill ID '%s'.\n", linenumber, skillid );
						aFree( temp );
						continue;
					}
				}

				flag = skill->get_inf(skillid);

				i++;
				if ( line[i] == ' ' || line[i] == '	' || line[i] == ',' )
					i++;

				if ( line[i] != '\"' ) {
					ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    NPC event must start with Quotation Mark.\n", linenumber );
					aFree( temp );
					continue;
				}

				i++;
				j = 0;
				while ( i <= l ) {
					if ( line[i] != ':' && line[i] != '\0' )
						temp[j++] = line[i];
					else {
						temp[j] = '\0';
						break;
					}
					i++;
				}
				if ( strlen(temp) > 23 ) {
					ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    NPC name must not more than 23 characters.\n", linenumber );
					aFree( temp );
					continue;
				}
				{
					struct npc_data *nd = npc->name2id(temp);
					if (!nd) {
						ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    NPC name '%s' does not found.\n", linenumber, temp );
						aFree( temp );
						continue;
					}
				}
				safestrncpy( npcname, temp, 24 );
				if ( line[i+1] != ':' && line[i+2] != ':' ) {
					ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    NPC event format is \"<npc name>::<label name>\".\n", linenumber );
					aFree( temp );
					continue;
				}
				i += 2;

				j = 0;
				while ( i <= l ) {
					if ( line[i] != '\"' && line[i] != '\0' )
						temp[j++] = line[i];
					else {
						temp[j] = '\0';
						break;
					}
					i++;
				}
				if ( strlen(temp) > 23 ) {
					ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    NPC label must not more than 23 characters.\n", linenumber );
					aFree( temp );
					continue;
				}
				if ( temp[0] != 'O' || temp[1] != 'n' ) {
					ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    NPC label must start with 'On'.\n", linenumber );
					aFree( temp );
					continue;
				}
				safestrncpy( eventname, temp, 24 );

				if ( line[i] != '\"' ) {
					ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    NPC event must end with Quotation Mark.\n", linenumber );
					aFree( temp );
					continue;
				}

				safesnprintf( event, 50, "%s::%s", npcname, eventname );
				{
					struct event_data* ev = (struct event_data*)strdb_get(npc->ev_db, event);
					if ( ev == NULL || ev->nd == NULL ) {
						ShowWarning( "OnPCUseSkillEvent: Error in reading line %d.\n    NPC label '%s' does not found.\n", linenumber, eventname );
						aFree( temp );
						continue;
					}
				}
				aFree( temp );
			//	ShowDebug("Loaded skill = %d, flag = %d, event = %s\n", skillid, flag, event );
				if ( flag == 1 ) {
					enemyskillid[enemyskillcount] = skillid;
					RECREATE(enemyevent, char *, ++enemyskillcount);
					enemyevent[enemyskillcount - 1] = aStrdup(event);
				}
				else if ( flag == 16 ) {
					friendskillid[friendskillcount] = skillid;
					RECREATE(friendevent, char *, ++friendskillcount);
					friendevent[friendskillcount - 1] = aStrdup(event);
				}
				else if ( flag == 2 ) {
					placeskillid[placeskillcount] = skillid;
					RECREATE(placeevent, char *, ++placeskillcount);
					placeevent[placeskillcount - 1] = aStrdup(event);
				}
			}
		}
		fclose(fp);
		return true;
	}
	else {
		ShowError( "File not found: '%s'.\n", confpath );
		return false;
	}
}

HPExport void plugin_init (void) {
	addHookPre( "skill->castend_damage_id_unknown", skill_castend_damage_id_unknown_pre );
	addHookPre( "skill->castend_nodamage_id_unknown", skill_castend_nodamage_id_unknown_pre );
	addHookPre( "skill->castend_pos2_unknown", skill_castend_pos2_unknown_pre );
}

HPExport void server_online (void) {
	read_skillevent( "conf/import/OnPCUseSkillEvent.txt" );
}