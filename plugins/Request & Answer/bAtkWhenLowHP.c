//===== Hercules Plugin ======================================
//= bAtkWhenLowHP
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2019-03-20
//===== Description: =========================================
//= Add bAtk when the player is on Red life -> below 25% hp
//= bonus2 bAtkWhenLowHP, n;
//= Gain n bAtk when HP falls below 25% hp
//===== Topic ================================================
//= http://herc.ws/board/topic/16642-custom-card-code/
//===== Additional Comments: =================================  
//= this is fun, my 1st attempt to get atk/matk calculate properly
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/battle.h"
#include "common/utils.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"bAtkWhenLowHP",
	SERVER_TYPE_MAP,
	"0.2",
	HPM_VERSION,
};

struct player_data {
	int atk_when_low_hp;
	bool is_low_hp;
};

int ATK_WHEN_LOW_HP = 0;

int pc_bonus_pre( struct map_session_data **sd, int *type, int *val ) {
	if ( *sd == NULL )
		return 0;
	if ( *type == ATK_WHEN_LOW_HP ) {
		if ( (*sd)->state.lr_flag != 2 ) {
			struct player_data *ssd = getFromMSD( *sd, 0 );
			if ( ssd == NULL ) {
				CREATE( ssd, struct player_data, 1 );
				ssd->atk_when_low_hp = *val;
				ssd->is_low_hp = false;
				addToMSD( *sd, ssd, 0, true );
			}
			else
				ssd->atk_when_low_hp += *val;
		}
		hookStop();
	}
	return 0;
}

void pc_damage_post( struct map_session_data *sd, struct block_list *src, unsigned int hp, unsigned int sp ) {
	if ( sd == NULL )
		return;
	struct player_data *ssd = getFromMSD( sd, 0 );
	if ( ssd != NULL && ssd->atk_when_low_hp > 0 ) {
		if ( sd->battle_status.hp *100 / sd->battle_status.max_hp < 25 && ssd->is_low_hp == false ) {
			int bonus = sd->base_status.batk + ssd->atk_when_low_hp;
			sd->base_status.batk = cap_value( bonus, 0, USHRT_MAX );
			ssd->is_low_hp = true;
			status_calc_bl( &sd->bl, SCB_BATK );
		}
		else if ( sd->battle_status.hp *100 / sd->battle_status.max_hp >= 25 && ssd->is_low_hp == true ) {
//			int bonus = sd->base_status.batk - ssd->atk_when_low_hp;
//			sd->base_status.batk = cap_value( bonus, 0, USHRT_MAX );
//			ssd->is_low_hp = false;
//			status_calc_bl( &sd->bl, SCB_BATK );
			ssd->is_low_hp = false;
			status_calc_pc( sd, SCO_NONE );
		}
	}
	return;
}

void pc_heal_post( struct map_session_data *sd, unsigned int hp, unsigned int sp, int type ) {
	if ( sd == NULL )
		return;
	struct player_data *ssd = getFromMSD( sd, 0 );
	if ( ssd != NULL && ssd->atk_when_low_hp > 0 ) {
		if ( sd->battle_status.hp *100 / sd->battle_status.max_hp < 25 && ssd->is_low_hp == false ) {
			int bonus = sd->base_status.batk + ssd->atk_when_low_hp;
			sd->base_status.batk = cap_value( bonus, 0, USHRT_MAX );
			ssd->is_low_hp = true;
			status_calc_bl( &sd->bl, SCB_BATK );
		}
		else if ( sd->battle_status.hp *100 / sd->battle_status.max_hp >= 25 && ssd->is_low_hp == true ) {
			ssd->is_low_hp = false;
			status_calc_pc( sd, SCO_NONE );
		}
	}
	return;
}

// flush bAtkWhenLowHP back to NULL when recalculating bonus
int status_calc_pc_pre( struct map_session_data **sd, enum e_status_calc_opt *opt ) {
	struct player_data *ssd = getFromMSD( *sd, 0 );
	if ( ssd )
		removeFromMSD( *sd, 0 );
	return 0;
}

HPExport void plugin_init (void) {
	ATK_WHEN_LOW_HP = map->get_new_bonus_id();
	script->set_constant( "bAtkWhenLowHP", ATK_WHEN_LOW_HP, false, false );
	addHookPre( pc, bonus, pc_bonus_pre );
	addHookPost( pc, damage, pc_damage_post );
	addHookPost( pc, heal, pc_heal_post );
	addHookPre( status, calc_pc_, status_calc_pc_pre );
}
