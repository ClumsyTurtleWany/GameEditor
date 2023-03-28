pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./enum.proto
protoc.exe -I=./ --cpp_out=./ ./struct.proto
protoc.exe -I=./ --cpp_out=./ ./protocol.proto

GenPacket.exe --path=./Protocol.proto --name=client --output=ClientPacketHandler --recv=S_ --send=C_
GenPacket.exe --path=./Protocol.proto --name=server --output=ServerPacketHandler --recv=C_ --send=S_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y enum.pb.h "../../../SampleProject2"
XCOPY /Y struct.pb.h "../../../SampleProject2"
XCOPY /Y protocol.pb.h "../../../SampleProject2"
XCOPY /Y enum.pb.cc "../../../SampleProject2"
XCOPY /Y struct.pb.cc "../../../SampleProject2"
XCOPY /Y protocol.pb.cc "../../../SampleProject2"
XCOPY /Y ClientPacketHandler.h "../../../SampleProject2"
XCOPY /Y ServerPacketHandler.h "../../../SampleProject2"

PAUSE