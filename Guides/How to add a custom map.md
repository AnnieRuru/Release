## How to add a custom map
conf\grf-files.txt
```
//-----------------------------------------
// GRF Files
// Add as many as needed.
//-----------------------------------------
grf: D:\Ragnarok\client\rdata.grf
grf: D:\Ragnarok\client\data.grf

//-----------------------------------------
// Data Directory
// Use the base folder, not the data\ path.
//-----------------------------------------
data_dir: D:\Ragnarok\client\
```
conf\map\maps.conf
```
	"arathi",
```
db\map_index.txt
```
arathi    1250
```

For rAthena, just double-click `mapcache.bat`. **Done**

#### Hercules has these extra steps

1. add `mapcache.c` plugin into **conf/plugins.conf**
```
plugins_list: [
 /* Enable HPMHooking when plugins in use rely on Hooking */
 //"HPMHooking",
 //"db2sql",
 //"sample",
 //"other",
 "mapcache",
]
```

2. add `mapcache` as a project in visual studio. See [how to install a plugin](https://github.com/AnnieRuru/Release/blob/master/Guides/How%20to%20install%20a%20plugin.md).


3. [Windows Key] + [R] -> type **cmd**  
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/herc_mapcache_1_1.png)

4. point to your Hercules emulator folder, and type `map-server.exe --help`  
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/herc_mapcache_1_2.png)

5. type `map-server.exe --map arathi`  
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/herc_mapcache_1_3.png)

#### Once get the hang of it, there is a faster method with Windows

3. point to your `map-server.exe`, and **create shortcut**
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/herc_mapcache_2_1.png)

4. right-click, Properties, and edit the Target:  
`D:\Ragnarok\Hercules\map-server.exe --map arathi`  
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/herc_mapcache_2_2.png)

### External Links:
[Adding a custom mapcache file](http://herc.ws/board/topic/15817-adding-a-custom-map-cache-file/#comment-87194)  
[[Guide] Mapcache Generation 2018](http://herc.ws/board/topic/15868-guide-mapcache-generation-2018/)

