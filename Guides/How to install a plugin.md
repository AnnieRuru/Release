## How to install a plugin

### Step 1
Download the `YourProject.c` file in the **\src\plugins\\** folder
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step1.png)

### Step 2
If using **MSVC 2017**, navigate to **\vcproj-15\**
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step2.png)

### Step 3
Select `plugin-sample.vcxproj`, copy paste it
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step3.png)

### Step 4
Rename `plugin-sample - Copy.vcxproj` into `YourProject.vcxproj`  
While renaming, Highlight the word **YourProject** and Copy it by **Ctrl+C**
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step4.png)

### Step 5
Right-Click on `YourFile.vcxproj`, Select **Edit with Notepad++**, or **Microsoft Notepad**
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step5.png)

### Step 6
Find this line
`<ProjectName>plugin-sample</ProjectName>`
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step6.png)
Rename `plugin-sample` to `YourProject` by **Ctrl+V**
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step6_.png)

### Step 7
Find this line
`<TargetName Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">sample</TargetName>`
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step7.png)
Rename `sample` to `YourProject` by **Ctrl+V**  
`<TargetName Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">YourProject</TargetName>`
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step7_.png)

### Step 8
Find this line
`<ClCompile Include="..\src\plugins\sample.c" /> `
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step8.png)
Rename `sample` to `YourProject` by Ctrl+V
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step8_.png)

### Step 9
Press **Ctrl+S** to save the file.

### Step 10
Open `Hercules-15.sln`,
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step10.png)

### Step 11
Right-click `Hercules-15`, Select **Add** -> **Existing Project**...
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step11.png)

### Step 12
Select `YourProject.vcxproj` in the **\vcproj-15\\** folder
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step12.png)

### Step 13
Right-click `Hercules-15`, Select **Build-Solution**
![](https://raw.githubusercontent.com/AnnieRuru/customs/master/screenshots/MSVC_step13.png)

### External Link:
http://herc.ws/wiki/Building_HPM_Plugin_for_MSVC  
https://github.com/HerculesWS/Hercules/wiki/Building-HPM-Plugin-for-MSVC
