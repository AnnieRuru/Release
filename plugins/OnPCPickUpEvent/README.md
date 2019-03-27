```c
prontera,155,185,5	script	dfskhfs	1_F_MARIA,{
	end;
OnPCPickUpEvent:
	announce sprintf( _$( "%s has pick up %s !!" ), strcharinfo(PC_NAME), getitemname2( @pickup_itemid, @pickup_identify, @pickup_refine, @pickup_attribute, @pickup_card1, @pickup_card2, @pickup_card3, @pickup_card4 ) ), bc_all;
	end;
}
```