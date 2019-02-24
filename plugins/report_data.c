//===== Hercules Plugin ======================================
//= report_data Script Command
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2015-12-30
//===== Description: =========================================
//= report the data of that particular field
//===== Topic ================================================
//= <none>
//===== Additional Comments: =================================  
//= I need this to write that 'c' type variant BUILDIN_DEF pull request
//= [Outdated] = use *getdatatype script command
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map/script.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"reportdata",
	SERVER_TYPE_MAP,
	">.<",
	HPM_VERSION,
};

BUILDIN(report_data) {
	if ( !script_hasdata(st,2) ) {
		ShowWarning( "script:report_data: a field is needed to run this command.\n" );
		return false;
	}
	else {
		struct script_data* data = script_getdata(st,2);
		if ( data_isreference(data) ) {
			if ( reference_toconstant(data) )
				ShowDebug( "constant -> %d", reference_getconstant(data) );
			if ( reference_toparam(data) )
				ShowDebug( "parameter -> %d", reference_getparamtype(data) );
			if ( reference_tovariable(data) )
				ShowDebug( "variable -> %s", reference_getname(data) );
		}
		script->reportdata(data);
	}
	return true;
}

HPExport void plugin_init (void) {
	addScriptCommand( "report_data", "?", report_data );
}