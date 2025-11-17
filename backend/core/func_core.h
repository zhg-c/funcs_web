#pragma once
#include "common.h"
#include <string>
#include <vector>

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

std::vector<DNSRecord> execute_dns_record_core(const char *target);