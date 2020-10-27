//===== Hercules Plugin ======================================
//= *setmobdata/getmobdata script command
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.0
//===== Compatible With: ===================================== 
//= Hercules 2015-11-07
//===== Description: =========================================
//= *getmobdata to retrieve the monster's information
//= *setmobdata to modify the monster attribute
//===== Topic ================================================
//= http://herc.ws/board/topic/11097-setmobdata-getmobdata/
//===== Additional Comments: =================================  
//= This is not the same setmobdata/getmobdata from [Lance] mob controller system
//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map/mob.h"
#include "map/script.h"
#include "map/battle.h"
#include "common/memmgr.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"setmobdata",   // Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

enum mobdata_type {
	MOBDATA_MOBID = 1,
	MOBDATA_NAME,
	MOBDATA_LEVEL,
	MOBDATA_HP,
	MOBDATA_MAXHP,
	MOBDATA_ATK,
	MOBDATA_MATK,
	MOBDATA_DEF,
	MOBDATA_MDEF,
	MOBDATA_HIT,
	MOBDATA_FLEE,
	MOBDATA_CRITICAL,
	MOBDATA_PERFECT_DODGE,
	MOBDATA_STR,
	MOBDATA_AGI,
	MOBDATA_VIT,
	MOBDATA_INT,
	MOBDATA_DEX,
	MOBDATA_LUK,
	MOBDATA_ATTACK_RANGE,
	MOBDATA_RACE,
	MOBDATA_ELEMENT_TYPE,
	MOBDATA_ELEMENT_LEVEL,
	MOBDATA_MODE,
	MOBDATA_MOVESPEED,
	MOBDATA_ATTACK_DELAY,
	MOBDATA_ATTACK_MOTION,
	MOBDATA_DAMAGE_MOTION,
	MOBDATA_DIRECTION,
	MOBDATA_BLOCKIMMUNE,
	MOBDATA_NORANDOMWALK,
};

struct monster_data {
	unsigned int norandomwalk: 1;
	unsigned int blockimmune: 1;
};

BUILDIN(setmobdata) {
	int mob_id = script_getnum(st,2),
		type = script_getnum(st,3);
	struct block_list *bl = map->id2bl( mob_id );
	if ( bl == NULL ) {
		ShowWarning( "buildin_setmobdata: Attempted to retrieve a non-existed GID %d.\n", mob_id );
		return false;
	}
	else if ( bl->type != BL_MOB ) {
		ShowWarning( "buildin_setmobdata: Attempted to retrieve not a monster. GID %d.\n", mob_id );
		return false;
	}
	else {
		TBL_MOB *md = (TBL_MOB*)bl;
		switch( type ) {
			case MOBDATA_MOBID :
				ShowWarning( "buildin_setmobdata: Attempted to change MobID with type MOBDATA_MOBID.\n" );
				break;
			case MOBDATA_NAME : safesnprintf( md->name, NAME_LENGTH, script_getstr(st,4) ); break;
			case MOBDATA_LEVEL : md->level = script_getnum(st,4); break;
			case MOBDATA_HP : md->status.hp = script_getnum(st,4); break;
			case MOBDATA_MAXHP : md->status.max_hp = script_getnum(st,4); break;
			case MOBDATA_ATK : md->status.rhw.atk = script_getnum(st,4); break;
			case MOBDATA_MATK : md->status.rhw.atk2 = script_getnum(st,4); break;
			case MOBDATA_DEF : md->status.def = script_getnum(st,4); break;
			case MOBDATA_MDEF : md->status.mdef = script_getnum(st,4); break;
			case MOBDATA_HIT : md->status.hit = script_getnum(st,4); break;
			case MOBDATA_FLEE : md->status.flee = script_getnum(st,4); break;
			case MOBDATA_CRITICAL : md->status.cri = script_getnum(st,4); break;
			case MOBDATA_PERFECT_DODGE : md->status.flee2 = script_getnum(st,4); break;
			case MOBDATA_STR : md->status.str = script_getnum(st,4); break;
			case MOBDATA_AGI : md->status.agi = script_getnum(st,4); break;
			case MOBDATA_VIT : md->status.vit = script_getnum(st,4); break;
			case MOBDATA_INT : md->status.int_ = script_getnum(st,4); break;
			case MOBDATA_DEX : md->status.dex = script_getnum(st,4); break;
			case MOBDATA_LUK : md->status.luk = script_getnum(st,4); break;
			case MOBDATA_ATTACK_RANGE : md->status.rhw.range = script_getnum(st,4); break;
			case MOBDATA_RACE : md->status.race = script_getnum(st,4); break;
			case MOBDATA_ELEMENT_TYPE : md->status.def_ele = script_getnum(st,4); break;
			case MOBDATA_ELEMENT_LEVEL : md->status.ele_lv = script_getnum(st,4); break;
			case MOBDATA_MODE : md->status.mode = script_getnum(st,4); break;
			case MOBDATA_MOVESPEED : md->status.speed = script_getnum(st,4); break;
			case MOBDATA_ATTACK_DELAY : md->status.adelay = script_getnum(st,4); break;
			case MOBDATA_ATTACK_MOTION : md->status.amotion = script_getnum(st,4); break;
			case MOBDATA_DAMAGE_MOTION : md->status.dmotion = script_getnum(st,4); break;
			case MOBDATA_DIRECTION : md->ud.dir = script_getnum(st,4); break;
			case MOBDATA_BLOCKIMMUNE : {
				struct monster_data *mmd = getFromMOBDATA( md, 0 );
				CREATE( mmd, struct monster_data, 1 );
				mmd->blockimmune = script_getnum(st,4) > 0;
				addToMOBDATA( md, mmd, 0, true );
				break;
			}
			case MOBDATA_NORANDOMWALK : {
				struct monster_data *mmd = getFromMOBDATA( md, 0 );
				CREATE( mmd, struct monster_data, 1 );
				mmd->norandomwalk = script_getnum(st,4) > 0;
				addToMOBDATA( md, mmd, 0, true );
				break;
			}
			default :
				ShowError( "buildin_setmobdata: Attempted to use non-existing type %d.\n", type );
				return false;
		}
	}
	return true;
}

BUILDIN(getmobdata) {
	int mob_id = script_getnum(st,2),
		type = script_getnum(st,3);
	struct block_list *bl = map->id2bl( mob_id );
	if ( bl == NULL ) {
		ShowWarning( "buildin_getmobdata: Attempted to retrieve a non-existed GID %d.\n", mob_id );
		return false;
	}
	else if ( bl->type != BL_MOB ) {
		ShowWarning( "buildin_getmobdata: Attempted to retrieve not a monster. GID %d.\n", mob_id );
		return false;
	}
	else {
		TBL_MOB *md = (TBL_MOB*)bl;
		switch( type ) {
			case MOBDATA_MOBID : script_pushint( st, md->class_ ); break;
			case MOBDATA_NAME : script_pushstrcopy( st, md->name ); break;
			case MOBDATA_LEVEL : script_pushint( st, md->level ); break;
			case MOBDATA_HP : script_pushint( st, md->status.hp ); break;
			case MOBDATA_MAXHP : script_pushint( st, md->status.max_hp ); break;
			case MOBDATA_ATK : script_pushint( st, md->status.rhw.atk ); break;
			case MOBDATA_MATK : script_pushint( st, md->status.rhw.atk2 ); break;
			case MOBDATA_DEF : script_pushint( st, md->status.def ); break;
			case MOBDATA_MDEF : script_pushint( st, md->status.mdef ); break;
			case MOBDATA_HIT : script_pushint( st, md->status.hit ); break;
			case MOBDATA_FLEE : script_pushint( st, md->status.flee ); break;
			case MOBDATA_CRITICAL : script_pushint( st, md->status.cri ); break;
			case MOBDATA_PERFECT_DODGE : script_pushint( st, md->status.flee2 ); break;
			case MOBDATA_STR : script_pushint( st, md->status.str ); break;
			case MOBDATA_AGI : script_pushint( st, md->status.agi ); break;
			case MOBDATA_VIT : script_pushint( st, md->status.vit ); break;
			case MOBDATA_INT : script_pushint( st, md->status.int_ ); break;
			case MOBDATA_DEX : script_pushint( st, md->status.dex ); break;
			case MOBDATA_LUK : script_pushint( st, md->status.luk ); break;
			case MOBDATA_ATTACK_RANGE : script_pushint( st, md->status.rhw.range ); break;
			case MOBDATA_RACE : script_pushint( st, md->status.race ); break;
			case MOBDATA_ELEMENT_TYPE : script_pushint( st, md->status.def_ele ); break;
			case MOBDATA_ELEMENT_LEVEL : script_pushint( st, md->status.ele_lv ); break;
			case MOBDATA_MODE : script_pushint( st, md->status.mode ); break;
			case MOBDATA_MOVESPEED : script_pushint( st, md->status.speed ); break;
			case MOBDATA_ATTACK_DELAY : script_pushint( st, md->status.adelay ); break;
			case MOBDATA_ATTACK_MOTION : script_pushint( st, md->status.amotion ); break;
			case MOBDATA_DAMAGE_MOTION : script_pushint( st, md->status.dmotion ); break;
			case MOBDATA_DIRECTION : script_pushint( st, md->ud.dir ); break;
			case MOBDATA_BLOCKIMMUNE : {
				struct monster_data *mmd = getFromMOBDATA( md, 0 );
				if (( mmd = getFromMOBDATA( md, 0 ) )) {
					script_pushint( st, mmd->blockimmune );
					break;
				}
				else {
					script_pushint( st, 0 );
					break;
				}
			}
			case MOBDATA_NORANDOMWALK : {
				struct monster_data *mmd = getFromMOBDATA( md, 0 );
				if (( mmd = getFromMOBDATA( md, 0 ) )) {
					script_pushint( st, mmd->norandomwalk );
					break;
				}
				else {
					script_pushint( st, 0 );
					break;
				}
			}
			default :
				ShowError( "buildin_getmobdata: Attempted to use non-existing type %d.\n", type );
				return false;
		}
	}
	return true;
}

int mob_randomwalk_pre( struct mob_data *md, int64 *tick ) {
	struct monster_data *mmd;
	if (( mmd = getFromMOBDATA( md, 0 ) ))
		if ( mmd->norandomwalk )
			hookStop();
	return 0;
}

int battle_check_target_post( int retVal, struct block_list *src, struct block_list *target, int *flag ) {
	if ( retVal == 1 && target->type == BL_MOB ) {
		struct block_list *t_bl;
		struct monster_data *mmd;
		if ( ( t_bl = battle->get_master(target) ) == NULL )
			t_bl = target;
		if (( mmd = getFromMOBDATA( (TBL_MOB*)t_bl, 0 ) ))
			if ( mmd->blockimmune )
				return -1;
	}
	return retVal;
}

void script_hardcoded_constants_pre(void) {
	script->set_constant("MOBDATA_MOBID",MOBDATA_MOBID,false);
	script->set_constant("MOBDATA_NAME",MOBDATA_NAME,false);
	script->set_constant("MOBDATA_LEVEL",MOBDATA_LEVEL,false);
	script->set_constant("MOBDATA_HP",MOBDATA_HP,false);
	script->set_constant("MOBDATA_MAXHP",MOBDATA_MAXHP,false);
	script->set_constant("MOBDATA_ATK",MOBDATA_ATK,false);
	script->set_constant("MOBDATA_MATK",MOBDATA_MATK,false);
	script->set_constant("MOBDATA_DEF",MOBDATA_DEF,false);
	script->set_constant("MOBDATA_MDEF",MOBDATA_MDEF,false);
	script->set_constant("MOBDATA_HIT",MOBDATA_HIT,false);
	script->set_constant("MOBDATA_FLEE",MOBDATA_FLEE,false);
	script->set_constant("MOBDATA_CRITICAL",MOBDATA_CRITICAL,false);
	script->set_constant("MOBDATA_PERFECT_DODGE",MOBDATA_PERFECT_DODGE,false);
	script->set_constant("MOBDATA_STR",MOBDATA_STR,false);
	script->set_constant("MOBDATA_AGI",MOBDATA_AGI,false);
	script->set_constant("MOBDATA_VIT",MOBDATA_VIT,false);
	script->set_constant("MOBDATA_INT",MOBDATA_INT,false);
	script->set_constant("MOBDATA_DEX",MOBDATA_DEX,false);
	script->set_constant("MOBDATA_LUK",MOBDATA_LUK,false);
	script->set_constant("MOBDATA_ATTACK_RANGE",MOBDATA_ATTACK_RANGE,false);
	script->set_constant("MOBDATA_RACE",MOBDATA_RACE,false);
	script->set_constant("MOBDATA_ELEMENT_TYPE",MOBDATA_ELEMENT_TYPE,false);
	script->set_constant("MOBDATA_ELEMENT_LEVEL",MOBDATA_ELEMENT_LEVEL,false);
	script->set_constant("MOBDATA_MODE",MOBDATA_MODE,false);
	script->set_constant("MOBDATA_MOVESPEED",MOBDATA_MOVESPEED,false);
	script->set_constant("MOBDATA_ATTACK_DELAY",MOBDATA_ATTACK_DELAY,false);
	script->set_constant("MOBDATA_ATTACK_MOTION",MOBDATA_ATTACK_MOTION,false);
	script->set_constant("MOBDATA_DAMAGE_MOTION",MOBDATA_DAMAGE_MOTION,false);
	script->set_constant("MOBDATA_DIRECTION",MOBDATA_DIRECTION,false);
	script->set_constant("MOBDATA_BLOCKIMMUNE",MOBDATA_BLOCKIMMUNE,false);
	script->set_constant("MOBDATA_NORANDOMWALK",MOBDATA_NORANDOMWALK,false);
	return;
}

HPExport void plugin_init (void) {
	addScriptCommand("setmobdata","iiv",setmobdata);
	addScriptCommand("getmobdata","ii",getmobdata);

	addHookPre( "mob->randomwalk", mob_randomwalk_pre );
	addHookPost( "battle->check_target", battle_check_target_post );
	addHookPre( "script->hardcoded_constants", script_hardcoded_constants_pre );
}