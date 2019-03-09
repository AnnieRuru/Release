I always thought there are only 2 methods to write a PVP ladder script

#### Method 1:
Use Sql to log the number of kills and death of each character ID
```sql
create table pvpladder (
	char_id int(11) primary key,
	name varchar(23),
	kills int(11),
	key (kills)
) engine = innodb;
```
```c
prontera,150,185,5	script	sql_ladder	1_F_MARIA,{
	mes "Top 5";
	.@query$  = "SELECT `name`, IF(@d=t.`kills`, @r, @r:=@i), @d:=t.`kills`, @i:=@i+1 ";
	.@query$ += "FROM `pvpladder` t, (SELECT @d:=0, @r:=0, @i:=1)q ";
	.@query$ += "ORDER BY `kills` DESC LIMIT 5";
	.@nb = query_sql(.@query$, .@name$, .@rank, .@kills, .@dummy);
	if ( !.@nb ) {
		mes "no entry";
		close;
	}
	for ( .@i = 0; .@i < .@nb; ++.@i )
		mes "No."+ .@rank[.@i] +" ["+ .@name$[.@i] +"] ~ "+ .@kills[.@i] +" kills";
	next;
	if ( !query_sql( "SELECT `kills`, 1+(SELECT COUNT(1) FROM `pvpladder` t1 WHERE t1.`kills` > t2.`kills`) FROM `pvpladder` t2 WHERE `char_id` = "+ getcharid(CHAR_ID_CHAR), .@kills, .@rank ) ) {
		mes "You are not in the rank.";
		close;
	}
	mes "Your Total kills -> "+ .@kills;
	mes "Your current rank -> "+ .@rank;
	close;
OnPCKillEvent:
	if ( killedrid == getcharid(CHAR_ID_ACCOUNT) ) end; // kill self doesn't increase count. EG: Grand-cross
	query_sql "insert into pvpladder values ( "+ getcharid(CHAR_ID_CHAR) +", '"+ escape_sql( strcharinfo(PC_NAME) ) +"', 1 ) on duplicate key update kills = kills +1";
	end;
}
```

#### Method 2:
TXT style ladder, kinda similar to [Terces PVP ladder](https://github.com/AnnieRuru/customs/blob/master/scripts/tercer_pvp_ladder.txt) ([topic](http://herc.ws/board/topic/10967-req-top-leader-custom-mvp-hunter/#comment-65307))
```c
prontera,152,185,5	script	txt_ladder	1_F_MARIA,{
	mes "Top 5";
	.@size = getarraysize( $pvpladder_kills );
	if ( .@size ) {
		for ( .@i = 0; .@i < .@size; ++.@i )
			mes "No."+ ( .@i +1 ) +" ["+ $pvpladder_name$[.@i] +"] ~ "+ $pvpladder_kills[.@i] +" kills";
	}
	else
		mes "no entry";
	next;
	if ( !pvpladder_kills )
		mes "You are not in the rank.";
	else
		mes "Your Total kills -> "+ pvpladder_kills;
	close;
OnInit:
	.ladderlength = 5; // how many entry shown on the ranking board ?
	end;
OnPCKillEvent:
	if ( killedrid == getcharid(CHAR_ID_ACCOUNT) ) end; // kill self doesn't increase count. EG: Grand-cross
	++pvpladder_kills;
	for ( .@i = 0; .@i < .ladderlength; ++.@i ) {
		if ( pvpladder_kills > $pvpladder_kills[.@i] ) { // if found
			if ( $pvpladder_cid[.@i] == getcharid(CHAR_ID_CHAR) ) { // update your own points
				$pvpladder_kills[.@i] = pvpladder_kills;
				break;
			}
			if ( .@i == .ladderlength -1 ) { // last position of the ladder, just overwrite the value
				$pvpladder_kills[.@i] = pvpladder_kills;
				$pvpladder_name$[.@i] = strcharinfo(PC_NAME);
				$pvpladder_cid[.@i] = getcharid(CHAR_ID_CHAR);
				break;
			}
			// if more points, but somehow its not your own name
			for ( .@j = .@i +1; .@j < .ladderlength; ++.@j ) {
				if ( $pvpladder_name$[.@j] == strcharinfo(PC_NAME) ) { // found your name
					.@moveamount = .@j - .@i; // save amount to move
					break;
				}
			}
			if ( !.@moveamount ) // if not found ...
				.@moveamount = .ladderlength - 1 - .@i; // means move the whole ladder from the last index
			copyarray $pvpladder_kills[.@i +1], $pvpladder_kills[.@i], .@moveamount;
			copyarray $pvpladder_name$[.@i +1], $pvpladder_name$[.@i], .@moveamount;
			copyarray $pvpladder_cid[.@i +1], $pvpladder_cid[.@i], .@moveamount;
			$pvpladder_kills[.@i] = pvpladder_kills;
			$pvpladder_name$[.@i] = strcharinfo(PC_NAME);
			$pvpladder_cid[.@i] = getcharid(CHAR_ID_CHAR);
			break;
		}
	}
	end;
}
```
#### Method 3:
but after viewing [this topic](https://rathena.org/board/topic/117819-add-death-count-on-pvp-ranking/)  
this is another Method No.3 that logs every kills made by player
```sql
create table pvplog (
	timstamp datetime,
	killer int(11),
	killed int(11),
	key (killer)
) engine myisam;
```
```c
prontera,154,185,5	script	pvp_log_ladder	1_F_MARIA,{
	mes "Top 5";
	.@nb = query_sql( "select count(1) as kills, (select name from `char` where char_id = killer) from pvplog group by killer order by kills desc limit 5", .@kills, .@name$ );
	if ( !.@nb ) {
		mes "no entry";
		close;
	}
	for ( .@i = 0; .@i < .@nb; ++.@i )
		mes "No."+( .@i +1 )+" ["+ .@name$[.@i] +"] ~ "+ .@kills[.@i] +" kills";
	close;
OnPCKillEvent:
	query_sql "insert into pvplog values ( now(), "+ getcharid(CHAR_ID_CHAR) +", "+ getcharid(0, rid2name(killedrid)) +" )";
	end;
}
```
I start to think this is the preferred method for PVP ladder script...  
instead of just adding the kill counter, this actually logs player's action 
