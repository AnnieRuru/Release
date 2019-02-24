//===== Hercules Plugin ======================================
//= spam_help
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-02-16
//===== Description: =========================================
//= make the player automatically spam help chat and emoticon
//===== Topic ================================================
//= https://rathena.org/board/topic/117966-how-to-find-nearest-enemyobtain-a-list-of-units-on-the-same-map-etc/?do=findComment&comment=357634
//===== Additional Comments: =================================  
//= it has a cooldown of 10 seconds, so it wont spam repeatedly
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/clif.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"spam_help",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

struct player_data {
	int spam_help;
};

static bool pc_authok_post( bool retVal, struct map_session_data *sd, int login_id2, time_t expiration_time, int group_id, const struct mmo_charstatus *st, bool changing_mapservers ) {
	struct player_data *ssd;
	if ( retVal == false )
		return false;
	CREATE( ssd, struct player_data, 1 );
	ssd->spam_help = (int)time(NULL);
	addToMSD( sd, ssd, 0, true );
	return retVal;
}

static void pc_damage_post( struct map_session_data *sd, struct block_list *src, unsigned int hp, unsigned int sp ) {
	struct player_data *ssd = getFromMSD(sd,0);
	if ( pc->readparam( sd, SP_HP ) * 100 / pc->readparam( sd, SP_MAXHP ) < 25 && ssd->spam_help + 10 <= (int)time(NULL) ) {
		clif->disp_overhead( &sd->bl, "I need Healing !!", AREA_CHAT_WOC, NULL );
		clif->emotion( &sd->bl, E_HLP );
		ssd->spam_help = (int)time(NULL);
	}
}

HPExport void plugin_init (void) {
	addHookPost( pc, authok, pc_authok_post );
	addHookPost( pc, damage, pc_damage_post );
}
