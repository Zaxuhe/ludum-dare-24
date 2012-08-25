call app_info_setup.bat

:call blackberry-nativepackager.bat -help-advanced

call blackberry-nativepackager.bat -DQNX_TARGET=%QNX_TARGET% -configuration com.qnx.qcc.configuration.exe.release.1153117490 -package Device-Release/%APP_NAME%.bar bar-descriptor.xml

pause