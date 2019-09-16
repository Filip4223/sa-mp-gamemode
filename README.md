# GameMode RolePlay
![Version](https://img.shields.io/badge/version-2.0.1-yellow.svg?logo=appveyor&longCache=true&style=flat-square)    ![Build](https://img.shields.io/badge/build-success-green.svg?logo=appveyor&longCache=true&style=flat-square)

### Editor
We preffer using Sublime Text 3. You can find setup tutorial on [sa-mp forums](https://forum.sa-mp.com/showthread.php?t=626423).

### Sublime build system
> To use this build system you must add root repository folder to your Sublime project.

```json
{
	"shell_cmd": "echo Building Honest RolePlay... && pawncc.exe $folder\\gamemodes\\hrp.pwn -o$folder\\gamemodes\\hrp.amx -;+ -(+ -d3 && echo. && echo Honest Roleplay compilation ended",
	"file_regex": "(.*?)[(]([0-9]*)[)]",
	"selector": "hrp.pwn",
	"working_dir": "$folder\\pawno"
}
```
a