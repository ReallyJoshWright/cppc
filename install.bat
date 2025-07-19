@echo off
setlocal

rem Define installation directory and other variables
set "INSTALL_DIR=%USERPROFILE%\_config_cppc"
set "REG_KEY=HKCU\Environment"
set "REG_VALUE_NAME=PATH"

echo Creating install directory: %INSTALL_DIR%
rem Create the directory; '/p' equivalent is implicit in mkdir for full paths
mkdir "%INSTALL_DIR%" >nul 2>&1
if not exist "%INSTALL_DIR%" (
    echo Error: Failed to create directory "%INSTALL_DIR%".
    pause
    goto :eof
)
echo Directory "%INSTALL_DIR%" created successfully or already exists.

echo Adding cppc binary and builder.h to %INSTALL_DIR%
rem Copy the compiled binary and header file
copy cppc "%INSTALL_DIR%" >nul
if %errorlevel% neq 0 (
    echo Error: Failed to copy cppc to "%INSTALL_DIR%". Ensure 'cppc' exists in the current directory.
    pause
    goto :eof
)
copy builder.h "%INSTALL_DIR%" >nul
if %errorlevel% neq 0 (
    echo Error: Failed to copy builder.h to "%INSTALL_DIR%". Ensure 'builder.h' exists in the current directory.
    pause
    goto :eof
)
echo Files copied successfully.

rem --- Check and add to PATH environment variable ---
set "PATH_ALREADY_EXISTS=0"

rem Get the current user's persistent PATH environment variable from the registry
rem 'reg query' is used to read the environment variable, and 'findstr' filters the output.
for /f "tokens=2*" %%a in ('reg query "%REG_KEY%" /v "%REG_VALUE_NAME%" 2^>nul ^| findstr /i "%REG_VALUE_NAME%"') do (
    set "CURRENT_USER_PATH=%%b"
)

rem Normalize the INSTALL_DIR for consistent comparison (remove trailing backslash if any)
set "NORMALIZED_INSTALL_DIR=%INSTALL_DIR%"
if "%NORMALIZED_INSTALL_DIR:~-1%"=="\" set "NORMALIZED_INSTALL_DIR=%NORMALIZED_INSTALL_DIR:~0,-1%"

if defined CURRENT_USER_PATH (
    rem Split the PATH into individual directories using ';' as a delimiter
    rem Then, check each directory to see if it matches the INSTALL_DIR (case-insensitive)
    for %%P in ("%CURRENT_USER_PATH:;=" "%") do (
        set "DIR_IN_PATH=%%~P"
        rem Normalize the directory from PATH for comparison
        if "%DIR_IN_PATH:~-1%"=="\" set "DIR_IN_PATH=%DIR_IN_PATH:~0,-1%"
        if /i "%DIR_IN_PATH%"=="%NORMALIZED_INSTALL_DIR%" (
            set "PATH_ALREADY_EXISTS=1"
            goto :path_check_done
        )
    )
)

:path_check_done

if %PATH_ALREADY_EXISTS% equ 0 (
    echo Adding %INSTALL_DIR% to PATH...
    rem Use setx to permanently add the new directory to the user's PATH.
    rem If CURRENT_USER_PATH is not defined (i.e., user PATH is empty), set it directly.
    if defined CURRENT_USER_PATH (
        setx PATH "%CURRENT_USER_PATH%;%INSTALL_DIR%"
    ) else (
        setx PATH "%INSTALL_DIR%"
    )

    if %errorlevel% equ 0 (
        echo Added %INSTALL_DIR% to PATH.
        echo IMPORTANT: Please restart your terminal (e.g., Command Prompt, PowerShell)
        echo or log out and log in again for the PATH changes to take effect.
        echo For the current session only, you can manually run: set PATH=%%PATH%%;%INSTALL_DIR%
    ) else (
        echo Error: Failed to add %INSTALL_DIR% to PATH.
        echo This might be due to the PATH variable exceeding the maximum length (typically 1024 characters).
    )
) else (
    echo %INSTALL_DIR% already exists in PATH. No changes made to PATH.
)

echo.
echo Installation Complete!
pause
endlocal
