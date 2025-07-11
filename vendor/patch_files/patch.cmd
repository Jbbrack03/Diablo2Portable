* set the product patch name
PatchVersion This patch upgrades Diablo II Lord of Destruction from version 1.07 or later to version 1.14d.
* make sure that we don't patch version 1.14d or greater
FileVersionLessThan "$(InstallPath)\Game.exe"    1.14.3.71    0xffff.0xffff.0xffff.0xffff
SetLauncher "$(InstallPath)\Diablo II.exe"
SetUninstall $(WinDir)\DIIUnin.dat
