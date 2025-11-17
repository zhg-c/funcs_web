#pragma once
#include <string>
#include <vector>
struct PortScanResult {
	int port;
	std::string status;
	std::string service; // 服务的识别逻辑将在 Python 层处理，这里暂时返回空或模拟值
};

struct WhoisInfo {
	std::string domain;
	std::string registryDomainID;
	std::string registrar;
	std::string registrarWhoisServer;
	std::string registrarURL;
	std::string creationDate;
	std::string updatedDate;
	std::string expiryDate;
	std::vector<std::string> statuses;
	std::vector<std::string> nameServers;
	std::string dnssec;
};

struct DNSRecord {
	std::string type;
	std::string value;
	int ttl;
};
