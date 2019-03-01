Original Idea by both Myriad and 4144, brainstorm together in these 2 PR  
[equipidx](https://github.com/HerculesWS/Hercules/pull/2355) and [delitemidx](https://github.com/HerculesWS/Hercules/pull/2394)  
It was nearly going to release until we found out the inventory index can became empty  
and thus those 2 PR are probably not going to be accepted ...

#### *getinventorylistidx
```c
*getinventorylistidx()

Similar to 'getinventorylist', this command also set a bunch of arrays with a
complete list of the invoking character has in their inventory. But this return
in inventory index similar to source code 'sd->status.inventory[i]' where player's
inventory can have empty index whenever player remove an item.

@inventorylistidx_id[]         - array of item ids.
@inventorylistidx_amount[]     - their corresponding item amounts.
@inventorylistidx_equip[]      - will return the slot the item is equipped on, if at all.
@inventorylistidx_refine[]     - for how much it is refined.
@inventorylistidx_identify[]   - whether it is identified.
@inventorylistidx_attribute[]  - whether it is broken.
@inventorylistidx_card1[]      - These four arrays contain card data for the items.
@inventorylistidx_card2[]        These data slots are also used to store names 
@inventorylistidx_card3[]        inscribed on the items, so you can explicitly 
@inventorylistidx_card4[]        check if the character owns an item
                                 made by a specific craftsman.
@inventorylistidx_expire[]     - expire time (Unix time stamp). 0 means never expires.
@inventorylistidx_bound        - whether it is an account bounded item or not.
@inventorylistidx_opt_id1[]    - first array of item option IDs
@inventorylistidx_opt_val1[]   - first array of item option values
@inventorylistidx_opt_param1[] - first array of item option parameters
@inventorylistidx_opt_id2[]    - second array of item option IDs
@inventorylistidx_opt_val2[]   - second array of item option values
@inventorylistidx_opt_param2[] - second array of item option parameters
@inventorylistidx_opt_id3[]    - third array of item option IDs
@inventorylistidx_opt_val3[]   - third array of item option values
@inventorylistidx_opt_param3[] - third array of item option parameters
@inventorylistidx_opt_id4[]    - fourth array of item option IDs
@inventorylistidx_opt_val4[]   - fourth array of item option values
@inventorylistidx_opt_param4[] - fourth array of item option parameters
@inventorylistidx_opt_id5[]    - fifth array of item option IDs
@inventorylistidx_opt_val5[]   - fifth array of item option values
@inventorylistidx_opt_param5[] - fifth array of item option parameters
@inventorylistidx_canequip[]   - check the item at the index can be use by current job.
                                 return 1 if the condition is true, return 0 otherwise

For more examples, see 'delitemidx' and 'equipidx'.
```

#### *delitemidx
```c
*delitemidx(<index>{, <amount>{, <account id>}})

This command will remove an item at the given inventory index. Unlike the 'delitem()'
counterpart, this doesn't check invalid Item ID, making it useful to remove invalid
item IDs in player's inventory.

If <amount> is not specified, this will remove all of the items at the specified index.
Note that items with the 'ForceSerial' flag, not yet merged through 'mergeitem()', will
only be removed at the given index.

The only way to get the inventory index is by using 'getinventorylistidx()'. After deleting
an item at the given index, that index can remain empty until the player relogs, so you
should recall 'getinventorylistidx()' again. If you try to delete an item at an invalid index,
the script will terminate with an error.

This command is also useful to remove rental/bound items because 'delitem()' does not
discriminate at choosing which item to remove.

Example:

//	This will remove all invalid Item ID in player's inventory
	getinventorylistidx();
	.@inventorysize = getInventorySize();
	for (.@i = 0; .@i < .@inventorysize; ++.@i)
		if (getiteminfo(@inventorylist_id[.@i], ITEMINFO_TYPE) == -1)
			delitemidx(.@i);
```

#### *equipidx
```c
*equipidx(<index>)

This function will attempt to equip the item at the given inventory index.
This feature is only really useful when multiple instances of an Item ID
exist in the player's inventory.

See 'getinventorylistidx' for an explanation on retrieving an inventory index.

Note the '@inventorylistidx_canequip' can be use to check if the player can
equip the item at <index> in inventory.

Examples:

//	This will prioritize equip a rental Knife
	getinventorylistidx();
	.@inventorysize = getInventorySize();
	for (.@i = 0; .@i < .@inventorysize; ++.@i) {
		if (@inventorylistidx_id[.@i] == Knife && @inventorylistidx_expire[.@i]) {
			equipidx(.@i);
			break;
		}
	}

//	This will make the player equip any equipment can be use by current job.
	getinventorylistidx();
	.@inventorysize = getInventorySize();
	for (.@i = 0; .@i < .@inventorysize; ++.@i) {
		if (@inventorylistidx_canequip == true) {
			equipidx(.@i);
			break;
		}
	}
```
