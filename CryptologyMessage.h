// CryptologyMessage.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.

#pragma once

#include <mutex>
#include <vector>
#include <thread>
#include <unordered_map>

struct ClientIdenty
{
	std::string name;
	int id;

	ClientIdenty() : name(""), id(0) {}
	ClientIdenty(const std::string& name, int id) : name(name), id(id) {}

	bool operator==(const ClientIdenty& other) const { return name == other.name && id == other.id; }
};

struct Vector3 { float x, y, z; };
struct ClientData
{
	std::string messageText;

	ClientData() : messageText("") {}
};

// 해시 함수 정의
namespace std {
	template <>
	struct hash<ClientIdenty> {
		size_t operator()(const ClientIdenty& client) const {
			// 해시 함수의 간단한 구현
			size_t hashName = hash<std::string>{}(client.name);
			size_t hashId = hash<int>{}(client.id);
			return hashName ^ (hashId << 1); // XOR와 bit shift를 사용하여 해시를 결합
		}
	};
}

class MessageSystem
{
public:
	static void BroadcastMessage(const ClientIdenty& sender, const std::string& message);  // 메시지 브로드캐스트 함수
	static void HandleClient(int clientSocket);									// 서버 데이터 전송, 수신 메소드
	static void FindErrorClinet(ClientIdenty newClient);						// 해당 이름의 클라이언트가 존재하는지 확인하는 메소드

	static int StartServer(int port);		// 서버 시작 메소드
	static void StopServer();				// 서버 종료 메소드
	static void ServerLoop(int port);		// 서버 Loop

	static void Daemonize();				// 프로그램 데몬화 메소드
	static void SignalHandler(int signal);	// 프로그램 시그널 처리 메소드

	static bool serverRunning;

private:
	static std::vector<int> clients;
	static std::unordered_map<ClientIdenty, ClientData> clientData;

	static std::mutex clientsMutex;
	static std::thread serverThread;

	static int serverSocket;
};