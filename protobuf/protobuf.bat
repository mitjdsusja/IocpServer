

protoc.exe -I=./ --cpp_out=./ ./messageTest.proto

XCOPY /Y messageTest.pb.cc "../ServerLib"
XCOPY /Y messageTest.pb.h "../ServerLib"

XCOPY /Y messageTest.pb.cc "../IocpServer"
XCOPY /Y messageTest.pb.h "../IocpServer"

XCOPY /Y messageTest.pb.cc "../IocpClient"
XCOPY /Y messageTest.pb.h "../IocpClient"

pause