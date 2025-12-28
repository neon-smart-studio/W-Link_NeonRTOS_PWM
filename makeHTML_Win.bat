cd html
@echo off
setlocal enabledelayedexpansion
>..\filelist.txt (
    for /f "delims=" %%i in ('powershell -Command "Get-ChildItem -Recurse -File -Name"') do (
        set "filePath=%%i"
        set "filePath=!filePath:\=/!"
        echo !filePath!
    )
)
..\mkespfsimage -c 0 < ..\filelist.txt > ..\webpages.neonrtfs
del ..\filelist.txt && cd ..
