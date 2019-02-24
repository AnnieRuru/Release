//===== Hercules Plugin ======================================
//= 1charpergm
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2a
//===== Compatible With: ===================================== 
//= Hercules 2019-02-16
//===== Description: =========================================
//= make each GM account can only have 1 character slot
//===== Topic ================================================
//= http://herc.ws/board/topic/11215-one-char-per-gm-account/?do=findComment&comment=90332
//===== Additional Comments: =================================  
//= simple trick, only allow them to create/login the character on slot 0
//============================================================

#include "common/hercules.h"
#include "char/char.h"
#include "common/nullpo.h"
#include "common/socket.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"1charpergm",
	SERVER_TYPE_CHAR,
	"0.2a",
	HPM_VERSION,
};

static int char_make_new_char_sql_pre( struct char_session_data **sd, const char **name_, int *str, int *agi, int *vit, int *int_, int *dex, int *luk, int *slot, int *hair_color, int *hair_style, int *starting_job, uint8 *sex) {
	nullpo_retr(-2, *sd);
	if ( *slot != 0 && (*sd)->group_id > 10 ) {	// change 10 to minimum gm level
		hookStop();
		return -2;
	}
	return 0;
}

static void char_parse_char_select_pre( int *fd, struct char_session_data **sd, uint32 *ipl ) {
	int slot = RFIFOB(*fd, 2);
	if ( slot != 0 && (*sd)->group_id > 10 ) {
		RFIFOSKIP(*fd, 3);
		chr->creation_failed(*fd, -4);
		hookStop();
	}
	return;
}

HPExport void plugin_init (void) {
	addHookPre( chr, make_new_char_sql, char_make_new_char_sql_pre );
	addHookPre( chr, parse_char_select, char_parse_char_select_pre );
}