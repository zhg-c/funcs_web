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

class ports_scan {
public:
	ports_scan() { }
	~ports_scan() { }

private:
	std::string tcp_connect_scan(const std::string &ip_address, int port);
	std::string udp_scan(const std::string &target, int port);
	std::vector<int> parse_ports(const std::string &ports_str);

public:
	std::vector<PortScanResult> execute_scan(
		const std::string &target,
		const std::string &ports_str,
		const std::string &scan_type);
};

class whois {
public:
	whois() { }
	~whois() { }

private:
	bool beginsWith(const std::string &s, const std::string &prefix);
	std::string trim(const std::string &s);
	WhoisInfo parseWhois(const std::string &rawText);
	bool hostname2ip(const char *hostname, std::string &ip);
	bool whois_query(const char *server, const char *target, std::string &data);

public:
	WhoisInfo execute_whois(const char *target);
};

std::vector<PortScanResult> execute_scan_core(
	const std::string &target,
	const std::string &ports_str,
	const std::string &scan_type);
WhoisInfo execute_whois_core(const char *target);