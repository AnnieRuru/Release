//===== Hercules Plugin ======================================
//= bHPGainRateWhenHit
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.2
//===== Compatible With: ===================================== 
//= Hercules 2019-03-12
//===== Description: =========================================
//= bonus2 bHPGainRateWhenHit, n, x;
//= n/10% chance to gain x% of HP when being attack
//===== Topic ================================================
//= http://herc.ws/board/topic/16597-help-absorb-damage-taken-bonus/?do=findComment&comment=90808
//===== Additional Comments: =================================  
//= useless, just use bNoWeaponDamage/bNoMagicDamage instead
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/map.h"
#include "common/memmgr.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"bHPGainRateWhenHit",
	SERVER_TYPE_MAP,
	"0.2",
	HPM_VERSION,
};

struct player_data {
	int hp_gain_rate_chance;
	int hp_gain_rate_amount;
};

int HP_GAIN_RATE_WHEN_HIT = 0;

int pc_bonus2_pre( struct map_session_data **sd, int *type, int *type2, int *val ) {
	if ( *sd == NULL )
		return 0;
	if ( *type == HP_GAIN_RATE_WHEN_HIT ) {
		if ( (*sd)->state.lr_flag != 2 ) {
			struct player_data *ssd = getFromMSD( *sd, 0 );
			if ( !ssd ) {
				CREATE( ssd, struct player_data, 1 );
				ssd->hp_gain_rate_chance = *type2;
				ssd->hp_gain_rate_amount = *val;
				addToMSD( *sd, ssd, 0, true );
			}
			else {
				ssd->hp_gain_rate_chance += *type2;
				ssd->hp_gain_rate_amount += *val;
			}
		}
		hookStop();
	}
	return 0;
}

void pc_damage_pre( struct map_session_data **sd, struct block_list **src, unsigned int *hp, unsigned int *sp ) {
	struct player_data *ssd = getFromMSD( *sd, 0 );
	if ( ssd ) {
		if ( (*src) == NULL || *src == &(*sd)->bl )
			return;
		if ( (rand() % 1000) < ssd->hp_gain_rate_chance ) {
			pc->heal( *sd, *hp * ssd->hp_gain_rate_amount / 100, 0, true );
			*hp *= ( 100 - ssd->hp_gain_rate_amount ) / 100;
		}
	}
	return;
}

// flush bHPGainRateWhenHit back to NULL when recalculating bonus
int status_calc_pc_pre( struct map_session_data **sd, enum e_status_calc_opt *opt ) {
	struct player_data *ssd = getFromMSD( *sd, 0 );
	if ( ssd )
		removeFromMSD( *sd, 0 );
	return 0;
}

HPExport void plugin_init (void) {
	HP_GAIN_RATE_WHEN_HIT = map->get_new_bonus_id();
	script->set_constant( "bHPGainRateWhenHit", HP_GAIN_RATE_WHEN_HIT, false, false );
	addHookPre( pc, bonus2, pc_bonus2_pre );
	addHookPre( pc, damage, pc_damage_pre );
	addHookPre( status, calc_pc_, status_calc_pc_pre );
}
