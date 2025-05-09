@echo off

set batch_script_path=%~dp0
set premake_exe_path=%batch_script_path%tools\premake
REM set dev_path=%batch_script_path%dev

REM set premake_script_path=%dev_path%
REM set premake5_file_path=%dev_path%\premake5.lua

set os_premake_flag=--os=windows

REM set scripts_premake_flag=--scripts=%premake_script_path%
REM set premake5_file_flag=--file=%premake5_file_path%

set ide_action=vs2022

REM echo %batch_script_path%
REM echo %premake_exe_path%
REM echo %premake_script_path%

REM echo %scripts_premake_flag%

REM echo %premake_exe_path%\premake5.exe %scripts_premake_flag% %os_premake_flag% %ide_action%
REM call %premake_exe_path%\premake5.exe %scripts_premake_flag% %os_premake_flag% %ide_action%

REM call %premake_exe_path%\premake5.exe %premake5_file_flag% %os_premake_flag% %ide_action%

call %premake_exe_path%\premake5.exe %os_premake_flag% %ide_action%

REM source: https://stackoverflow.com/questions/986447/batch-file-to-copy-files-from-one-folder-to-another-folder
REM xcopy /y %batch_script_path%script.csl %batch_script_path%build\crayon
REM echo f | xcopy /f /y %batch_script_path%script.csl %batch_script_path%build\crayon
REM xcopy /d /y %batch_script_path%scripts %batch_script_path%build\crayon\scripts
REM echo d | xcopy srcdirectory destdirectory /S

REM xcopy docs page: https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/xcopy
REM -s copy directories and subdirectories when they are not empty
REM -e copy directories and subdirectoreis even if they are empty
echo d | xcopy /f /y %batch_script_path%scripts %batch_script_path%build\crayon\scripts /s