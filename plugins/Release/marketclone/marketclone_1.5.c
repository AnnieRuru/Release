//===== Hercules Plugin ======================================
//= @market clone
//===== By: ==================================================
//= AnnieRuru
//= Credit - Dastgir -> http://hercules.ws/board/topic/7188-market/
//===== Current Version: =====================================
//= 1.5
//===== Compatible With: ===================================== 
//= Hercules 2016-1-1
//===== Description: =========================================
//= Create a market clone, to leave a message for other players
//= while the player can go hunting/questing/events
//===== Topic ================================================
//= http://hercules.ws/board/topic/7242-market-clone/
//===== Additional Comments: =================================  
//= -- AtCommands --
//= @market "<Title>" "<Message>" <Color>
//=   create a market clone with a chat room titled -> "<Title>"
//=   when players tries to join the chat room, it refuse the joining, but instead leave a message -> "<Message>" <- with their own desired <Color>
//= @marketkill
//=   kill the market clone without logging off
//=
//= a GM doing a @killmonster @killmonster2, or *killmonster *kilmonsterall script command will not remove the clone
//= but @reloadscript, however, will remove it
//============================================================

//	how many seconds delay from creating another market clone ?
//	Note: if you set it too low like 1 second, player might spawn the clone all over the town, unit->free doesn't act fast enough to remove them, 10 seconds is ideal
int market_clone_delay = 10; // 10 seconds

//	the zeny cost for creating a market clone
int market_clone_zenycost = 0;

//	if the player has cart/riding/ward/falcon, will these objects cloned as well ?
int market_clone_companion = 0; // set to 1 to enable

//============================================================

#include "common/hercules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map/pc.h"
#include "map/mob.h"
#include "map/npc.h"
#include "map/chat.h"
#include "map/clif.h"
#include "map/guild.h"
#include "common/memmgr.h"
#include "common/strlib.h"
#include "common/socket.h"
#include "common/HPMDataCheck.h"

HPExport struct hplugin_info pinfo = {
	"marketclone",
	SERVER_TYPE_MAP,
	"1.5",
	HPM_VERSION,
};

struct player_data {
	int market_clone_id;
	int market_clone_delay;
};

struct monster_data {
	int uid;
	int market_pushcart;
	int market_chat_id;
	int market_msg_color;
	char market_message[CHAT_SIZE_MAX];
};

void color_list( int fd ) {
	clif->messagecolor_self( fd, 0x00FFFF, "0  - Aqua (default)" );
	clif->messagecolor_self( fd, 0x000001, "1  - Black" );
	clif->messagecolor_self( fd, 0x0000FF, "2  - Blue" );
	clif->messagecolor_self( fd, 0x808080, "3  - Grey" );
	clif->messagecolor_self( fd, 0x008000, "4  - Green" );
	clif->messagecolor_self( fd, 0x00FF00, "5  - Lime" );
	clif->messagecolor_self( fd, 0x800000, "6  - Maroon" );
	clif->messagecolor_self( fd, 0x000080, "7  - Navy" );
	clif->messagecolor_self( fd, 0x808000, "8  - Olive" );
	clif->messagecolor_self( fd, 0xFFA500, "9  - Orange" );
	clif->messagecolor_self( fd, 0x800080, "10 - Purple" );
	clif->messagecolor_self( fd, 0xFF0000, "11 - Red" );
	clif->messagecolor_self( fd, 0xC0C0C0, "12 - Silver" );
	clif->messagecolor_self( fd, 0x008080, "13 - Teal" );
	clif->messagecolor_self( fd, 0xFFFFFF, "14 - White" );
	clif->messagecolor_self( fd, 0xFFC0CB, "15 - Pink" );
	clif->messagecolor_self( fd, 0xD2691E, "16 - Chocolate" );
	clif->messagecolor_self( fd, 0xFFD700, "17 - Gold" );
	clif->messagecolor_self( fd, 0xEE82EE, "18 - Violet" );
	return;
}
//	I was thinking of using array before ... but I guess, this kind of implementation is easier for you guys to add custom color
//	if want to remove a color, just comment/delete that line. If want to add, well, you can guess the format XD
void color_message( int fd, int msg_color, char market_msg[] ) {
	switch ( msg_color ) {
		default: clif->messagecolor_self( fd, 0x00FFFF, market_msg ); break;
		case 1: clif->messagecolor_self( fd, 0x000001, market_msg ); break;
		case 2: clif->messagecolor_self( fd, 0x0000FF, market_msg ); break;
		case 3: clif->messagecolor_self( fd, 0x808080, market_msg ); break;
		case 4: clif->messagecolor_self( fd, 0x008000, market_msg ); break;
		case 5: clif->messagecolor_self( fd, 0x00FF00, market_msg ); break;
		case 6: clif->messagecolor_self( fd, 0x800000, market_msg ); break;
		case 7: clif->messagecolor_self( fd, 0x000080, market_msg ); break;
		case 8: clif->messagecolor_self( fd, 0x808000, market_msg ); break;
		case 9: clif->messagecolor_self( fd, 0xFFA500, market_msg ); break;
		case 10: clif->messagecolor_self( fd, 0x800080, market_msg ); break;
		case 11: clif->messagecolor_self( fd, 0xFF0000, market_msg ); break;
		case 12: clif->messagecolor_self( fd, 0xC0C0C0, market_msg ); break;
		case 13: clif->messagecolor_self( fd, 0x008080, market_msg ); break;
		case 14: clif->messagecolor_self( fd, 0xFFFFFF, market_msg ); break;
		case 15: clif->messagecolor_self( fd, 0xFFC0CB, market_msg ); break;
		case 16: clif->messagecolor_self( fd, 0xD2691E, market_msg ); break;
		case 17: clif->messagecolor_self( fd, 0xFFD700, market_msg ); break;
		case 18: clif->messagecolor_self( fd, 0xEE82EE, market_msg ); break;
	}
	return;
}

int mob_clone_spawn_market( struct map_session_data *sd, int16 m, int16 x, int16 y, char market_title[], char market_msg[], int msg_color ) { //Copy of mob_clone_spawn with some modification.
	int class_;
	struct mob_data *md;
	struct monster_data *mmd;
	struct mob_db* db;
	struct status_data *mstatus;
	struct chat_data* cd;

	ARR_FIND( MOB_CLONE_START, MOB_CLONE_END, class_, mob->db_data[class_] == NULL );
	if ( class_ >= MOB_CLONE_END )
		return 0;

	db = mob->db_data[class_] = (struct mob_db*)aCalloc( 1, sizeof(struct mob_db) );
	mstatus = &db->status;
	safestrncpy( db->sprite, sd->status.name, NAME_LENGTH );
	safestrncpy( db->name, sd->status.name, NAME_LENGTH );
	safestrncpy( db->jname, sd->status.name, NAME_LENGTH );
	db->lv = status->get_lv(&sd->bl);
	memcpy( mstatus, &sd->base_status, sizeof( struct status_data ) );
	mstatus->rhw.atk = mstatus->rhw.atk2 = mstatus->lhw.atk = mstatus->lhw.atk2 = mstatus->hp = mstatus->max_hp = mstatus->sp = mstatus->max_sp =  1;
	mstatus->mode = 0;
	memcpy( &db->vd, &sd->vd, sizeof( struct view_data ) );
	db->base_exp = db->job_exp = db->range2 = db->range3 = 1;
	db->option = 0;

	md = mob->once_spawn_sub( &sd->bl, m, x, y, sd->status.name, class_, "", SZ_SMALL, AI_NONE );
	if ( !md )
		return 0;
	md->special_state.clone = 1;
	mob->spawn(md);
	unit->setdir( &md->bl, unit->getdir(&sd->bl) );
	cd = chat->create( &md->bl, market_title, "", 1, false, 0, "", 0, 1, MAX_LEVEL );
	if ( !cd )
		return 0;
	mmd = getFromMOBDATA(md,0);
	CREATE( mmd, struct monster_data, 1 );
	mmd->market_chat_id = cd->bl.id;
	safestrncpy( mmd->market_message, market_msg, CHAT_SIZE_MAX );
	mmd->uid = md->bl.id;
	addToMOBDATA( md, mmd, 0, true );
	clif->dispchat( cd, 0 );
	if ( sd->vd.dead_sit == 2 )
		clif->sitting( &md->bl );
	if ( market_clone_companion ) {
		if ( sd->sc.data[SC_PUSH_CART] ) {
			mmd->market_pushcart = sd->sc.data[SC_PUSH_CART]->val1;
			clif->sc_load( &md->bl, mmd->uid, AREA, SI_ON_PUSH_CART, mmd->market_pushcart, 0, 0 );
		}
		if ( pc_hasmount(sd) || pc_isfalcon(sd) ) {
			int option = sd->sc.option & (OPTION_RIDING|OPTION_DRAGON|OPTION_WUGRIDER|OPTION_FALCON|OPTION_MADOGEAR);
			md->sc.option |= option;
			clif->changeoption( &md->bl );
		}
	}
	mmd->market_msg_color = msg_color;
	return md->bl.id;
}

ACMD(market){
	struct player_data *ssd = getFromMSD(sd,0);
	char title[CHAT_SIZE_MAX], msg[CHAT_SIZE_MAX], atcmd_output[CHAT_SIZE_MAX];
	int color = 0;
	if ( !ssd ) {
		CREATE( ssd, struct player_data, 1 );
		ssd->market_clone_id = 0;
		ssd->market_clone_delay = 0;
		addToMSD( sd, ssd, 0, true );
	}
	if ( ssd->market_clone_id ) {
		clif->message( fd, "You already have a Market clone summoned. Type '@marketkill' to remove that clone.");
		return false;
	}
	if ( ssd->market_clone_delay + market_clone_delay > (int)time(NULL) ) {
		safesnprintf( atcmd_output, CHAT_SIZE_MAX, "You must wait %d seconds before using this command again.", ssd->market_clone_delay + market_clone_delay - (int)time(NULL) );
		clif->message( fd, atcmd_output );
		return false;
	}
	if ( !map->list[sd->bl.m].flag.town ) {
		clif->message( fd, "You can only use @market in a town.");
		return false;
	}
	if ( map->getcell( sd->bl.m, &sd->bl, sd->bl.x, sd->bl.y, CELL_CHKNOCHAT ) ) {
		clif->message( fd, "You cannot use @market in this area.");
		return false;
	}
	if ( sd->status.zeny < market_clone_zenycost ) {
		safesnprintf( atcmd_output, CHAT_SIZE_MAX, "You must have at least %d zeny to have a Market clone.", market_clone_zenycost );
		clif->message( fd, atcmd_output );
		return false;
	}
	if ( pc_isdead(sd) ) {
		clif->message( fd, "You can't create a Market clone while you are dead." );
		return false;
	}
	if ( sd->chatID ) {
		clif->message( fd, "You can't create a Market clone while you already open a chatroom." );
		return false;
	}
	if ( sd->state.vending ) {
		clif->message( fd, "You can't create a Market clone while you are vending." );
		return false;
	}
	if ( sd->sc.option & OPTION_INVISIBLE ) {
		clif->message( fd, "You can't create a Market clone while you are using @hide." );
		return false;
	}
	if ( npc->isnear( &sd->bl ) ) {
		clif->message( fd, "You can't create a Market clone too near to an npc." );
		return false;
	}
	if ( !message || !*message ) {
		clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\" <Color>");
		clif->message( fd, "The <Color> field is optional. Examples:-");
		color_list(fd);
		return false;
	}
//	if ( sscanf( message, "\"%[^\"]\" \"%[^\"]\"", title, msg ) < 2 ) {
//		clif->message( fd, "Remember the Quotation Mark -> \"");
//		clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\"");
//		return false;
//	} no more f*cking sscanf
	{ // say hello to the dirties string calculation ~ Hooray ~ !!
		int i = 0, j = 0, l = strlen( message ) +1;
		char *temp = (char*)aMalloc( strlen( message ) +1 );
		if ( message[0] != '\"' ) {
			clif->message( fd, "Remember the <Title> should start with a Quotation Mark -> \"");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\" <Color>");
			aFree(temp);
			return false;
		}
		i = 1;
		while ( i <= l ) {
			if ( message[i] == '\"' || message[i] == '\0' )
				break;
			else
				temp[j++] = message[i];
			++i;
		}
		if ( message[i] != '\"' ) {
			clif->message( fd, "Remember the <Title> should end with a Quotation Mark -> \"");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\" <Color>");
			aFree(temp);
			return false;
		}
		temp[j] = '\0';
		safestrncpy( title, temp, CHAT_SIZE_MAX );		
		++i;
		if ( message[i] != ' ' ) {
			clif->message( fd, "Remember the [Space] between the <Title> and <Message>.");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\" <Color>");
			aFree(temp);
			return false;
		}
		++i;
		if ( message[i] != '\"' ) {
			clif->message( fd, "Remember the <Message> should start with a Quotation Mark -> \"");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\" <Color>");
			aFree(temp);
			return false;
		}
		++i;
		j = 0;
		while ( i <= l ) {
			if ( message[i] == '\"' || message[i] == '\0' )
				break;
			else
				temp[j++] = message[i];
			++i;
		}
		if ( message[i] != '\"' ) {
			clif->message( fd, "Remember the <Message> should end with a Quotation Mark -> \"");
			clif->message( fd, "Syntax: @market \"<Title>\" \"<Message>\" <Color>");
			aFree(temp);
			return false;
		}
		temp[j] = '\0';
		safestrncpy( msg, temp, CHAT_SIZE_MAX );
		++i;
		if ( message[i] == ' ' ) {
			j = 0;
			++i;
			while ( i <= l ) {
				if ( message[i] == '\0' )
					break;
				else
					temp[j++] = message[i];
				++i;
			}
			temp[j] = '\0';
			if ( strlen( temp ) > 2 ) {
				clif->message( fd, "Please input a valid number from the list.");
				color_list(fd);
				aFree(temp);
				return false;
			}
			color = atoi(temp);
		}
		aFree(temp);
	}
	if ( strlen( title ) < 4 ) {
		clif->message( fd, "The Title must more than 4 characters." );
		return false;
	}
	if ( strlen( title ) >= CHATROOM_TITLE_SIZE ) {
		safesnprintf( atcmd_output, CHAT_SIZE_MAX, "The Title must not more than %d characters.", CHATROOM_TITLE_SIZE );
		clif->message( fd, atcmd_output );
		return false;
	}
	if ( strlen( msg ) < 4 ) {
		clif->message( fd, "The Message must more than 4 characters." );
		return false;
	}
	ssd->market_clone_id = mob_clone_spawn_market( sd, sd->bl.m, sd->bl.x, sd->bl.y, title, msg, color );
	if ( !ssd->market_clone_id ) {
		clif->message( fd, "Market clone Cannot be Created." );
		return false;
	}
	if ( market_clone_zenycost )
		pc->payzeny( sd, market_clone_zenycost, LOG_TYPE_COMMAND, NULL );
	clif->message( fd, "Market clone Created." );
	return true;
}

ACMD(marketkill){
	struct player_data *ssd = getFromMSD(sd,0);
	if ( !ssd ) {
		CREATE( ssd, struct player_data, 1 );
		ssd->market_clone_id = 0;
		ssd->market_clone_delay = 0;
		addToMSD( sd, ssd, 0, true );
	}
	if ( !ssd->market_clone_id ) {
		clif->message( fd, "You don't have a market clone yet. Type '@market \"<title>\" \"<message>\"' to create one.");
		return false;
	}
	status_kill( map->id2bl( ssd->market_clone_id ) );
	clif->message( fd, "Your market clone has removed." );
	ssd->market_clone_id = 0;
	ssd->market_clone_delay = (int)time(NULL);
	return true;
}

int npc_reload_pre(void) {
	TBL_PC *sd;
	struct player_data *ssd;
	struct s_mapiterator *iter;
	iter = mapit->alloc( MAPIT_NORMAL, BL_PC );
	for ( sd = (TBL_PC*)mapit->first(iter); mapit->exists(iter); sd = (TBL_PC*)mapit->next(iter) ) {
		if (( ssd = getFromMSD(sd,0) )) {
			if ( ssd->market_clone_id ) {
				status_kill( map->id2bl( ssd->market_clone_id ) );
				ssd->market_clone_id = 0;
				ssd->market_clone_delay = (int)time(NULL);
			}
		}
	}
	mapit->free(iter);
	return 0;	
}

int battle_check_target_post( int retVal, struct block_list *src, struct block_list *target, int *flag ) {
	if ( retVal == 1 && target->type == BL_MOB ) {
		struct monster_data *mmd = getFromMOBDATA( (TBL_MOB*)target, 0 );
		if ( mmd )
			return -1;
	}
	return retVal;
}

bool chat_joinchat_pre( struct map_session_data *sd, int *chatid, const char *pass ) {
	struct chat_data* cd = (struct chat_data*)map->id2bl( *chatid );
	if( cd == NULL || cd->bl.type != BL_CHAT || cd->bl.m != sd->bl.m || sd->state.vending || sd->state.buyingstore || sd->chatID || ((cd->owner->type == BL_NPC) ? cd->users+1 : cd->users) >= cd->limit ) {
		clif->joinchatfail(sd,0); // room full
		hookStop();
		return false;
	}
	if ( cd->owner->type == BL_MOB ) {
		struct mob_data *md = (TBL_MOB*)cd->owner;
		struct monster_data *mmd = getFromMOBDATA(md,0);
		if ( mmd ) {
			char output[CHAT_SIZE_MAX];
			safesnprintf( output, CHAT_SIZE_MAX, "%s : %s", md->name, mmd->market_message );
			color_message( sd->fd, mmd->market_msg_color, output );
			hookStop();
			return true;
		}
	}
	return true;
}

void clif_getareachar_unit_post( struct map_session_data *sd, struct block_list *bl ) {
	if ( bl->type == BL_MOB ) {
		struct monster_data *mmd = getFromMOBDATA( (TBL_MOB*)bl, 0);
		if ( mmd ) {
			clif->dispchat( (struct chat_data*)map->id2bl( mmd->market_chat_id ), sd->fd );
			if ( market_clone_companion && mmd->market_pushcart )
				clif->sc_load( &sd->bl, mmd->uid, SELF, SI_ON_PUSH_CART, mmd->market_pushcart, 0, 0 );
		}
	}
	return;
}

void clif_charnameack_pre( int *fd, struct block_list *bl ) {
	if ( bl->type == BL_MOB ) {
		TBL_MOB *md = (TBL_MOB*)bl;
		if ( md->guardian_data && md->guardian_data->g )
			return;
		else if ( battle->bc->show_mob_info ) {
			struct monster_data *mmd = getFromMOBDATA(md,0);
			if ( !mmd )
				return;
			else {
				char buf[103];
				WBUFW(buf, 0) = 0x95;
				WBUFL(buf,2) = md->bl.id;
				memcpy(WBUFP(buf,6), md->name, NAME_LENGTH);
				if ( *fd == 0 )
					clif->send( buf, 30, bl, AREA );
				else {
					WFIFOHEAD( *fd, 30 );
					memcpy( WFIFOP(*fd, 0), buf, 30 );
					WFIFOSET( *fd, 30 );
				}
				hookStop();
				return;
			}
		}
	}
}

int map_quit_pre( struct map_session_data *sd ) {
	struct player_data *ssd = getFromMSD(sd,0);
	if ( ssd && ssd->market_clone_id )
		status_kill( map->id2bl( ssd->market_clone_id ) );
	return 0;
}

int killmonster_sub_pre( struct block_list *bl, va_list *ap ) {
	struct mob_data *md = (TBL_MOB*)bl;
	struct monster_data *mmd = getFromMOBDATA( md, 0 );
	if ( mmd )
		hookStop();
	return 0;
}

int skillnotok_pre( uint16 *skill_id, struct map_session_data *sd ) {
	struct player_data *ssd = getFromMSD(sd,0);
	if ( ssd && ssd->market_clone_id && ( *skill_id == MC_VENDING || *skill_id == ALL_BUYING_STORE ) ) {
		clif->messagecolor_self( sd->fd, COLOR_RED, "You can't use vending while you already have a market clone." );
		hookStop();
		return 1;
	}
	return 0;
}

HPExport void plugin_init (void) {
	addAtcommand( "market", market );
	addAtcommand( "marketkill", marketkill );

	addHookPre( "npc->reload", npc_reload_pre );
	addHookPost( "battle->check_target", battle_check_target_post );
	addHookPre( "chat->join", chat_joinchat_pre );
	addHookPost( "clif->getareachar_unit", clif_getareachar_unit_post );
	addHookPre( "clif->charnameack", clif_charnameack_pre );
	addHookPre( "map->quit", map_quit_pre );
	addHookPre( "atcommand->atkillmonster_sub", killmonster_sub_pre );
	addHookPre( "script->buildin_killmonster_sub_strip", killmonster_sub_pre );
	addHookPre( "script->buildin_killmonster_sub", killmonster_sub_pre );
	addHookPre( "script->buildin_killmonsterall_sub_strip", killmonster_sub_pre );
	addHookPre( "script->buildin_killmonsterall_sub", killmonster_sub_pre );
	addHookPre( "skill->not_ok", skillnotok_pre );
}