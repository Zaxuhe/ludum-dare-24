call app_info_setup.bat
set IPK_NAME=%BUNDLEIDPREFIX%%APPNAME%_%VERSION%_all.ipk
REM del %APPNAME%
del %IPK_NAME%
if not exist %APPNAME% %RT_UTILS%\beeper.exe /p
call packageit.bat

if not exist %IPK_NAME% %RT_UTILS%\ beeper.exe /p
call palm-install.bat -r c%BUNDLEIDPREFIX%%APPNAME%
call palm-install.bat %IPK_NAME%
call RunOnDevice.bat
