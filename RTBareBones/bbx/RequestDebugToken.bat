call app_info_setup.bat
:call blackberry-debugtokenrequest.bat -register -cskpass %STORE_PASSWORD% -csjpin %CSJPIN% client****.csj
call blackberry-debugtokenrequest.bat -cskpass %CSJPIN% -storepass %STORE_PASSWORD% -deviceId 0x50217E03 bbx_debugtoken.bar
call blackberry-deploy.bat -installDebugToken bbx_debugtoken.bar -device %BBX_DEVICE_IP% -password %DEVICE_PASSWORD%
pause