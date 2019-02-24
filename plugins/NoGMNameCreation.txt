//===== Hercules Plugin ======================================
//= NoGMNameCreation
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0_0
//===== Compatible With: ===================================== 
//= Hercules 2019-01-31
//===== Description: =========================================
//= disallow the word GM at the beginning of the character creation
//===== Topic ================================================
//= 
//===== Additional Comments: =================================  
//= 
//============================================================

#include "common/hercules.h"
#include "char/char.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"NoGMNameCreation",
	SERVER_TYPE_CHAR,
	"0_0",
	HPM_VERSION,
};

char_check_char_name_post( int retVal, const char *name, const char *esc_name ) {
	if ( name[0] == 'G' && name[1] == 'M' )
		return -5;
	return retVal;
}

HPExport void plugin_init (void) {
	addHookPost( chr, check_char_name, char_check_char_name_post );
}
