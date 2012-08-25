call app_info_setup.bat
:call blackberry-nativepackager.bat


call blackberry-nativepackager.bat -configuration com.qnx.qcc.configuration.exe.release.1153117490 -devMode -debugToken usbdebugtoken.bar -package Device-Release/%APP_NAME%.bar bar-descriptor.xml

call blackberry-deploy.bat -uninstallApp -password %DEVICE_PASSWORD% %BBX_DEVICE_IP% Device-Release/%APP_NAME%.bar
call blackberry-deploy.bat -installApp -password %DEVICE_PASSWORD% %BBX_DEVICE_IP% Device-Release/%APP_NAME%.bar

pause