#include "func_core.h"

#include <iostream>
#include <sstream>
#include <map>
#include <chrono> // 用于计时
#include <thread> // 用于线程处理

// 包含 Posix Socket 相关的头文件
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring> // For memset and memcpy

// ⭐️ 新增/确保包含这些头文件用于 DNS 解析和原始套接字操作
#include <netdb.h> // For hostent and gethostbyname
#include <sys/time.h> // For timeval struct used in select
#include <netinet/ip.h> // For iphdr
#include <netinet/ip_icmp.h> // For icmphdr and ICMP types
#include <stdio.h> //scanf , printf
#include <string.h> //strtok
#include <stdlib.h> //realloc
#include <string>
#include <vector>
#include <algorithm>

// 定义超时时间 (毫秒)
constexpr int TIMEOUT_MS = 500;
// UDP 扫描的总体超时时间
constexpr int UDP_TIMEOUT_MS = 2000;
// ICMP 监听套接字接收超时时间 (较短，用于快速循环)
constexpr int ICMP_RECV_TIMEOUT_MS = 100;

/**
 * @brief 尝试与指定 IP 和端口建立 TCP 连接。
 * @param ip_address 目标IP地址
 * @param port 目标端口
 * @return 状态 ("Open", "Closed", "Filtered")
 */
std::string ports_scan::tcp_connect_scan(const std::string &ip_address, int port)
{
	int sock = -1;
	std::string result = "Closed";
	struct sockaddr_in target_addr;

	// 1. 创建套接字
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		return "Filtered";
	}

	// 2. 配置目标地址结构
	std::memset(&target_addr, 0, sizeof(target_addr)); // 初始化
	target_addr.sin_family = AF_INET;
	target_addr.sin_port = htons(port);

	// 将 IP 地址字符串转换为网络字节序
	if (inet_pton(AF_INET, ip_address.c_str(), &target_addr.sin_addr) <= 0) {
		// 如果不是有效的IP，尝试 DNS 解析
		struct hostent *host_info = gethostbyname(ip_address.c_str());
		if (host_info == nullptr) {
			close(sock);
			return "Filtered"; // 视为过滤或无效目标
		}
		std::memcpy(&target_addr.sin_addr, host_info->h_addr_list[0], host_info->h_length);
	}

	// 3. 设置非阻塞模式
	fcntl(sock, F_SETFL, O_NONBLOCK);

	// 4. 发起连接
	int conn_result = connect(sock, (struct sockaddr *)&target_addr, sizeof(target_addr));

	if (conn_result < 0 && errno != EINPROGRESS) {
		// 立即失败，通常是 ECONNREFUSED (Closed)
		result = (errno == ECONNREFUSED) ? "Closed" : "Filtered";
	} else if (conn_result == 0) {
		// 立即连接成功
		result = "Open";
	} else {
		// 5. 连接正在进行中 (EINPROGRESS)，使用 select 等待连接完成或超时
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = TIMEOUT_MS * 1000;

		fd_set write_fds;
		FD_ZERO(&write_fds);
		FD_SET(sock, &write_fds);

		// select 等待套接字变为可写状态（连接成功）或出错
		int select_result = select(sock + 1, NULL, &write_fds, NULL, &tv);

		if (select_result > 0) {
			// 套接字状态改变
			int so_error;
			socklen_t len = sizeof(so_error);
			// 检查套接字错误状态
			if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len) < 0) {
				result = "Filtered";
			} else if (so_error == 0) {
				result = "Open"; // 成功连接
			} else {
				// 连接被拒绝 (例如 Connection Refused)
				result = (so_error == ECONNREFUSED) ? "Closed" : "Filtered";
			}
		} else if (select_result == 0) {
			// 超时 (select 返回 0)
			result = "Filtered"; // 视为过滤或丢包
		} else {
			// select 错误
			result = "Filtered";
		}
	}

	// 6. 关闭套接字
	if (sock >= 0) {
		close(sock);
	}
	return result;
}

// =================================================================
// ⭐️ NEW: UDP SCAN LOGIC (Using Raw ICMP Sockets)
// =================================================================

/**
 * @brief 尝试使用原始套接字监听 ICMP 响应进行 UDP 端口扫描。
 * @param target 目标IP地址
 * @param port 目标端口
 * @return 状态 ("Open", "Closed", "Filtered", or "Error:...")
 */
std::string ports_scan::udp_scan(const std::string &target, int port)
{
	// 1. 创建 UDP 套接字 (用于发送数据)
	int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock < 0) {
		return "Error: Cannot create UDP socket";
	}

	// 2. 创建 ICMP 原始套接字 (用于监听 ICMP 响应)
	// 注意：需要 root 权限或 CAP_NET_RAW 能力
	int icmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (icmp_sock < 0) {
		close(udp_sock);
		if (errno == EPERM) {
			return "Error: Requires CAP_NET_RAW or root to use RAW sockets";
		}
		return "Error: Cannot create RAW ICMP socket";
	}

	// 设置 ICMP 套接字的接收超时时间
	struct timeval tv_icmp;
	tv_icmp.tv_sec = 0;
	tv_icmp.tv_usec = ICMP_RECV_TIMEOUT_MS * 1000;
	setsockopt(icmp_sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_icmp, sizeof(tv_icmp));

	// 3. 目标地址设置
	struct sockaddr_in target_addr;
	std::memset(&target_addr, 0, sizeof(target_addr));
	target_addr.sin_family = AF_INET;
	target_addr.sin_port = htons(port);

	// 解析目标 IP
	if (inet_pton(AF_INET, target.c_str(), &target_addr.sin_addr) <= 0) {
		struct hostent *host_info = gethostbyname(target.c_str());
		if (host_info == nullptr) {
			close(udp_sock);
			close(icmp_sock);
			return "Error: Host resolution failed";
		}
		std::memcpy(&target_addr.sin_addr, host_info->h_addr_list[0], host_info->h_length);
	}

	// 4. 发送 UDP 探测包
	const char *probe_data = "U";
	sendto(udp_sock, probe_data, 1, 0, (struct sockaddr *)&target_addr, sizeof(target_addr));

	// 5. 监听 ICMP 响应
	auto start_time = std::chrono::steady_clock::now();
	char recv_buf[1500]; // 足够接收完整的 IP/ICMP 包

	while (true) {
		auto current_time = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

		if (elapsed >= UDP_TIMEOUT_MS) {
			// 总体超时：没有收到 ICMP 错误。端口 Open 或 Filtered。默认 Nmap 行为通常视为 Open/Filtered。
			close(udp_sock);
			close(icmp_sock);
			return "Open";
		}

		struct sockaddr_in src_addr;
		socklen_t addr_len = sizeof(src_addr);

		// 尝试从 ICMP 原始套接字接收数据
		ssize_t bytes_received = recvfrom(icmp_sock, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&src_addr, &addr_len);

		if (bytes_received > 0) {
			// 收到数据包，解析 ICMP 头
			struct iphdr *ip_hdr = (struct iphdr *)recv_buf;
			// ICMP 消息从 IP 头后面开始
			struct icmphdr *icmp_hdr = (struct icmphdr *)(recv_buf + (ip_hdr->ihl * 4));

			// 检查 ICMP 类型
			if (icmp_hdr->type == ICMP_DEST_UNREACH && icmp_hdr->code == ICMP_PORT_UNREACH) {
				// 收到 ICMP Port Unreachable (Code 3)
				// **端口 Closed**
				close(udp_sock);
				close(icmp_sock);
				return "Closed";
			}

			// 收到其他 ICMP 响应，继续等待
			continue;
		} else if (bytes_received < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
			// 接收错误
			close(udp_sock);
			close(icmp_sock);
			return "Error: ICMP recv failed";
		}

		// 如果 recvfrom 超时，循环将继续并检查总体超时
	}

	// 理论上不会到达这里
	close(udp_sock);
	close(icmp_sock);
	return "Error: Unexpected exit";
}

// 辅助函数：解析端口范围字符串 ("1-100,22,8080")
std::vector<int> ports_scan::parse_ports(const std::string &ports_str)
{
	std::vector<int> ports;
	std::stringstream ss(ports_str);
	std::string segment;

	while (std::getline(ss, segment, ',')) {
		size_t dash_pos = segment.find('-');
		if (dash_pos == std::string::npos) {
			// 单个端口，例如 "80"
			try {
				int port = std::stoi(segment);
				if (port > 0 && port <= 65535)
					ports.push_back(port);
			} catch (...) {
			}
		} else {
			// 范围端口，例如 "1-100"
			try {
				int start = std::stoi(segment.substr(0, dash_pos));
				int end = std::stoi(segment.substr(dash_pos + 1));
				if (start <= end) { // 确保范围有效
					for (int i = start; i <= end; ++i) {
						if (i > 0 && i <= 65535)
							ports.push_back(i);
					}
				}
			} catch (...) {
			}
		}
	}
	return ports;
}

/**
 * @brief C++ 核心扫描函数。
 * @param target 目标地址 (IP地址或域名)
 * @param ports_str 端口范围字符串 (e.g., "1-1024,80")
 * @param scan_type 扫描类型 ("tcp" 或 "udp")
 * @return std::vector<PortScanResult> 端口扫描结果列表
 */
std::vector<PortScanResult> ports_scan::execute_scan(
	const std::string &target,
	const std::string &ports_str,
	const std::string &scan_type)
{
	std::cout << "[C++ Core] Scanning target: " << target
			  << ", ports: " << ports_str
			  << ", type: " << scan_type << std::endl;

	auto start_time = std::chrono::high_resolution_clock::now();

	std::vector<int> ports_to_scan = parse_ports(ports_str);
	std::vector<PortScanResult> results;

	// 核心扫描循环
	for (int port : ports_to_scan) {
		PortScanResult result;
		result.port = port;

		if (port <= 0 || port > 65535)
			continue;

		if (scan_type == "tcp") {
			// 🚨 调用 TCP 扫描逻辑
			result.status = tcp_connect_scan(target, port);

		} else if (scan_type == "udp") {
			// ⭐️ 调用 UDP 扫描逻辑
			result.status = udp_scan(target, port);
		} else {
			result.status = "Error: Invalid Scan Type";
		}

		// 简单的服务模拟识别（Python 层将做更精确的识别）
		if (result.status == "Open") {
			if (port == 80)
				result.service = "http";
			else if (port == 443)
				result.service = "https";
			else if (port == 22)
				result.service = "ssh";
			else
				result.service = (scan_type == "udp") ? "udp-open" : "tcp-open"; // 区分 TCP/UDP 开放
		} else {
			result.service = "";
		}

		results.push_back(result);
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

	std::cout << "[C++ Core] Scan finished. Ports checked: " << ports_to_scan.size()
			  << ", Total time: " << duration << " ms." << std::endl;

	return results;
}

bool whois::beginsWith(const std::string &s, const std::string &prefix)
{
	return s.rfind(prefix, 0) == 0;
}
std::string whois::trim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t\r\n");
	size_t end = s.find_last_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	return s.substr(start, end - start + 1);
}
WhoisInfo whois::parseWhois(const std::string &rawText)
{
	WhoisInfo info;
	std::stringstream ss(rawText);
	std::string line;

	while (std::getline(ss, line, '\n')) {
		line = trim(line);

		// 跳过空行 / 噪声
		if (line.empty())
			continue;

		// 去除控制字符(\u0000等)
		line.erase(std::remove_if(line.begin(), line.end(),
					   [](char c) { return (unsigned char)c < 32; }),
			line.end());

		// ---- 匹配关键字段 ----
		if (beginsWith(line, "Domain Name:")) {
			info.domain = trim(line.substr(12));
		} else if (beginsWith(line, "Registry Domain ID:")) {
			info.registryDomainID = trim(line.substr(19));
		} else if (beginsWith(line, "Registrar WHOIS Server:")) {
			info.registrarWhoisServer = trim(line.substr(23));
		} else if (beginsWith(line, "Registrar URL:")) {
			info.registrarURL = trim(line.substr(14));
		} else if (beginsWith(line, "Registrar:")) {
			info.registrar = trim(line.substr(10));
		} else if (beginsWith(line, "Creation Date:")) {
			info.creationDate = trim(line.substr(14));
		} else if (beginsWith(line, "Updated Date:")) {
			info.updatedDate = trim(line.substr(13));
		} else if (beginsWith(line, "Registry Expiry Date:")) {
			info.expiryDate = trim(line.substr(20));
		} else if (beginsWith(line, "Domain Status:")) {
			info.statuses.push_back(trim(line.substr(14)));
		} else if (beginsWith(line, "Name Server:")) {
			info.nameServers.push_back(trim(line.substr(12)));
		} else if (beginsWith(line, "DNSSEC:")) {
			info.dnssec = trim(line.substr(7));
		}
	}

	return info;
}
bool whois::hostname2ip(const char *hostname, std::string &ip)
{
	struct hostent *he;
	struct in_addr **addr_list;
	if ((he = gethostbyname(hostname)) == nullptr) {
		return false;
	}
	addr_list = (struct in_addr **)he->h_addr_list;
	for (size_t i = 0; addr_list[i]; i++) {
		ip = inet_ntoa(*addr_list[i]);
		return true;
	}
	return false;
}

bool whois::whois_query(const char *server, const char *target, std::string &data)
{
	std::string ip, strTmp;
	int sock = 0;
	struct sockaddr_in dest;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	if (!hostname2ip(server, ip)) {
		return false;
	}
	dest.sin_addr.s_addr = inet_addr(ip.data());
	dest.sin_port = htons(43);
	if (connect(sock, (const struct sockaddr *)&dest, sizeof(dest)) < 0) {
		return false;
	}
	strTmp = target;
	strTmp += "\r\n";
	if (send(sock, strTmp.data(), strTmp.size(), 0) < 0) {
		return false;
	}
	int r = 0;
	char buff[2048] = {};
	while ((r = recv(sock, buff, sizeof(buff), 0))) {
		data += buff;
	}
	return true;
}

WhoisInfo whois::execute_whois(const char *target)
{
	std::string data;
	if (!whois_query("whois.iana.org", target, data)) {
		perror("whois_query");
	}
	char *wch = nullptr, *pch = nullptr;
	pch = strtok(data.data(), "\n");
	while (pch) {
		wch = strstr(pch, "whois.");
		if (wch) {
			break;
		}
		pch = strtok(nullptr, "\n");
	}
	if (wch) {
		whois_query(wch, target, data);
	}
	WhoisInfo info = parseWhois(data);
	info.domain = target;
	return info;
}

std::vector<PortScanResult> execute_scan_core(
	const std::string &target,
	const std::string &ports_str,
	const std::string &scan_type)
{
	ports_scan scan;
	return scan.execute_scan(target, ports_str, scan_type);
}

WhoisInfo execute_whois_core(const char *target)
{
	whois lookup;
	return lookup.execute_whois(target);
}