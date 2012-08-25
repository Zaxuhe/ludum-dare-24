call app_info_setup.bat
set IPK_NAME=%BUNDLEIDPREFIX%%APPNAME%_%VERSION%_all.ipk
::I stopped killing the process because it seemed to sometimes crash the phone
::plink -P 10022 root@localhost -pw "" "killall -9 gdbserver %APPNAME%"
del %APPNAME%
del %IPK_NAME%
call build.bat
if not exist %APPNAME% %RT_UTILS%\beeper.exe /p
call packageit.bat
call palm-install.bat -r %BUNDLEIDPREFIX%%APPNAME%
if not exist %IPK_NAME% %RT_UTILS%\beeper.exe /p
call palm-install.bat %IPK_NAME%
call RunOnDevice.bat

