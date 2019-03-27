```c
prontera,155,185,5	script	dfskhfs	1_F_MARIA,{
	end;
OnPCPickUpEvent:
	if ( @pickup_action != LOG_TYPE_PICKDROP_PLAYER && @pickup_amount > 0 )
		end;
	announce sprintf( _$( "%s has pick up %s !!" ), strcharinfo(PC_NAME), getitemname2( @pickup_itemid, @pickup_identify, @pickup_refine, @pickup_attribute, @pickup_card1, @pickup_card2, @pickup_card3, @pickup_card4 ) ), bc_all;
//	dispbottom @pickup_amount +" "+ @pickup_uniqueid1 +" "+ @pickup_uniqueid2;
	end;
}
```