https://rathena.org/board/topic/117831-showcase-weapon-skin/.  
https://github.com/rathena/rathena/issues/1779

got people ask how to do that ...  
so I write a plugin for it

### Download: 1.1
plugin

also need to add a file in **conf/import/change_equipment_skin.conf**

```
change_equipment_skin: (
{
	Type: "LOOK_WEAPON"
	ItemID: 24001
	SkinID: 1238
},
//{
//	Type: "LOOK_SHIELD"
//	ItemID: 24002
//	SkinID: 2101
//},
)
```

When player equip a shadow weapon ID 24001, will show the weapon look 1238, which is Zeny_Knife

"@reloadequipmentskin" will reload this file ... 

------------------------------------

Note: yes, the clif->sendlook is extremely messy, I attempt to make this support change shield skin  
but for dunno what reason, equip a shield telling the client changing LOOK_WEAPON ... WTF !!  
if anybody knows how to make it support shield, I'm open to suggestion

<details>
<summary>Revision History</summary>

0.1
- initial draft

0.2
- apply the libconfig format

0.3
- apply vector

0.4 - plugin
- shield are send as LOOK_WEAPON .... bullshit

1.0 - plugin
- release with just the weapon look

1.1 - plugin
- display a message for file not found
- fix memory leak on VECTOR_ENSURE

</details>
