call app_info_setup.bat
call blackberry-deploy -installApp -launchApp %BBX_DEVICE_IP% -password %DEVICE_PASSWORD% Device-Release/%APP_NAME%.bar
pause