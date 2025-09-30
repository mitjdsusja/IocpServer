# 실시간 게임 서버

시연 영상 : https://youtu.be/UtUiSn3o3l4 <br>
주요 코드 설명 : https://www.notion.so/MMO-26a1e89d067180c493f1c9343960055f?source=copy_link <br>

### 주요 내용
- IOCP 기반 **비동기 네트워크 처리** 및 멀티스레드 서버 구조 설계
- **그리드 기반 위치 동기화**로 실시간 위치 동기화 구현
- Unity 클라이언트와 연동하여 **실제 플레이 환경 테스트**

## 파일
/IocpClient : 더미 클라이언트 코드 <br>
/IocpServer : 서버 컨텐츠 코드<br>
/ServerLib : 서버 코어 코드 (네트워크)<br>

### 사용 기술
- **언어**: C++
- **데이터 베이스**: Mysql
- **네트워크 라이브러리**: TCP, IOCP
- **데이터 직렬화**: Protobuf
- **운영 체제**: Windows

---

![서버 주요 기능](./images/Slide4.jpg)
![서버 아키텍쳐](./images/Slide5.jpg)
![패킷 처리 흐름](./images/패킷처리흐름.png)
![패킷 처리 흐름 설명](./images/Slide7.jpg)
![Actor모델 1](./images/Actor모델1.jpg)
![JobQueue 1](./images/JobQueue1.jpg)
![JobTimer1](./images/Slide12.jpg)
![JobTimer2](./images/Slide13.jpg)
![TLS BufferPool 1](./images/TLS1.jpg)
![Grid 기반 위치 동기화1](./images/Slide15.jpg)
![Grid 기반 위치 동기화2](./images/Slide16.jpg)
![패킷 분할 전송](./images/패킷분할전송1.jpg)

