//===== Hercules Plugin ======================================
//= OnPCStatCalcEvent
//===== By: ==================================================
//= AnnieRuru
//= originally by QQfoolsorellina
//===== Current Version: =====================================
//= 1.3
//===== Compatible With: ===================================== 
//= Hercules 2020-09-16
//===== Description: =========================================
//= give player permanent bonus
//===== Topic ================================================
//= http://herc.ws/board/topic/11292-onpcstatcalcevent/
//===== Additional Comments: =================================  
//= remove nullpo in plugins ~ thanks to hemagx
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/npc.h"
#include "map/script.h"
#include "map/status.h"
#include "common/timer.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"OnPCStatCalcEvent",
	SERVER_TYPE_MAP,
	"1.3",
	HPM_VERSION,
};

void status_calc_pc_additional_post(struct map_session_data *sd, enum e_status_calc_opt opt) {
	if (sd == NULL)
		return;
	npc->event_doall_id("OnPCStatCalcEvent", sd->bl.id);
	return;
}

int recalculate_countdown(int tid, int64 tick, int id, intptr data) {
	struct map_session_data *sd = map->id2sd(id);
	status_calc_pc(sd, SCO_FORCE);
	return 0;
}

BUILDIN(recalculatestat) {
	struct map_session_data *sd;
	if (script_hasdata(st,2)) {
		if (data_isstring( script_getdata(st,2)) )
			sd = map->nick2sd( script_getstr(st,2), true);
		else
			sd = map->id2sd(script_getnum(st,2));
	}
	else
		sd = script->rid2sd(st);
	if (sd)
		timer->add(timer->gettick() +1, recalculate_countdown, sd->bl.id, 0); // another trick ... although not thread-safe, but whatever ~
	return true;
}
/*
ACMD(recalculatestat) {
	status_calc_pc(sd, SCO_FORCE);
	return true;
}
*/
HPExport void plugin_init (void) {
	addHookPost(status, calc_pc_additional, status_calc_pc_additional_post);
	addScriptCommand("recalculatestat", "?", recalculatestat);
//	addAtcommand("recalculatestat", recalculatestat);
}
