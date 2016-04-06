@echo off
doxygen.exe IDD.doxyconf
cat warnings.log
pause
