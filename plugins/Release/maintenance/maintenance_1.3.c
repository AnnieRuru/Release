//===== Hercules Plugin ======================================
//= @Maintenance mod
//===== By: ==================================================
//= AnnieRuru
//===== Current Version: =====================================
//= 1.3
//===== Compatible With: ===================================== 
//= Hercules 2014-09-25
//===== Description: =========================================
//= Turn the server into maintenance mode.
//= -> blocked players from login the game for a set of duration
//= a GM will announce the maintenance, and players will be kicked out after a while for maintenance. 
//= Player below {Group ID can login} will not able to login for the duration
//= but GM99 can always login without restriction
//===== Topic ================================================
//= http://hercules.ws/board/topic/7127-
//===== Additional Comments: =================================  
//= -- AtCommands --
//= @maintenance <Group ID can stay 1~99> <duration to kick in minute> <maintenance duration in minute> <reason>
//= @maintenanceoff
//=
//= -- Script Commands --
//= maintenance <Group ID can stay 1~99>, <duration to kick in minute>, <maintenance duration in minute> { , <reason> };
//=   reason field is optional, default to '*Regular server maintenance*'
//= maintenanceoff { <reason> };
//=   reason field is optional, default to '*maintenanceoff*'
//= maintenancecheck( <type> );
//=   default:
//=	    return 0 if server is normal
//=	    return 1 if server is going to have maintenance
//=	    return 2 if server is having maintenance
//=   type 1:
//=	    return the Group ID that has been denied to login
//=   type 2:
//=     return the time stamp of last/current starting time for the maintenance
//=   type 3:
//=     return the time stamp of last/current ending time for the maintenance
//=   type 4:
//=     return the ID of last used maintenance_countid ( for debugging only )
//=   type 5:
//=     return the ID of last used maintenance_timerid ( for debugging only )
//============================================================

/*	Remember to build a Sql table !
create table maintenance (
	id int(11) primary key auto_increment,
	account_id int(11),
	name varchar(23),
	reason varchar(99),
	minlv2connect tinyint(4),
	order_time datetime,
	start_time datetime,
	end_time datetime
) engine = innodb;
*/

//	change the announcement color
int maintenance_color = 0xFFFF00;
//	If you still want to personalize each announcement to different color, just Ctrl+F 'maintenance_color' below

//	==========================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../map/pc.h"
#include "../map/map.h"
#include "../map/intif.h"
#include "../common/HPMi.h"
#include "../common/timer.h"
#include "../common/strlib.h"
#include "../common/malloc.h"
#include "../common/HPMDataCheck.h" /* should always be the last file included! (if you don't make it last, it'll intentionally break compile time) */

HPExport struct hplugin_info pinfo = {
	"maintenance",	// Plugin name
	SERVER_TYPE_MAP,// Which server types this plugin works with?
	"1.0",			// Plugin version
	HPM_VERSION,	// HPM Version (don't change, macro is automatically updated)
};

int maintenance_group = 0;
int maintenance_starttime = 0;
int maintenance_endtime = 0;
int maintenance_countid = INVALID_TIMER;
int maintenance_timerid = INVALID_TIMER;
char maintenance_timeformat[24];

int maintenance_progress( int tid, int64 tick, int id, intptr data ) {
	char output[CHAT_SIZE_MAX];
	timer->delete( maintenance_timerid, maintenance_progress );
	maintenance_timerid = INVALID_TIMER;
	safesnprintf( output, CHAT_SIZE_MAX, "Maintenance mode has ended. Players are able to login now." );
	intif->broadcast2( output, strlen(output) +1, maintenance_color, 0x190, 12, 0, 0);
	maintenance_group = 0;
	maintenance_starttime = (int)time(NULL);
	maintenance_endtime = (int)time(NULL);
	ShowStatus( CL_YELLOW "Maintenance has ended." CL_RESET "\n" );
	return false;
}

int maintenance_countdown( int tid, int64 tick, int id, intptr data ) {
	char output[CHAT_SIZE_MAX];
	int countdown = maintenance_starttime - (int)time(NULL);
	if ( countdown > 90 ) {
		safesnprintf( output, CHAT_SIZE_MAX, "Maintenance will start in %d minutes", countdown /60 );
		intif->broadcast2( output, strlen(output) +1, maintenance_color, 0x190, 12, 0, 0);
		timer->delete( maintenance_countid, maintenance_countdown );
		if ( ( countdown % 60 ) > 0 ) // fine tune the timer
			maintenance_countid = timer->add( timer->gettick() + ( ( countdown % 60 ) * 1000 ), maintenance_countdown, 0, 0 );
		else
			maintenance_countid = timer->add( timer->gettick() + 60000, maintenance_countdown, 0, 0 );
	}
	else if ( countdown > 15 ) {
		safesnprintf( output, CHAT_SIZE_MAX, "Maintenance will start in %d seconds", countdown );
		intif->broadcast2( output, strlen(output) +1, maintenance_color, 0x190, 12, 0, 0);
		timer->delete( maintenance_countid, maintenance_countdown );
		if ( ( countdown % 10 ) > 0 ) // fine tune the timer
			maintenance_countid = timer->add( timer->gettick() + ( ( countdown % 10 ) * 1000 ), maintenance_countdown, 0, 0 );
		else
			maintenance_countid = timer->add( timer->gettick() + 10000, maintenance_countdown, 0, 0 );
	}
	else if ( countdown > 0 ) {
		safesnprintf( output, CHAT_SIZE_MAX, "Maintenance will start in %d seconds", countdown );
		intif->broadcast2( output, strlen(output) +1, maintenance_color, 0x190, 12, 0, 0);
		timer->delete( maintenance_countid, maintenance_countdown );
		maintenance_countid = timer->add( timer->gettick() + 1000, maintenance_countdown, 0, 0 );
	}
	else {
		struct s_mapiterator* iter = mapit->alloc( MAPIT_NORMAL, BL_PC );
		TBL_PC *sd;
		safesnprintf( output, CHAT_SIZE_MAX, "Maintenance starts now. Every player will be kick out." );
		intif->broadcast2( output, strlen(output) +1, maintenance_color, 0x190, 12, 0, 0);
		for ( sd = (TBL_PC*)mapit->first(iter); mapit->exists(iter); sd = (TBL_PC*)mapit->next(iter) )
			if ( sd->group_id < maintenance_group )
				clif->authfail_fd( sd->fd, 1 );
		mapit->free(iter);
		timer->delete( maintenance_countid, maintenance_countdown );
		maintenance_countid = INVALID_TIMER;
		maintenance_timerid = timer->add( timer->gettick() + ( ( maintenance_endtime - maintenance_starttime )*1000 ), maintenance_progress, 0, 0 );
		ShowStatus( CL_YELLOW "Maintenance has started." CL_RESET "\n" );
	}
	return false;
}

HPExport void server_online (void) {
	if ( SQL->Query( map->mysql_handle, "select minlv2connect, unix_timestamp( start_time ), unix_timestamp( end_time ), weekday( end_time ), hour( end_time ), minute( end_time ) from maintenance where id = ( select max(id) from maintenance )" ) == SQL_ERROR )
		Sql_ShowDebug( map->mysql_handle );
	else if ( SQL->NextRow( map->mysql_handle ) == SQL_SUCCESS ) {
		char *data;
		short weekday = 0, hour = 0, minute = 0;
		char* weekdayname[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
		char am_pm[3], min_display[3];
		if ( SQL->GetData( map->mysql_handle, 0, &data, NULL ) == SQL_SUCCESS )
			maintenance_group = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 1, &data, NULL ) == SQL_SUCCESS )
			maintenance_starttime = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 2, &data, NULL ) == SQL_SUCCESS )
			maintenance_endtime = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 3, &data, NULL ) == SQL_SUCCESS )
			weekday = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 4, &data, NULL ) == SQL_SUCCESS )
			hour = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 5, &data, NULL ) == SQL_SUCCESS )
			minute = atoi(data);
		if ( hour == 0 ) {
			hour = 12;
			safesnprintf( am_pm, 3, "AM" );
		}
		else if ( hour < 12 )
			safesnprintf( am_pm, 3, "AM" );
		else {
			hour = hour - 12;
			safesnprintf( am_pm, 3, "PM" );
		}
		if ( minute < 10 )
			safesnprintf( min_display, 3, "0%d", minute );
		else
			safesnprintf( min_display, 3, "%d", minute );
		safesnprintf( maintenance_timeformat, 24, "%s, %d:%s %s", weekdayname[ weekday ], hour, min_display, am_pm );
	}
	if ( maintenance_starttime > (int)time(NULL) ) {
		int countdown = maintenance_starttime - (int)time(NULL);
		if ( countdown > 60 )
			maintenance_countid = timer->add( timer->gettick() + ( ( countdown % 60 )*1000 ), maintenance_countdown, 0, 0 );
		else
			maintenance_countid = timer->add( timer->gettick() + 1000, maintenance_countdown, 0, 0 );
		ShowStatus( CL_YELLOW "Maintenance will start in %d min %d sec." CL_RESET "\n", countdown /60, countdown %60 );
	}
	else if ( maintenance_endtime > (int)time(NULL) ) {
		int countdown = maintenance_endtime - (int)time(NULL);
		maintenance_timerid = timer->add( timer->gettick() + ( countdown *1000 ), maintenance_progress, 0, 0 );
		ShowStatus( CL_YELLOW "Maintenance will end in %d min %d sec on %s." CL_RESET "\n", countdown/60, countdown %60, maintenance_timeformat );
	}
}

bool pc_authok_pre( struct map_session_data *sd, int *login_id2, time_t *expiration_time, int *group_id, struct mmo_charstatus *st, bool *changing_mapservers ) {
	if ( (int)time(NULL) > maintenance_starttime && maintenance_endtime > (int)time(NULL) && *group_id < maintenance_group ) {
		clif->authfail_fd( sd->fd, 1 );
		hookStop();
		return false;
	}
	return true;
}

void clif_parse_LoadEndAck_pre( int *fd, struct map_session_data *sd ) {
	if ( sd->state.connect_new ) {
		if ( maintenance_starttime > (int)time(NULL) ) {
			char output[255];
			safesnprintf( output, 255, "Maintenance starts in %d min %d sec, last %d minutes. Server up by %s", ( maintenance_starttime - (int)time(NULL) ) /60, ( maintenance_starttime - (int)time(NULL) ) %60, ( maintenance_endtime - maintenance_starttime ) /60, maintenance_timeformat );
			clif->message( sd->fd, output );
		}
		else if ( maintenance_endtime > (int)time(NULL) ) {
			char output[255];
			safesnprintf( output, 255, "Server is currently in maintenance mode, will end in %d min %d sec on %s", ( maintenance_endtime - (int)time(NULL) ) /60, ( maintenance_endtime - (int)time(NULL) ) %60, maintenance_timeformat );
			clif->message( sd->fd, output );
		}
	}
	return;
}

ACMD(maintenance) {
	int group_id = 0, kick_duration = 0, maintenance_duration = 0;
	char reason[99], esc_reason[198], esc_name[46], min_display[3], output[CHAT_SIZE_MAX];
	short weekday = 0, hour = 0, minute = 0;
	if ( maintenance_starttime > (int)time(NULL) ) {
		int countdown = maintenance_starttime - (int)time(NULL);
		clif->message( sd->fd, "Type '@maintenanceoff' to turn off maintenance mode." );
		safesnprintf( output, CHAT_SIZE_MAX, "Maintenance mode will start in %d min %d sec.", countdown /60, countdown %60 );
		clif->message( sd->fd, output );
		return true;
	}
	if ( maintenance_endtime > (int)time(NULL) ) {
		int countdown = maintenance_endtime - (int)time(NULL);
		clif->message( sd->fd, "Type '@maintenanceoff' to turn off maintenance mode." );
		safesnprintf( output, CHAT_SIZE_MAX, "Server is currently in maintenance mode, will end in %d min %d sec on %s", countdown /60, countdown %60, maintenance_timeformat );
		clif->message( sd->fd, output );
		return true;
	}
	if ( !message || !*message ) {
		clif->message( sd->fd, "@maintenance Syntax :" );
		clif->message( sd->fd, "@maintenance <Group ID can stay 1~99> <duration to kick in minute> <maintenance duration in minute> <reason>" );
		return false;
	}
//	WTF ! sscanf can't be used ??
//	if ( sscanf( message, "%d %d %d %99[^\n]", &group_id, &kick_duration, &maintenance_duration, &reason ) < 4 ) {
//		clif->message( sd->fd, "@maintenance Syntax :" );
//		clif->message( sd->fd, "@maintenance <Group ID can stay 1~99> <duration to kick in minute> <maintenance duration in minute> <reason>" );
//		return false;
//	}
//	now has to do the stupid string calculation <_<
	{
		int i = 0, j = 0, k = 0, l = strlen( message );
		char *temp = (char*)aMalloc( strlen( message ) +1 );
		while ( i <= l && k < 3 ) {
			if ( message[i] != ' ' && message[i] != '\0' ) {
				temp[j++] = message[i];
			}
			else if ( message[i-1] != ' ' ) {
				temp[j] = '\0';
				if ( k == 0 )
					group_id = atoi( temp );
				else if ( k == 1 )
					kick_duration = atoi( temp );
				else if ( k == 2 )
					maintenance_duration = atoi( temp );
				k++;
				j = 0;
			}
			i++;
		}
		safestrncpy( reason, &message[i], 99 );
		aFree( temp );
		if ( k < 3 ) {
			clif->message( sd->fd, "@maintenance Syntax :" );
			clif->message( sd->fd, "@maintenance <Group ID can stay 1~99> <duration to kick in minute> <maintenance duration in minute> <reason>" );
			return false;
		}
	}
	if ( !group_id ) {
		clif->message( sd->fd, "The Group ID field cannot be 0, otherwise normal player able to login." );
		return false;
	}
	if ( group_id < 1 || group_id > 99 ) {
		safesnprintf( output, 255, "Invalid Group ID %d. Range must between 1~99.", group_id );
		clif->message( sd->fd, output );
		return false;
	}
	if ( kick_duration <= 0 ) {
		clif->message( sd->fd, "Kick duration cannot be 0 or negative numbers." );
		return false;
	}
	if ( kick_duration > 1440 ) {
		clif->message( sd->fd, "Kick duration cannot be more than 1 day." );
		return false;
	}
	if ( maintenance_duration <= 0 ) {
		clif->message( sd->fd, "Maintenance duration cannot be 0 or negative numbers." );
		return false;
	}
	if ( maintenance_duration > 10080 ) {
		clif->message( sd->fd, "Maintenance duration cannot be more than 1 week." );
		return false;
	}
	if ( safestrnlen( reason, 99 ) < 4 ) {
		clif->message( sd->fd, "You must input a valid reason for doing this." );
		return false;
	}
	SQL->EscapeString( map->mysql_handle, esc_name, sd->status.name );
	SQL->EscapeString( map->mysql_handle, esc_reason, reason );
	if ( SQL->Query( map->mysql_handle, "insert into maintenance values ( null, %d, '%s', '%s', %d, now(), timestampadd( minute, %d, now() ), timestampadd( minute, %d, now() ) )", sd->status.account_id, esc_name, esc_reason, group_id, kick_duration, kick_duration + maintenance_duration ) == SQL_ERROR ) {
		Sql_ShowDebug( map->mysql_handle );
		return false;
	}
	if ( SQL->Query( map->mysql_handle, "select unix_timestamp( start_time ), unix_timestamp( end_time ), weekday( end_time ), hour( end_time ), minute( end_time ) from maintenance where id = ( select max(id) from maintenance )" ) == SQL_ERROR ) {
		Sql_ShowDebug( map->mysql_handle );
		return false;
	}
	else if ( SQL->NextRow( map->mysql_handle ) == SQL_SUCCESS ) {
		char *data;
		maintenance_group = group_id;
		if ( SQL->GetData( map->mysql_handle, 0, &data, NULL ) == SQL_SUCCESS )
			maintenance_starttime = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 1, &data, NULL ) == SQL_SUCCESS )
			maintenance_endtime = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 2, &data, NULL ) == SQL_SUCCESS )
			weekday = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 3, &data, NULL ) == SQL_SUCCESS )
			hour = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 4, &data, NULL ) == SQL_SUCCESS )
			minute = atoi(data);
		SQL->FreeResult( map->mysql_handle );
	}
	else {
		SQL->FreeResult( map->mysql_handle );
		return false;
	}
	{	// stupid date_format doesn't work
		char* weekdayname[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
		char am_pm[3];
		if ( hour == 0 ) {
			hour = 12;
			safesnprintf( am_pm, 3, "AM" );
		}
		else if ( hour < 12 )
			safesnprintf( am_pm, 3, "AM" );
		else {
			hour = hour - 12;
			safesnprintf( am_pm, 3, "PM" );
		}
		if ( minute < 10 )
			safesnprintf( min_display, 3, "0%d", minute );
		else
			safesnprintf( min_display, 3, "%d", minute );
		safesnprintf( maintenance_timeformat, 24, "%s, %d:%s %s", weekdayname[ weekday ], hour, min_display, am_pm );
		safesnprintf( output, CHAT_SIZE_MAX, "Maintenance mode will be commence in %d minutes. Players are adviced to log out right now. Maintenance last %d minutes. Server will come back up on %s", kick_duration, maintenance_duration, maintenance_timeformat );
		intif->broadcast2( output, strlen(output) +1, maintenance_color, 0x190, 12, 0, 0);
	}
	maintenance_countid = timer->add( timer->gettick() +( ( kick_duration == 1 )? 1000 : 60000 ), maintenance_countdown, 0, 0 );
	ShowStatus( CL_YELLOW "Maintenance will start in %d min by " CL_GREEN "%s" CL_RESET ".\n", kick_duration, sd->status.name );
	return true;
}

ACMD(maintenanceoff) {
	char esc_name[46], output[CHAT_SIZE_MAX];
	if ( maintenance_endtime <= (int)time(NULL) ) {
		clif->message( sd->fd, "The server is currently in not in maintenance mode." );
		return true;
	}
	SQL->EscapeString( map->mysql_handle, esc_name, sd->status.name );
	if ( SQL->Query( map->mysql_handle, "insert into maintenance values ( null, %d, '%s', '   @maintenanceoff', 0, null, now(), now() )", sd->status.account_id, esc_name ) == SQL_ERROR ) {
		Sql_ShowDebug( map->mysql_handle );
		return false;
	}
	maintenance_group = 0;
	maintenance_starttime = (int)time(NULL);
	maintenance_endtime = (int)time(NULL);
	if ( maintenance_countid != INVALID_TIMER ) {
		timer->delete( maintenance_countid, maintenance_countdown );
		maintenance_countid = INVALID_TIMER;
	}
	if ( maintenance_timerid != INVALID_TIMER ) {
		timer->delete( maintenance_timerid, maintenance_progress );
		maintenance_timerid = INVALID_TIMER;
	}
	safesnprintf( output, CHAT_SIZE_MAX, "%s ended the Maintenance mode. Players are able to login now.", sd->status.name );
	intif->broadcast2( output, strlen(output) +1, maintenance_color, 0x190, 12, 0, 0);
	ShowStatus( CL_YELLOW "Maintenance has ended by " CL_GREEN "%s" CL_RESET ".\n", sd->status.name );
	return true;
}

//	maintenance <Group ID can stay 1~99>, <duration to kick in minute>, <maintenance duration in minute> { , <reason> };
BUILDIN(maintenance) {
	int group_id = 0, kick_duration = 0, maintenance_duration = 0;
	char reason[99], esc_reason[198], min_display[3];
	short weekday = 0, hour = 0, minute = 0;
	if ( maintenance_endtime > (int)time(NULL) ) {
		ShowError( "buildin_maintenance: Maintenance is currently ON.\n" );
		return false;
	}
	group_id = script_getnum(st,2);
	kick_duration = script_getnum(st,3);
	maintenance_duration = script_getnum(st,4);
	if ( !group_id ) {
		ShowError( "buildin_maintenance: Group ID field cannot be 0.\n" );
		return false;
	}
	if ( group_id < 1 || group_id > 99 ) {
		ShowError( "buildin_maintenance: Invalid Group ID %d. Range must between 1~99.\n", group_id );
		return false;
	}
	if ( kick_duration <= 0 ) {
		ShowError( "buildin_maintenance: Kick duration cannot be 0 or negative numbers.\n" );
		return false;
	}
	if ( kick_duration > 1440 ) {
		ShowError( "buildin_maintenance: Kick duration cannot be more than 1 day.\n" );
		return false;
	}
	if ( maintenance_duration <= 0 ) {
		ShowError( "buildin_maintenance: Maintenance duration cannot be 0 or negative numbers.\n" );
		return false;
	}
	if ( maintenance_duration > 10080 ) {
		ShowError( "buildin_maintenance: Maintenance duration cannot be more than 1 week.\n" );
		return false;
	}
	safesnprintf( reason, 99, script_hasdata(st,5)? script_getstr(st,5) : "   *Regular server maintenance*" );
	SQL->EscapeString( map->mysql_handle, esc_reason, reason );
	if ( SQL->Query( map->mysql_handle, "insert into maintenance values ( null, 0, 'NPC', '%s', %d, now(), timestampadd( minute, %d, now() ), timestampadd( minute, %d, now() ) )", esc_reason, group_id, kick_duration, kick_duration + maintenance_duration ) == SQL_ERROR ) {
		Sql_ShowDebug( map->mysql_handle );
		return false;
	}
	if ( SQL->Query( map->mysql_handle, "select unix_timestamp( start_time ), unix_timestamp( end_time ), weekday( end_time ), hour( end_time ), minute( end_time ) from maintenance where id = ( select max(id) from maintenance )" ) == SQL_ERROR ) {
		Sql_ShowDebug( map->mysql_handle );
		return false;
	}
	else if ( SQL->NextRow( map->mysql_handle ) == SQL_SUCCESS ) {
		char *data;
		maintenance_group = group_id;
		if ( SQL->GetData( map->mysql_handle, 0, &data, NULL ) == SQL_SUCCESS )
			maintenance_starttime = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 1, &data, NULL ) == SQL_SUCCESS )
			maintenance_endtime = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 2, &data, NULL ) == SQL_SUCCESS )
			weekday = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 3, &data, NULL ) == SQL_SUCCESS )
			hour = atoi(data);
		if ( SQL->GetData( map->mysql_handle, 4, &data, NULL ) == SQL_SUCCESS )
			minute = atoi(data);
		SQL->FreeResult( map->mysql_handle );
	}
	else {
		SQL->FreeResult( map->mysql_handle );
		return false;
	}
	{	// stupid date_format doesn't work
		char* weekdayname[7] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
		char am_pm[3], output[CHAT_SIZE_MAX];
		if ( hour == 0 ) {
			hour = 12;
			safesnprintf( am_pm, 3, "AM" );
		}
		else if ( hour < 12 )
			safesnprintf( am_pm, 3, "AM" );
		else {
			hour = hour - 12;
			safesnprintf( am_pm, 3, "PM" );
		}
		if ( minute < 10 )
			safesnprintf( min_display, 3, "0%d", minute );
		else
			safesnprintf( min_display, 3, "%d", minute );
		safesnprintf( maintenance_timeformat, 24, "%s, %d:%s %s", weekdayname[ weekday ], hour, min_display, am_pm );
		safesnprintf( output, CHAT_SIZE_MAX, "Maintenance mode will be commence in %d minutes. Players are adviced to log out right now. Maintenance last %d minutes. Server will come back up on %s", kick_duration, maintenance_duration, maintenance_timeformat );
		intif->broadcast2( output, strlen(output) +1, maintenance_color, 0x190, 12, 0, 0);
	}
	maintenance_countid = timer->add( timer->gettick() +( ( kick_duration == 1 )? 1000 : 60000 ), maintenance_countdown, 0, 0 );
	ShowStatus( CL_YELLOW "Maintenance will start in %d min" CL_RESET ".\n", kick_duration );
	return true;
}

BUILDIN(maintenanceoff) {
	char reason[99], esc_reason[198];
	if ( maintenance_endtime <= (int)time(NULL) ) {
		ShowError( "buildin_maintenanceoff: Maintenance is currently OFF.\n" );
		return false;
	}
	safesnprintf( reason, 99, script_hasdata(st,2)? script_getstr(st,2) : "   *maintenance off*" );
	SQL->EscapeString( map->mysql_handle, esc_reason, reason );
	if ( SQL->Query( map->mysql_handle, "insert into maintenance values ( null, 0, 'NPC', '%s', 0, null, now(), now() )", esc_reason ) == SQL_ERROR ) {
		Sql_ShowDebug( map->mysql_handle );
		return false;
	}
	maintenance_group = 0;
	maintenance_starttime = (int)time(NULL);
	maintenance_endtime = (int)time(NULL);
	if ( maintenance_countid != INVALID_TIMER ) {
		timer->delete( maintenance_countid, maintenance_countdown );
		maintenance_countid = INVALID_TIMER;
	}
	if ( maintenance_timerid != INVALID_TIMER ) {
		timer->delete( maintenance_timerid, maintenance_progress );
		maintenance_timerid = INVALID_TIMER;
	}
	intif->broadcast2( "Maintenance mode has ended. Players are able to login now." , 255, maintenance_color, 0x190, 12, 0, 0);
	ShowStatus( CL_YELLOW "Maintenance has ended" CL_RESET ".\n" );
	return true;
}

BUILDIN(maintenancecheck) {
	int type = script_hasdata(st,2)? script_getnum(st,2) : 0;
	switch( type ) {
	default:
		if ( maintenance_starttime > (int)time(NULL) )
			script_pushint(st, 1);
		else if ( maintenance_endtime > (int)time(NULL) )
			script_pushint(st, 2);
		else
			script_pushint(st, 0);
		return true;
	case 1:
		script_pushint(st, maintenance_group);
		return true;
	case 2:
		script_pushint(st, maintenance_starttime);
		return true;
	case 3:
		script_pushint(st, maintenance_endtime);
		return true;
	case 4:
		script_pushint(st, maintenance_countid);
		return true;
	case 5:
		script_pushint(st, maintenance_timerid);
		return true;
//	case 6: // for some reason this cause memory leak
//		script_pushstr(st, maintenance_timeformat );
//		return true;
	}
}

HPExport void plugin_init (void) {
	clif = GET_SYMBOL("clif");
	timer = GET_SYMBOL("timer");
	intif = GET_SYMBOL("intif");
	strlib = GET_SYMBOL("strlib");
	SQL = GET_SYMBOL("SQL");
	mapit = GET_SYMBOL("mapit");
	map = GET_SYMBOL("map");
	script = GET_SYMBOL("script");
	iMalloc = GET_SYMBOL("iMalloc");

	addHookPre( "pc->authok", pc_authok_pre );
	addHookPre( "clif->pLoadEndAck", clif_parse_LoadEndAck_pre );

	addAtcommand( "maintenance", maintenance );
	addAtcommand( "maintenanceoff", maintenanceoff );

	addScriptCommand( "maintenance", "iii?", maintenance );
	addScriptCommand( "maintenanceoff", "?", maintenanceoff );
	addScriptCommand( "maintenancecheck", "?", maintenancecheck );
}