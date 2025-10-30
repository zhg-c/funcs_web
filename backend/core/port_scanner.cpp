#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <vector>
#include <string>
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

namespace py = pybind11;

// 定义超时时间 (毫秒)
constexpr int TIMEOUT_MS = 500;

// =================================================================
// C++ Data Structures matching Python Schemas
// =================================================================

// 端口扫描结果结构体 (对应 Python 的 ScanResult)
struct PortScanResult {
	int port;
	std::string status;
	std::string service; // 服务的识别逻辑将在 Python 层处理，这里暂时返回空或模拟值
};

// =================================================================
// CORE SCANNING LOGIC (Real TCP Connect Scan)
// =================================================================

/**
 * @brief 尝试与指定 IP 和端口建立 TCP 连接。
 * @param ip_address 目标IP地址
 * @param port 目标端口
 * @return 状态 ("Open", "Closed", "Filtered")
 */
std::string tcp_connect_scan(const std::string &ip_address, int port)
{
	int sock = -1;
	std::string result = "Closed";
	struct sockaddr_in target_addr;

	// 1. 创建套接字
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		// 创建套接字失败，视为过滤
		return "Filtered";
	}

	// 2. 配置目标地址结构
	target_addr.sin_family = AF_INET;
	target_addr.sin_port = htons(port);

	// 将 IP 地址字符串转换为网络字节序
	if (inet_pton(AF_INET, ip_address.c_str(), &target_addr.sin_addr) <= 0) {
		// IP地址无效
		close(sock);
		return "Filtered";
	}

	// 3. 设置非阻塞模式和超时
	// connect() 阻塞会很慢，因此我们使用非阻塞模式配合 select/poll 模拟超时
	fcntl(sock, F_SETFL, O_NONBLOCK);

	// 4. 发起连接
	int conn_result = connect(sock, (struct sockaddr *)&target_addr, sizeof(target_addr));

	if (conn_result < 0 && errno != EINPROGRESS) {
		// 立即失败，端口关闭或不可达
		result = "Closed";
	} else if (conn_result == 0) {
		// 立即连接成功 (极少数情况下发生)
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
				// getsockopt 失败，视为过滤
				result = "Filtered";
			} else if (so_error == 0) {
				// 成功连接
				result = "Open";
			} else {
				// 连接被拒绝 (例如 Connection Refused)
				result = "Closed";
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

// 辅助函数：解析端口范围字符串 ("1-100,22,8080")
std::vector<int> parse_ports(const std::string &ports_str)
{
	std::vector<int> ports;
	std::stringstream ss(ports_str);
	std::string segment;

	while (std::getline(ss, segment, ',')) {
		size_t dash_pos = segment.find('-');
		if (dash_pos == std::string::npos) {
			// 单个端口，例如 "80"
			try {
				ports.push_back(std::stoi(segment));
			} catch (...) {
			}
		} else {
			// 范围端口，例如 "1-100"
			try {
				int start = std::stoi(segment.substr(0, dash_pos));
				int end = std::stoi(segment.substr(dash_pos + 1));
				if (start <= end) { // 确保范围有效
					for (int i = start; i <= end; ++i) {
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
 * @param target 目标地址 (IP地址)
 * @param ports_str 端口范围字符串 (e.g., "1-1024,80")
 * @param scan_type 扫描类型 (目前仅支持 "tcp")
 * @return std::vector<PortScanResult> 端口扫描结果列表
 */
std::vector<PortScanResult> execute_scan_core(
	const std::string &target,
	const std::string &ports_str,
	const std::string &scan_type)
{
	// ⚠️ 注意：为了简化，我们假设 target 已经是有效的 IP 地址
	// 实际项目中，需要在这里或 Python 层进行 DNS 解析

	std::cout << "[C++ Core] Scanning target: " << target
			  << ", ports: " << ports_str
			  << ", type: " << scan_type << std::endl;

	// 计时开始
	auto start_time = std::chrono::high_resolution_clock::now();

	std::vector<int> ports_to_scan = parse_ports(ports_str);
	std::vector<PortScanResult> results;

	// 检查扫描类型，目前仅支持 TCP Connect Scan
	if (scan_type != "tcp") {
		std::cerr << "[C++ Core] Error: Only TCP connect scan is currently implemented." << std::endl;
		return results;
	}

	// 核心扫描循环
	for (int port : ports_to_scan) {
		// 限制：跳过无效或保留端口
		if (port <= 0 || port > 65535)
			continue;

		PortScanResult result;
		result.port = port;

		// 🚨 调用实际的 Socket 扫描逻辑
		result.status = tcp_connect_scan(target, port);

		// 简单的服务模拟识别（Python 层将做更精确的识别）
		if (result.status == "Open") {
			if (port == 80)
				result.service = "http";
			else if (port == 443)
				result.service = "https";
			else if (port == 22)
				result.service = "ssh";
			else
				result.service = "";
		} else {
			result.service = "";
		}

		results.push_back(result);
	}

	// 计时结束
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

	std::cout << "[C++ Core] Scan finished. Ports checked: " << ports_to_scan.size()
			  << ", Total time: " << duration << " ms." << std::endl;

	return results;
}

// =================================================================
// Pybind11 Binding
// =================================================================

PYBIND11_MODULE(port_scanner_core, m)
{
	m.doc() = "Pybind11 wrapper for the C++ port scanning core.";

	// 1. 绑定 C++ 结构体 PortScanResult 到 Python 类
	py::class_<PortScanResult>(m, "PortScanResult")
		.def(py::init<>())
		.def_readwrite("port", &PortScanResult::port)
		.def_readwrite("status", &PortScanResult::status)
		.def_readwrite("service", &PortScanResult::service);

	// 2. 绑定核心扫描函数 execute_scan_core
	m.def("execute_scan", &execute_scan_core,
		"Executes a port scan against a target.",
		py::arg("target"), py::arg("ports_str"), py::arg("scan_type"));
}
