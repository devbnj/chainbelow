cls
echo off
echo ******************************************************
echo * nimbus2.bat [com port]
echo * example nimbus2.bat com4
echo * ensure USB drivers are installed first
echo ******************************************************
echo * Agnicure Nimbus Products
echo * Panchanga EKG Arduino 8 Kit
echo * Nimbus 2
echo off

Product\Nimbus_Mega\avrdude -CC:Product\Nimbus_Mega\avrdude.conf -v -patmega2560 -cwiring -P%1 -b115200 -D -Uflash:w:Product\Nimbus_Mega\mega_olimex_agniheart.cpp.hex:i 

if errorlevel 255 goto finish
if errorlevel 1 goto error
goto finish

:error
echo Error! Error!
echo Nimbus unable to upload
pause

:finish