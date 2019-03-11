all right, I post up my custom stylist

### Download: 1.5
script


this one use CSV format

```c
	.list$[LOOK_HAIR] = "0-5,11-18,20-23";
	.list$[Job_Summoner + LOOK_CLOTHES_COLOR] = "0-1";
 ```

This means, the ID range for hair style is 0,1,2,3,4,5,11,12,13,14,15,16,17,18,20,21,22,23  
2nd line means Summoner job can only use clothing range 0 and 1, this prevent crash client

<details>
<summary>Revision History</summary>

1.0 - script
- there is a bug on Euphy's stylist npc, the style number should start with 0, but his script always start with 1
- and the naming sense very bad, .@style and .@styles, should be avoided

1.1 - script
- allow blacklist certain styles

1.2 - script
- change from blacklist into CSV format, this one is understood by many

1.3 - script
- add Job_Summoner, to prevent client crash from invalid cloth color

1.4 - script
- add 3rd job body style change

1.5 - script
- fix a bug if the player start with style number 510, the server throw you infinite loop error
</details>
