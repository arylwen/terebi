
rem cd ..

set SCRIPTS_DIR=.\scripts
set ETC_DIR=.\etc
set DRIVER_DIR=.\driver
set WORK_DIR=.\work

set CONFIG_FILE=%ETC_DIR%\ds.terebi.config
set MAIN=us.terebi.engine.Main

set CLASSPATH=%ETC_DIR%

@echo off

 setLocal EnableDelayedExpansion
 set CLASSPATH=%CLASSPATH%"
 for /R %DRIVER_DIR% %%a in (*.jar) do (
   set CLASSPATH=!CLASSPATH!;%%a
 )
 set CLASSPATH=!CLASSPATH!"
 echo !CLASSPATH!


@echo on
set JAVA_OPTS=%JAVA_OPTS% -Xmx512M

java -cp %CLASSPATH% %JAVA_OPTS% %MAIN% %CONFIG_FILE%

rem cd scripts