@echo off
call build || exit /b 1
pushd build
ninja test
popd
