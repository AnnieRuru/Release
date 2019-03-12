//===== Hercules Plugin ======================================
//= bHPGainRateWhenHit
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 0.5a
//===== Compatible With: ===================================== 
//= Hercules 2019-03-12
//===== Description: =========================================
//= bonus3 bHPGainRateWhenHit, bf, n, x;
//= n/10% chance to gain x% of HP when being attack by BF_FLAG (only BF_WEAPON|BF_MAGIC|BF_MISC are supported)
//= Example:
//= 	bonus3 bHPGainRateWhenHit, BF_WEAPON|BF_MAGIC|BF_MISC, 1000, 100;
//===== Topic ================================================
//= http://herc.ws/board/topic/16597-help-absorb-damage-taken-bonus/?do=findComment&comment=90814
//===== Additional Comments: =================================  
//= useless, just use bNoWeaponDamage/bNoMagicDamage/bNoMiscDamage instead
//============================================================

#include "common/hercules.h"
#include "map/pc.h"
#include "map/map.h"
#include "map/mob.h"
#include "map/battle.h"
#include "common/memmgr.h"
#include "common/random.h"
#include "plugins/HPMHooking.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"bHPGainRateWhenHit",
	SERVER_TYPE_MAP,
	"0.5",
	HPM_VERSION,
};

struct player_data {
	int hp_gain_rate_chance_weapon;
	int hp_gain_rate_amount_weapon;
	int hp_gain_rate_chance_magic;
	int hp_gain_rate_amount_magic;
	int hp_gain_rate_chance_misc;
	int hp_gain_rate_amount_misc;
};

int HP_GAIN_RATE_WHEN_HIT = 0;

int pc_bonus3_pre( struct map_session_data **sd, int *type, int *type2, int *type3, int *val ) {
	if ( *sd == NULL )
		return 0;
	if ( *type == HP_GAIN_RATE_WHEN_HIT ) {
		if ( (*sd)->state.lr_flag != 2 ) {
			struct player_data *ssd = getFromMSD( *sd, 0 );
			if ( ssd == NULL ) {
				CREATE( ssd, struct player_data, 1 );
				ssd->hp_gain_rate_chance_weapon = 0;
				ssd->hp_gain_rate_amount_weapon = 0;
				ssd->hp_gain_rate_chance_magic = 0;
				ssd->hp_gain_rate_amount_magic = 0;
				ssd->hp_gain_rate_chance_misc = 0;
				ssd->hp_gain_rate_amount_misc = 0;
				addToMSD( *sd, ssd, 0, true );
			}
			if ( (*type2 & BF_WEAPON) != 0 ) {
				ssd->hp_gain_rate_chance_weapon += *type3;
				ssd->hp_gain_rate_amount_weapon += *val;
			}
			if ( (*type2 & BF_MAGIC) != 0 ) {
				ssd->hp_gain_rate_chance_magic += *type3;
				ssd->hp_gain_rate_amount_magic += *val;
			}
			if ( (*type2 & BF_MISC) != 0 ) {
				ssd->hp_gain_rate_chance_misc += *type3;
				ssd->hp_gain_rate_amount_misc += *val;
			}
		}
		hookStop();
	}
	return 0;
}

int64 battle_calc_damage_pre( struct block_list **src, struct block_list **bl, struct Damage **d, int64 *damage, uint16 *skill_id, uint16 *skill_lv ) {
	if ( *bl == NULL || *d == NULL )
		return 0;

	if ( *damage == 0 )
		return 0;

	if ( battle->bc->ksprotection && mob->ksprotected( *src, *bl ) )
		return 0;

	struct map_session_data *t_sd = BL_CAST( BL_PC, *bl );
	if ( t_sd != NULL ) {
		struct player_data *ssd = getFromMSD( t_sd, 0 );
		if ( ssd ) {
			int flag = (*d)->flag;
			unsigned int damage_unsigned = (unsigned int)*damage; // just to remove compile error ... dunno if this break anything
			if ( (flag & BF_WEAPON) != 0 && (rnd() % 1000) < ssd->hp_gain_rate_chance_weapon ) {
				if ( ssd->hp_gain_rate_amount_weapon >= 100 ) {
					pc->heal( t_sd, damage_unsigned, 0, true );
					*damage = 0;
				}
				else {
					pc->heal( t_sd, damage_unsigned * ssd->hp_gain_rate_amount_weapon / 100, 0, true );
					*damage -= *damage * ssd->hp_gain_rate_amount_weapon / 100;
				}
			}
			if ( (flag & BF_MAGIC) != 0 && (rnd() % 1000) < ssd->hp_gain_rate_chance_magic ) {
				if ( ssd->hp_gain_rate_amount_magic >= 100 ) {
					pc->heal( t_sd, damage_unsigned, 0, true );
					*damage = 0;
				}
				else {
					pc->heal( t_sd, damage_unsigned * ssd->hp_gain_rate_amount_magic / 100, 0, true );
					*damage -= *damage * ssd->hp_gain_rate_amount_magic / 100;
				}
			}
			if ( (flag & BF_MISC) != 0 && (rnd() % 1000) < ssd->hp_gain_rate_chance_misc ) {
				if ( ssd->hp_gain_rate_amount_misc >= 100 ) {
					pc->heal( t_sd, damage_unsigned, 0, true );
					*damage = 0;
				}
				else {
					pc->heal( t_sd, damage_unsigned * ssd->hp_gain_rate_amount_misc / 100, 0, true );
					*damage -= *damage * ssd->hp_gain_rate_amount_misc / 100;
				}
			}
		}
	}
	return 0;
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
	addHookPre( pc, bonus3, pc_bonus3_pre );
	addHookPre( battle, calc_damage, battle_calc_damage_pre );
	addHookPre( status, calc_pc_, status_calc_pc_pre );
}
