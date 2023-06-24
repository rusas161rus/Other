@echo off

REM Укажите URL файла для скачивания
set "url=https://disk.yandex.ru/d/xxxx"

REM Укажите путь к директории "Program Files (x86)"
set "installPath=C:\Program Files (x86)\Oktell\Client"

REM Укажите имя файла для ярлыка
set "shortcutFileName=Oktell.lnk"

REM Укажите путь к рабочему столу пользователя
set "desktopPath=%userprofile%\Desktop"

REM Укажите путь для добавления папки в список исключений Windows Defender
powershell -Command "Add-MpPreference -ExclusionPath 'C:\Program Files (x86)\Oktell\Client\Oktell Launcher V2.exe'"

REM Проверка наличия папки "installPath"
if not exist "%installPath%" (
    echo there is no such folder
    pause
    exit
)

REM Скачивание файла напрямую в "Program Files (x86)"
powershell -Command "(New-Object System.Net.WebClient).DownloadFile('%url%', '%installPath%\Oktell Launcher V2.exe')"

REM Проверка успешного скачивания файла
if exist "%installPath%\Oktell Launcher V2.exe" (
    REM Создание ярлыка на рабочем столе
    powershell -Command "$WshShell = New-Object -ComObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%desktopPath%\%shortcutFileName%'); $Shortcut.TargetPath = '%installPath%\Oktell Launcher V2.exe'; $Shortcut.Save(); $Shortcut.WorkingDirectory = '%installPath%'; $Shortcut.Arguments = '-RunAs'; $Shortcut.Save()"

    echo Installation and shortcut creation completed.

    REM установка прав администратора для ярлыка
    powershell -Command "$WshShell = New-Object -comObject WScript.Shell; $Shortcut = $WshShell.CreateShortcut('%desktopPath%\Oktell.lnk'); $Shortcut.TargetPath = 'C:\Program Files (x86)\Oktell\Client\Oktell Launcher V2.exe'; $Shortcut.Save(); $bytes = [System.IO.File]::ReadAllBytes('%desktopPath%\Oktell.lnk'); $bytes[0x15] = $bytes[0x15] -bor 0x20; [System.IO.File]::WriteAllBytes('%desktopPath%\Oktell.lnk', $bytes)"

    echo Oktell shortcut now has admin rights.
) else (
    echo File download error.
)

pause
