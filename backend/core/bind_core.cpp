#include "func_core.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(func_core, m)
{
	m.doc() = "Pybind11 wrapper for the C++ func core.";

	// ports
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

	// whois

	py::class_<WhoisInfo>(m, "WhoisInfo")
		.def(py::init<>())
		.def_readwrite("domain", &WhoisInfo::domain)
		.def_readwrite("registryDomainID", &WhoisInfo::registryDomainID)
		.def_readwrite("registrar", &WhoisInfo::registrar)
		.def_readwrite("registrarWhoisServer", &WhoisInfo::registrarWhoisServer)
		.def_readwrite("registrarURL", &WhoisInfo::registrarURL)
		.def_readwrite("creationDate", &WhoisInfo::creationDate)
		.def_readwrite("updatedDate", &WhoisInfo::updatedDate)
		.def_readwrite("expiryDate", &WhoisInfo::expiryDate)
		.def_readwrite("statuses", &WhoisInfo::statuses)
		.def_readwrite("nameServers", &WhoisInfo::nameServers)
		.def_readwrite("dnssec", &WhoisInfo::dnssec);

	m.def("execute_whois", &execute_whois_core,
		"Executes a port scan against a target.",
		py::arg("target"));
}