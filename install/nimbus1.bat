cls
@echo off
echo ******************************************************
echo * nimbus1.bat <com port>
echo * example nimbus1.bat com2
echo * ensure USB drivers are installed first
echo ******************************************************
echo * Agnicure Nimbus Products
echo * Panchanga EKG Arduino 32 Kit
echo * Nimbus 1
echo * See Arduino forum at http://forum.arduino.cc/index.php?topic=132688.0
echo * More information at https://sites.google.com/site/arduinodueatmelstudio/home/uploading-code-to-due

@echo off

mode %1:1200,n,8,1,p
Product\Nimbus_Due\bossac.exe --port=%1 -U false -e -w -v -b Product\Nimbus_Due\due_olimex_agniheart.cpp.bin -R

if errorlevel 255 goto finish
if errorlevel 1 goto error
goto finish

:error
echo Error! Error!
echo Nimbus unable to upload
pause

:finish