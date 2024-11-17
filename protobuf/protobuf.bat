

protoc.exe -I=./ --cpp_out=./ ./messageTest.proto

XCOPY /Y messageTest.pb.cc "../ServerLib"
XCOPY /Y messageTest.pb.h "../ServerLib"

pause