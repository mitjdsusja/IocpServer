# TCP 기반 멀티플레이어 게임 서버

### 주요 기능
1. **클라이언트 연결 관리**: 클라이언트의 접속 및 연결 해제 처리.
2. **데이터 처리**: 클라이언트로부터 수신한 데이터를 처리
3. **JobTimer 작업 예약**: 특정 작업을 예약하고 정해진 시간에 실행.

### 사용 기술
- **언어**: C++
- **네트워크 라이브러리**: TCP, IOCP
- **데이터 직렬화**: Protobuf (https://github.com/protocolbuffers/protobuf#protobuf-compiler-installation)
- **utf변환: boost-locale
- **운영 체제**: Windows

---
