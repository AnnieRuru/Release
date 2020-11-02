//===== Hercules Plugin ======================================
//= bComaResist
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.1
//===== Compatible With: ===================================== 
//= Hercules 2020-11-03
//===== Description: =========================================
//= a custom bonus to defend against SC_COMA
//===== Topic ================================================
//= https://herc.ws/board/topic/18910-coma-status-eff_-usage/
//===== Additional Comments: =================================
//= bonus bComaResist,`n`;
//= resist SC_COMA in unit (n) 0.01% chance
//= 10000 is 100% resist
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/map.h"
#include "common/memmgr.h"
#include "common/nullpo.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"bComaResist",
	SERVER_TYPE_MAP,
	"0.1",
	HPM_VERSION,
};

int SP_COMA_RESIST = 0;

struct player_data {
	int coma_resist_value;
};

static int pc_reg_received_pre(struct map_session_data **sd) {
	if (*sd == NULL)
		return 0;
	struct player_data *ssd;
	CREATE(ssd, struct player_data, true);
	ssd->coma_resist_value = 0;
	addToMSD(*sd, ssd, 0, true);
	return 1;
}

int pc_bonus_pre(struct map_session_data **sd, int *type, int *val) {
	if (*sd == NULL)
		return 0;
	if (*type == SP_COMA_RESIST) {
		if ((*sd)->state.lr_flag != 2) {
			struct player_data *ssd = getFromMSD(*sd, 0);
			nullpo_ret(ssd);
			ssd->coma_resist_value += *val;
			hookStop();
		}
	}
	return 0;
}

static int status_get_sc_def_post(int retVal, struct block_list *src, struct block_list *bl, enum sc_type type, int rate, int tick, int flag) {
	if (type != SC_COMA || bl == NULL || bl->type != BL_PC)
		return retVal;
	struct map_session_data *sd = BL_CAST(BL_PC, bl);
	struct player_data *ssd = getFromMSD(sd, 0);
	nullpo_ret(ssd);
	if ((rand() % 10000) < ssd->coma_resist_value)
		return 0;
	return retVal;
}

// flush bComaResist back to 0 when recalculating bonus
int status_calc_pc_pre(struct map_session_data **sd, enum e_status_calc_opt *opt) {
	struct player_data *ssd = getFromMSD(*sd, 0);
	nullpo_ret(ssd);
	ssd->coma_resist_value = 0;
	return 0;
}

HPExport void plugin_init (void) {
	SP_COMA_RESIST = map->get_new_bonus_id();
	script->set_constant("bComaResist", SP_COMA_RESIST, false, false);
	addHookPre(pc, reg_received, pc_reg_received_pre);
	addHookPre(pc, bonus, pc_bonus_pre);
	addHookPost(status, get_sc_def, status_get_sc_def_post);
	addHookPre(status, calc_pc_, status_calc_pc_pre);
}
