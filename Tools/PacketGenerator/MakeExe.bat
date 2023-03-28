pushd %~dp0
pyinstaller	--onefile PacketGenerator.py
MOVE .\dist\PacketGenerator.exe .\GenPacket.exe
@RD /S /Q .\build
@RD /S /Q .\dist
DEL /S /F /Q .\PacketGenerator.spec

XCOPY /Y /E Templates "../../Common/protoc-3.17.1-win64/bin/Templates"
XCOPY /Y GenPacket.exe "../../Common/protoc-3.17.1-win64/bin"

PAUSE