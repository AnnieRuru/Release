### Download: 1.6
plugin

--------------------------------

Create a market clone, to leave a message for other players  
while the player can go hunting/questing/events

```
@market "<Title>" "<Message>" <Color>
```

create a market clone with a chat room titled -> "<Title>"  
when players tries to join the chat room, it refuse the joining, but instead leave a message -> "<Message>"

with 1.4 update,  
player can now choose their own favorite color for their AFK message  
the <Color> field is optional

I was struggling to use array for the color list ...  
but in the end, I guess the simplicity is the best  
so you guys can guess how to add in your own list easily


```
@marketkill
```

kill the market clone without logging off


when a GM do @killmonster @killmonster2, or *killmonster *kilmonsterall script command will not remove the clone  
but @reloadscript, however, will remove it


feels like I just copy paste from the description <_<
whatever ...


Credit : remember to rep up Dastgir's topic because I mostly copy his codes

<details>
<summary>Revision History</summary>

1.0  
plugin.  
patch

1.1  
plugin.  
patch
- fix clone's facing direction and sitting issue

1.2  
plugin.  
patch
- fix @killmonster still able to kill market clone

1.3  
plugin.  
patch
- change back all status-damage into status-kill for readability
- fix a bug that when a server enabled show_mob_info, the clone shouldn't has listed the hp nor level
- fix a bug that player shouldn't create a market clone while chatting or vending
- and for the patch, move all configuration into battle folder, makes more sense in this way (credit: Napster)
- add zeny requirement

1.4 - plugin
- update to new plugin format
- fix possible memory leak when a player input a fail message
- change all i++ into ++i
- no longer overwrite @reloadscript, hook to npc_reload function instead
- since clif->messagecolor_self has been fixed, use that command in chat_joinchat_pre function
- add configuration for player has a cart/mount/mado, the clone can display these items as well
  - for me, it feels weird ... peco and cart is 1 thing, but warg and falcon ... ?
- now player can choose to use their favorite color in their AFK message from a list
- since all killmonster at/script functions looks the same, all of them hook to 1 same function

1.4a
- optimized the OPTION area when spawn

1.5 - plugin
- fix an issue that when using @hide, the market clone also clone as invisible
  - the real issue about this is, if that GM do @marketkill, the surrounding client will crash

1.6 - plugin
- update to latest revision
- remove all TBL_ and change to struct
- add NULL checks to places where it needed
- update the clif_charnameack with struct packet_reqnameall_ack

</details>
