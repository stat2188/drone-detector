@echo off
REM =========================================
REM Enhanced Drone Analyzer (EDA) Code Collector - Windows Batch
REM =========================================

REM Simply call the PowerShell script
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0collect_eda_code.ps1"

pause
