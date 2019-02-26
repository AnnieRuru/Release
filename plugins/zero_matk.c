//===== Hercules Plugin ======================================
//= Zero MATK
//===== By: ==================================================
//= hemagx
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2019-02-24
//===== Description: =========================================
//= make players have 1 matk in custom map
//===== Topic ================================================
//= http://herc.ws/board/topic/16580-how-to-make-matk-1-in-status_calc_pc_/?do=findComment&comment=90591
//===== Additional Comments: =================================
//= useful stuff, learn how to check for custom map
//============================================================

/**
 * This file is part of Hercules.
 * http://herc.ws - http://github.com/HerculesWS/Hercules
 *
 * Copyright (C) 2013-2019  Hercules Dev Team
 * Copyright (C) Hemagx <brahem@aotsw.com>
 *
 * Hercules is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// Zero Matk Hercules Plugin

#include "common/hercules.h" /* Should always be the first Hercules file included! (if you don't make it first, you won't be able to use interfaces) */
#include "common/core.h"
#include "common/mapindex.h"
#include "map/map.h"
#include "map/pc.h"
#include "map/status.h"

#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h" /* should always be the last Hercules file included! (if you don't make it last, it'll intentionally break compile time) */

HPExport struct hplugin_info pinfo = {
	"Zero MATK",    // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"0.1",       // Plugin version
	HPM_VERSION, // HPM Version (don't change, macro is automatically updated)
};

const char *map_name = "prontera";
int16 map_id = -1; // init it with invalid value to make sure on failure it would always cause our check to fail.

static void status_calc_pc_additional_post(struct map_session_data *sd, enum e_status_calc_opt opt)
{
	if (sd == NULL) // I don'r prefer usage of nullpo in hooks, unless we're replacing a function multiaple nullpo reports can turn confusing.
		return;

	if (sd->bl.m != map_id)
		return;

	sd->matk_rate = 0;
	return;
}

/* run when server starts */
HPExport void plugin_init (void) {
	addHookPost(status, calc_pc_additional, status_calc_pc_additional_post);
}

HPExport void server_online (void)
{
	unsigned short map_index;

	if ((map_index = mapindex->name2id(map_name)) == 0) {
		ShowError("%s:plugin_init: Failed to get map index for map %s the plugin would now fail to start\n", pinfo.name, map_name);
		core->runflag = CORE_ST_STOP;
	}

	if ((map_id = map->mapindex2mapid(map_index)) == -1) {
		ShowError("%s:plugin_init: Failed to get map id for map %s the plugin would now fail to start\n", pinfo.name, map_name);
		core->runflag = CORE_ST_STOP;
	}
}