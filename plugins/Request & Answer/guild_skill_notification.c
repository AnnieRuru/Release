//===== Hercules Plugin ======================================
//= Guild Skill Notification
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1a
//===== Compatible With: ===================================== 
//= Hercules 2019-03-28
//===== Description: =========================================
//= tell the guild master the guild skill is ready
//===== Topic ================================================
//= http://herc.ws/board/topic/16662-guild-skill-notification/
//===== Additional Comments: =================================  
//= a lot of surprises for a 'simple' modification <_<
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "map/guild.h"
#include "map/mapreg.h"
#include "common/timer.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"guild_skill_notification",
	SERVER_TYPE_MAP,
	"0.1a",
	HPM_VERSION,
};

int timetick = 0; // this stupid variable is needed to prevent message display multiple times

int skill_blockpc_end_pre( int *tid, int64 *tick, int *id, intptr_t *data ) {
	struct map_session_data *sd = map->id2sd(*id);
	if ( *data <= 0 || *data >= MAX_SKILL_DB || sd == NULL || sd->blockskill[*data] == 0 ) {
		hookStop();
		return 0;
	}
	struct skill_cd *cd = idb_get( skill->cd_db, sd->status.char_id );
	int i;
	for ( i = 0; i < cd->cursor; ++i ) {
		if ( cd->entry[i] && cd->entry[i]->skill_id == GD_EMERGENCYCALL ) {
			if ( timetick == (int)time(NULL) )
				break;
			struct guild *g = guild->search( sd->status.guild_id );
			int j;
			for ( j = 0; j < MAX_GUILD; ++j ) {
				struct map_session_data *guild_sd = map->id2sd( g->member[j].account_id );
				if ( guild_sd == NULL )
					continue;
				clif->message( guild_sd->fd, "Guild Skill is ready." );
			}
			timetick = (int)time(NULL);
			break;
		}
	}
	return 1;
}

int skillnotok_post( int retVal, uint16 skill_id, struct map_session_data *sd ) {
	if ( sd == NULL )
		return retVal;
	if ( retVal == 0 )
		return 0;
	switch ( skill_id ) {
	case GD_BATTLEORDER:
	case GD_REGENERATION:
	case GD_RESTORE:
	case GD_EMERGENCYCALL:
		break;
	default:
		return 1;
	}
	struct skill_cd *cd = idb_get( skill->cd_db, sd->status.char_id );
	if ( cd == NULL )
		return 1;
	int i;
	for ( i = 0; i < cd->cursor; ++i ) {
		if ( cd->entry[i] && cd->entry[i]->skill_id == GD_EMERGENCYCALL ) {
			char msg[CHAT_SIZE_MAX];
			int cooldown = DIFF_TICK32( timer->gettick(), cd->entry[i]->started );
			int timeleft = ( ( cd->entry[i]->duration - cooldown ) / 1000 )+1;
			if ( timeleft >= 60 ) {
				int minutes = timeleft / 60;
				int seconds = timeleft % 60;
				safesnprintf( msg, CHAT_SIZE_MAX, "You must wait %d min %d sec.", minutes, seconds );
			}
			else
				safesnprintf( msg, CHAT_SIZE_MAX, "You must wait %d sec.", timeleft );
			clif->message( sd->fd, msg );
			break;
		}
	}
	return 1;
}

HPExport void server_online( void ) {
	addHookPre( skill, blockpc_end, skill_blockpc_end_pre );
	addHookPost( skill, not_ok, skillnotok_post );
}
