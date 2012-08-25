call app_info_setup.bat


:call blackberry-signer.bat
:call blackberry-signer -verbose -register -storepass %STORE_PASSWORD% -csjpin %CSJPIN% ..\..\AppleStuff\bbx\client-RDK-771693144.csj ..\..\AppleStuff\bbx\client-PBDT-771693144.csj

call blackberry-signer -storepass %STORE_PASSWORD% Device-Release/%APP_NAME%.bar
call blackberry-signer -verify Device-Release/%APP_NAME%.bar
pause