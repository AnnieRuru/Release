//===== Hercules Plugin ======================================
//= OnPCStatCalcEvent
//===== By: ==================================================
//= AnnieRuru
//= originally by QQfoolsorellina
//===== Current Version: =====================================
//= 2.0
//===== Compatible With: ===================================== 
//= Hercules 2020-11-12
//===== Description: =========================================
//= give player permanent bonus
//===== Topic ================================================
//= http://herc.ws/board/topic/11292-onpcstatcalcevent/
//===== Additional Comments: =================================  
//= fix `*skill` doesn't work correctly by using an external file
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/script.h"
#include "map/status.h"
#include "common/conf.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"OnPCStatCalcEvent",
	SERVER_TYPE_MAP,
	"2.0",
	HPM_VERSION,
};

VECTOR_DECL(struct script_code *)statcalcevent;

int read_onpcstatcalcevent(void) {
	const char *confpath = "conf/import/OnPCStatCalcEvent.conf";
	struct config_t onpcstatcalcevent_conf;
	if (!libconfig->load_file(&onpcstatcalcevent_conf, confpath)) {
		ShowError("can't read %s, file not found !\n", confpath);
		return -1;
	}

	struct config_setting_t *config_db = libconfig->setting_get_member(onpcstatcalcevent_conf.root, "script");
	if (config_db == NULL) {
		ShowError("can't read %s\n", confpath);
		libconfig->destroy(&onpcstatcalcevent_conf);
		return -1;
	}

	struct config_setting_t *config = NULL;
	int i = 0;
	struct script_code *tmp;
	const char *string = NULL;
	VECTOR_INIT(statcalcevent);
	while ((config = libconfig->setting_get_elem(config_db, i++))) {
		if (libconfig->setting_lookup_string(config, "OnPCStatCalcEvent", &string)) {
			tmp = script->parse(string, "OnPCStatCalcEvent", 0, SCRIPT_IGNORE_EXTERNAL_BRACKETS, NULL);
			VECTOR_ENSURE(statcalcevent, 1, 1);
			VECTOR_PUSH(statcalcevent, tmp);
		}
		else
			ShowError("Invalid Type on entry no."CL_WHITE"%d"CL_RESET" in '"CL_WHITE"%s"CL_RESET"'.\n", i, confpath);
	}

//	ShowDebug("%d\n", VECTOR_LENGTH(statcalcevent));
//	for (i = 0; i < VECTOR_LENGTH(statcalcevent); i++) {
//		ShowDebug("%s\n", VECTOR_INDEX(statcalcevent, i));
//		ShowDebug("%s\n", VECTOR_INDEX(statcalcevent, i)->script_buf);
//	}

	libconfig->destroy(&onpcstatcalcevent_conf);
	ShowStatus("Done reading '"CL_WHITE"%d"CL_RESET"' entries in '"CL_WHITE"%s"CL_RESET"'.\n", VECTOR_LENGTH(statcalcevent), confpath);
	return VECTOR_LENGTH(statcalcevent);
}

void status_calc_pc_additional_post(struct map_session_data *sd, enum e_status_calc_opt opt) {
	if (sd == NULL)
		return;
	for (int i = 0; i < VECTOR_LENGTH(statcalcevent); i++)
		script->run(VECTOR_INDEX(statcalcevent, i), 0, sd->bl.id, 0);
	return;
}

BUILDIN(recalculatestat) {
	struct map_session_data *sd;
	if (script_hasdata(st,2)) {
		if (data_isstring(script_getdata(st,2)))
			sd = map->nick2sd(script_getstr(st,2), true);
		else
			sd = map->id2sd(script_getnum(st,2));
	}
	else
		sd = script->rid2sd(st);
	if (sd)
		status_calc_pc(sd, SCO_FORCE);
	return true;
}

void clean_onpcstatcalcevent(void) {
	for (int i = 0; i < VECTOR_LENGTH(statcalcevent); ++i) {
		VECTOR_CLEAR(VECTOR_INDEX(statcalcevent, i)->script_buf);
		aFree(VECTOR_INDEX(statcalcevent, i));
	}
	VECTOR_CLEAR(statcalcevent);
	return;
}

int script_reload_post(int retVal) {
	clean_onpcstatcalcevent();
	read_onpcstatcalcevent();
	return retVal;
}

HPExport void plugin_init(void) {
	addHookPost(status, calc_pc_additional, status_calc_pc_additional_post);
	addScriptCommand("recalculatestat", "?", recalculatestat);
	addHookPost(script, reload, script_reload_post);
}

HPExport void server_online(void) {
	read_onpcstatcalcevent();
}

HPExport void plugin_final(void) {
	clean_onpcstatcalcevent();
}
