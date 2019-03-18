## Compare dynamic shop between rAthena & Hercules

I wonder how many of you still remember that old dynamic shop script ?  
[Sample Mithril Coin Shop](https://github.com/AnnieRuru/Release/blob/master/scripts/Quest%20%26%20Shops/mithril_coin_shop/mithril_coin_shop_1.1.txt)  
because nobody is using this method anymore ~ so long eAthena legacy ~

Hercules
```c
prontera,153,182,1	trader	TestCustom2	1_F_MARIA,{
	end;
OnInit:
	tradertype NST_CUSTOM;
	sellitem Apple,10;
	sellitem Banana,20;
	sellitem Grape,30;
	sellitem Carrot,40;
	sellitem Sweet_Potato,50;
	sellitem Yggdrasilberry,1;
	end;
OnPayFunds:
	if ( countitem(Mithril_Coin) < @price )
		end;
	delitem Mithril_Coin, @price;
	purchaseok();
	end;
OnCountFunds:
	setcurrency Mithril_Coin, 0;
	end;
}
```
rAthena
```c
prontera,156,182,5	itemshop	market	1_F_MARIA,674,512:10,513:20,514:30,515:40,516:50,607:1
```

Hercules seems longer because Hercules encourage use Item/Mob constants instead of numbers, for better readability

-----------------------------------

### callshop vs openshop

rAthena
```c
prontera,155,185,5	script	test shop	1_F_MARIA,{
	mes "Junk seller";
	next;
	switch( select( "Potions", "Herbs", "Fruits" ) ) {
	case 1: callshop "shop#potions", 1; end;
	case 2: callshop "shop#herbs", 1; end;
	case 3: callshop "shop#fruits", 1; end;
	}
	end;
}
-	itemshop	shop#potions	-1,712,501:1,502:1,503:1,504:1,505:1,506:1
-	itemshop	shop#herbs	-1,712,507:1,508:1,509:1,510:1,511:1
-	itemshop	shop#fruits	-1,712,512:1,513:1,514:1,515:1
```
Hercules
```c
prontera,155,185,5	script	test shop	1_F_MARIA,{
	mes "Junk seller";
	next;
	switch( select( "Potions", "Herbs", "Fruits" ) ) {
	dispbottom sprintf( _$( "Currently you have %d %s(%d)" ), countitem(Flower), getitemname(Flower), Flower );
	case 1: openshop "shop#potions"; end;
	case 2: openshop "shop#herbs"; end;
	case 3: openshop "shop#fruits"; end;
	}
	end;
}
-	trader	shop#potions	FAKE_NPC,{
OnInit:
	tradertype NST_CUSTOM;
	sellitem Red_Potion, 1;
	sellitem Orange_Potion, 1;
	sellitem Yellow_Potion, 1;
	sellitem White_Potion, 1;
	sellitem Blue_Potion, 1;
	sellitem Green_Potion, 1;
	end;
OnCountFunds:
	setcurrency Flower;
	end;
OnPayFunds:
    if ( countitem(Flower) < @price )
        end;
    delitem Flower, @price;
    purchaseok();
    end;
}
-	trader	shop#herbs	FAKE_NPC,{
OnInit:
	tradertype NST_CUSTOM;
	sellitem Red_Herb, 1;
	sellitem Yellow_Herb, 1;
	sellitem White_Herb, 1;
	sellitem Blue_Herb, 1;
	sellitem Green_Herb, 1;
	end;
OnCountFunds:
	setcurrency Flower;
	end;
OnPayFunds:
    if ( countitem(Flower) < @price )
        end;
    delitem Flower, @price;
    purchaseok();
    end;
}
-	trader	shop#fruits	FAKE_NPC,{
OnInit:
	tradertype NST_CUSTOM;
	sellitem Apple, 1;
	sellitem Banana, 1;
	sellitem Grape, 1;
	sellitem Carrot, 1;
	end;
OnCountFunds:
	setcurrency Flower;
	end;
OnPayFunds:
    if ( countitem(Flower) < @price )
        end;
    delitem Flower, @price;
    purchaseok();
    end;
}
```

Hercules seems to be longer on this one,  
however you can read them, as all stuffs in constants

rAthena seems to be doing better on this one, because it doesn't introduce new script command  
even the existing `*npcshopadditem` script commands works on `itemshop`

this makes Hercules seems having 2 sets of script commands do the same thing, but actually isn't  
`*callshop`, `*npcshopadditem` ... etc only works on `shop` type  
`*openshop`, `*sellitem` ... etc only works on `trader` type

another thing worth note, `*npcshopadditem` support `<npc name>`,  
means you don't have do use donpcevent,  
because `*sellitem` script command doesn't support adding items to another npc  
Example ... when I convert [Card Seller A-Z into itemshop mode](https://github.com/AnnieRuru/Release/blob/master/scripts/Quest%20%26%20Shops/card_seller/card_seller_r1.2.txt)  
I just change `shop` into `itemshop`, not much is even change there  
however when convert into Hercules format, [Hercules `*sellitem` doesn't seem to works so well](https://github.com/AnnieRuru/Release/blob/master/scripts/Quest%20%26%20Shops/card_seller/card_seller_1.0.txt)

#### External Links:
[Card Seller A-Z](http://herc.ws/board/topic/16586-card-seller-a-z/)
