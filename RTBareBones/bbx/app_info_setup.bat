call c:\pro\bbndk\bbndk-env.bat
:make clean
:make
set APP_NAME=RTBareBones
set BUILD_DIR=arm/o.le-v7-g
set BBX_SIM_IP=192.168.64.128
set BBX_DEVICE_IP=169.254.0.1

:Needed for final submission:
set STORE_PASSWORD=
set CSJPIN=

:The password that the device keeps hassling you everytime you turn you turn it on:
set DEVICE_PASSWORD=