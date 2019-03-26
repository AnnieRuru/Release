### Download: 1.1
sample quest log

### Download: 1.1
sample daily quest

---------------------------------

these are 5 sample quests using quest log system

this was actually made as a sample script inside a script support section in eathena forum  
original topic from eathena  
thanks to paradog eathena admin that makes eathena forum down, almost gonna remake everything from scratch  
luckily I got a few segment here and there in my old hard-disk so doesn't take too much time to write this sample  

#### Quest #1  
typical talk to npc A, then npc B ... etc  
demonstrate how to use `*checkquest`  

#### Quest #2
collect item quest  
just checkitem with it  
I still don't know how to use the client to trace is the item collecting has completed or not  

#### Quest #3
kill monster quest  
demonstrate how to use `*checkquest( x, HUNTING )`  

#### Quest #4
typical talk to npc A, then npc B ... BUT with a time limit  
demonstrate how to use `*checkquest( x, PLAYTIME )`  

#### Quest#5
if player having this quest, player can `*getitem` by killing this monster  
see https://github.com/HerculesWS/Hercules/blob/stable/db/quest_db.conf#L45-L50

![screen2019Hercules031.jpg](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/screen2019Hercules031.jpg)

![screen2019Hercules044.jpg](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/screen2019Hercules044.jpg)


and I want to remind this is just a sample script  
usually make for source modification on quest log system (now is 3rd time I think ?)

<details>
<summary>Revision History</summary>

0.1 - script

0.2 - script

0.3 - script
- update to new format, and new script commands

1.0 - script
- update to new format

1.1 - script
- add achievement system

</details>

--------------------------

2 sample script for daily system

### Quest 1. Daily item giver ...
although I think this should done with [attendance system](https://rathena.org/board/topic/117396-you-are-not-allowed-to-use-the-attendance-system/#comment-353681) ...

### Quest 2. Daily hunting ...
self explain ...

![screen2019Hercules032.jpg](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/screen2019Hercules032.jpg)

![screen2019Hercules045.jpg](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/screen2019Hercules045.jpg)

<details>
<summary>Revision History</summary>

1.0 - script

1.1 - script
- add achievement system

</details>

---
```lua
	[109991] = {
		UI_Type = 0,
		group = "CHATTING",
		major = 4,
		minor = 0,
		title = "Tavern Helper",
		content = {
			summary = "Visit the Tavern Master",
			details = "Visit the Tavern Master in Prontera."
		},
		resource = {
			[1] = { text = "Acolyte Trouble" },
			[2] = { text = "Gemstone shortage" },
			[3] = { text = "Poring Problem" },
			[4] = { text = "Hand over the Artifact" },
			[5] = { text = "Shining Rocker" }
		},
		reward = { title = 991 },
		score = 5
	},
```
```lua
	[109992] = {
		UI_Type = 1,
		group = "GOAL_ACHIEVE",
		major = 6,
		minor = 0,
		title = "Complete Daily Quest for 30 times",
		content = {
			summary = "Complete Daily Quest",
			details = "Complete Daily Quest for 30 times and you can earn some nice rewards!"
		},
		resource = {
			[1] = { text = "Collect Daily Items 30 times", count = 30 },
			[2] = { text = "Complete Daily Quest 30 times", count = 30 }
		},
		reward = { title = 992, item = 513 },
		score = 30
	},
```
UI_TYPE
- display a bar like in the daily quest screenshot
- 0: in the [resource], doesn't support [count]
- 1: support [count]

group:
- read from data\texture\À¯ÀúÀÎÅÍÆäÀÌ½º\achievement
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/goal_achieve.png)

major
- 1: General tab
- 2: Adventure tab
- 3: Battle tab
- 4: Quest tab
- 5: Memorial tab
- 6: Feat tab 
