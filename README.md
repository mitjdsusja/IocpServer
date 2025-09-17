# 실시간 게임 서버

시연 영상 : https://youtu.be/UtUiSn3o3l4 <br>
주요 코드 설명 : https://www.notion.so/MMO-26a1e89d067180c493f1c9343960055f?source=copy_link <br>

### 주요 내용
- IOCP 기반 **비동기 네트워크 처리** 및 멀티스레드 서버 구조 설계
- **시야 기반 위치 동기화**로 실시간 위치 동기화 구현
- Unity 클라이언트와 연동하여 **실제 플레이 환경 테스트**

### 사용 기술
- **언어**: C++
- **데이터 베이스**: Mysql
- **네트워크 라이브러리**: TCP, IOCP
- **데이터 직렬화**: Protobuf
- **utf변환**: boost-locale
- **운영 체제**: Windows

---

![서버 주요 기능](./images/Slide4.jpg)
![서버 아키텍쳐](./images/Slide5.jpg)
![패킷 처리 흐름](./images/패킷처리흐름.png)
![패킷 처리 흐름 설명](./images/Slide7.jpg)
![BufferPool1](./images/Slide8.jpg)
![BufferPool2](./images/Slide9.jpg)
[BufferPool 정의 (Header)](https://github.com/mitjdsusja/IocpServer/blob/main/ServerLib/BufferPool.h)<br>
[BufferPool 사용 예시 (PacketHandler.cpp: L50–L65)](https://github.com/mitjdsusja/IocpServer/blob/main/IocpServer/PacketHandler.cpp#L50-L65)
![JobQueue1](./images/Slide10.jpg)
[Job 사용 예시 (RoomManager.cpp: L136-145)](https://github.com/mitjdsusja/IocpServer/blob/main/IocpServer/RoomManager.cpp#L136-145)
![JobTimer1](./images/Slide12.jpg)
![JobTimer2](./images/Slide13.jpg)
![Grid 기반 위치 동기화1](./images/Slide15.jpg)
![Grid 기반 위치 동기화2](./images/Slide16.jpg)

