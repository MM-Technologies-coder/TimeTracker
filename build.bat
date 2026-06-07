@echo off
echo ============================================
echo   GESTIONE PRESENZE - Build EXE
echo ============================================
echo.

echo [1/3] Installazione dipendenze...
pip install flask pywebview pyinstaller
echo.

echo [2/3] Build SERVER (app.exe)...
pyinstaller --onefile --noconsole --add-data "index.html;." --name "GestionePresenze_Server" app.py
echo.

echo [3/3] Build CLIENT (client.exe)...
pyinstaller --onefile --noconsole --name "GestionePresenze" client.pyw
echo.

echo ============================================
echo   BUILD COMPLETATO!
echo.
echo   File generati nella cartella "dist\":
echo     - GestionePresenze_Server.exe  (per il tuo server)
echo     - GestionePresenze.exe         (per i PC dipendenti)
echo ============================================
pause
