Cls
call app_info_setup.bat

REM arm-none-linux-gnueabi-g++ -o tiltodemo ..\common\accelerometer.cpp ..\common\collision.cpp ..\common\gamelogic.cpp ..\common\geometry.cpp ..\common\graphics_ogl.cpp ..\common\graphics_sdl.cpp ..\common\main.cpp ..\common\sdl_init.cpp ..\common\sound.cpp "-I%PalmPDK%\include" "-I%PalmPDK%\include\SDL" "-L%PalmPDK%\device\lib" -Wl,--allow-shlib-undefined -lSDL -lSDL_net -lSDL_image -lSDL_mixer -lpdl -lGLES_CM

REM @echo off
@rem Set the device you want to build for to 1
@rem Use Pixi to allow running on either device
set PRE=0
set PIXI=1
REM set DEBUG=0 //set in app_info_setup.bat, set to 1 for a debug build that isn't stripped during packging

set SHARED=..\..\shared
set APP=..\source
set CLANMATH=..\..\shared\ClanLib-2.0\Sources\Core\Math
set ZLIBPATH=..\..\shared\util\zlib
set PPATH=..\..\shared\Renderer\linearparticle\sources\
set COMPPATH=..\..\shared\Entity

REM I really can't figure out how to make it accept a source file search path.. grr.  So I'll prepend for each.  If you know how tell me! -Seth
@rem List your source files here


REM **************************************** ENGINE SOURCE CODE FILES
set SRC= %SHARED%\PlatformSetup.cpp  %SHARED%\WebOS\SDLMain.cpp %SHARED%\win\app\main.cpp %SHARED%\webOS\WebOSUtils.cpp ^
%SHARED%\Audio\AudioManager.cpp %CLANMATH%\angle.cpp %CLANMATH%\mat3.cpp %CLANMATH%\mat4.cpp %CLANMATH%\rect.cpp %CLANMATH%\vec2.cpp %CLANMATH%\vec3.cpp ^
%CLANMATH%\vec4.cpp %SHARED%\Entity\Entity.cpp %SHARED%\Entity\Component.cpp %SHARED%\GUI\RTFont.cpp %SHARED%\Manager\Console.cpp %SHARED%\FileSystem\FileManager.cpp ^
%SHARED%\Manager\GameTimer.cpp %SHARED%\Manager\MessageManager.cpp %SHARED%\Manager\ResourceManager.cpp %SHARED%\Manager\VariantDB.cpp %SHARED%\Math\rtPlane.cpp ^
%SHARED%\Math\rtRect.cpp %SHARED%\Renderer\RenderBatcher.cpp %SHARED%\Renderer\SoftSurface.cpp %SHARED%\Renderer\Surface.cpp %SHARED%\Renderer\SurfaceAnim.cpp ^
%SHARED%\util\CRandom.cpp %SHARED%\util\GLESUtils.cpp %SHARED%\util\MathUtils.cpp %SHARED%\util\MiscUtils.cpp %SHARED%\util\RenderUtils.cpp %SHARED%\util\ResourceUtils.cpp ^
%SHARED%\util\Variant.cpp %SHARED%\util\boost\libs\signals\src\connection.cpp %SHARED%\util\boost\libs\signals\src\named_slot_map.cpp %SHARED%\util\boost\libs\signals\src\signal_base.cpp ^
%SHARED%\util\boost\libs\signals\src\slot.cpp %SHARED%\util\boost\libs\signals\src\trackable.cpp %SHARED%\FileSystem\StreamingInstance.cpp %SHARED%\FileSystem\StreamingInstanceZip.cpp ^
%SHARED%\FileSystem\StreamingInstanceFile.cpp %SHARED%\BaseApp.cpp %SHARED%\util\unzip\unzip.c %SHARED%\util\unzip\ioapi.c

REM **************************************** ENGINE COMPONENT SOURCE CODE FILES
set COMPONENT_SRC=%COMPPATH%\CustomInputComponent.cpp %COMPPATH%\FocusInputComponent.cpp %COMPPATH%\FocusUpdateComponent.cpp %COMPPATH%\ArcadeInputComponent.cpp


REM **************************************** ZLIB SOURCE CODE FILES
set ZLIB_SRC=%ZLIBPATH%/deflate.c %ZLIBPATH%/gzio.c %ZLIBPATH%/infback.c %ZLIBPATH%/inffast.c %ZLIBPATH%/inflate.c %ZLIBPATH%/inftrees.c %ZLIBPATH%/trees.c %ZLIBPATH%/uncompr.c %ZLIBPATH%/zutil.c %ZLIBPATH%/adler32.c %ZLIBPATH%/compress.c %ZLIBPATH%/crc32.c

REM **************************************** APP SOURCE CODE FILES
set APP_SRC=%APP%\App.cpp
REM **************************************** END SOURCE

set DEFINES=-D RT_WEBOS -D BOOST_ALL_NO_LIB  -D RT_WEBOS_ARM -D NDEBUG

set FLAGS=-O2

@rem List the libraries needed
set LIBS=-lSDL -lSDL_net -lSDL_image -lSDL_mixer -lpdl -lGLES_CM

@rem Name your output executable
set OUTFILE=%APPNAME%

set INCLUDE_DIRS=-I..\..\shared\util\boost -I%SHARED% -I%APP% "-I%PALMPDK%\include" "-I%PALMPDK%\include\SDL" -I..\..\shared\ClanLib-2.0\Sources -I%ZLIBPATH%

set LIB_DIRS="-L%PALMPDK%\device\lib"



if %PRE% equ 0 if %PIXI% equ 0 goto :END


if %DEBUG% equ 1 (
   set DEVICEOPTS=-g
   set FLAGS=-O
   set DEFINES=%DEFINES% -D _DEBUG
) else (
   set DEVICEOPTS=
   set DEFINES=%DEFINES% -D NDEBUG

)
if %PRE% equ 1 (
   set DEVICEOPTS=%DEVICEOPTS% -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
)

if %PIXI% equ 1 (
   set DEVICEOPTS=%DEVICEOPTS% -mcpu=arm1136jf-s -mfpu=vfp -mfloat-abi=softfp
)

echo %DEVICEOPTS%

arm-none-linux-gnueabi-gcc  %DEFINES% %DEVICEOPTS% %INCLUDE_DIRS% %LIB_DIRS% -o %OUTFILE% %SRC% %APP_SRC% %COMPONENT_SRC% %ZLIB_SRC% -Wl,--allow-shlib-undefined %LIBS% -Wno-deprecated %FLAGS%
if not exist %APPNAME% %RT_UTILS%\beeper.exe /p

goto :EOF

:END
echo Please select the target device by editing the PRE/PIXI variable in this file.

REM exit /b 1





