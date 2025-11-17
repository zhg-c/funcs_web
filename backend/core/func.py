import os
import sys

# ----------------------------------------------------------------------
# C++ MODULE DYNAMIC LOADING
# ----------------------------------------------------------------------
# 动态将当前目录添加到 Python 路径，确保可以导入 C++ 模块
# 模块名为 port_scanner_core，编译后文件名为 func_core.so/.pyd
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    # 🚨 导入 C++ 模块 (模块名在 C++ 文件中通过 PYBIND11_MODULE 定义)
    import func_core

    print("[Python Wrapper] Successfully imported C++ func_core module.")

    # 将 C++ 模块中的 PortScanResult 结构体映射为 Python 类型
    PortScanResult = func_core.PortScanResult
    WhoisInfo = func_core.WhoisInfo

except ImportError as e:
    print(f"[Python Wrapper] ERROR: Failed to import C++ core module 'func_core'.")
    print(f"Details: {e}")

    # 在无法导入 C++ 模块时，定义一个备用类，确保 Python 代码不崩溃
    class PortScanResult:
        def __init__(self, port, status, service):
            self.port = port
            self.status = status
            self.service = service

    def execute_scan(target: str, ports: str, scan_type: str) -> list[PortScanResult]:
        """Fallback mock function if C++ module is missing."""
        print(f"[Python Wrapper] C++ module missing. Running mock scan.")
        # 返回一个包含错误信息的模拟结果，让用户知道核心未加载
        return [
            PortScanResult(port=1, status="Error", service="C++ Core Missing"),
            PortScanResult(port=2, status="Error", service="Check Build Step"),
        ]

    class WhoisInfo:
        def __init__(
            self,
            domain=None,
            registryDomainID=None,
            registrar=None,
            registrarWhoisServer=None,
            registrarURL=None,
            creationDate=None,
            updatedDate=None,
            expiryDate=None,
            statuses=None,
            nameServers=None,
            dnssec=None,
        ):
            self.domain = domain
            self.registryDomainID = registryDomainID
            self.registrar = registrar
            self.registrarWhoisServer = registrarWhoisServer
            self.registrarURL = registrarURL
            self.creationDate = creationDate
            self.updatedDate = updatedDate
            self.expiryDate = expiryDate
            self.statuses = statuses or []
            self.nameServers = nameServers or []
            self.dnssec = dnssec

    def execute_whois(target: str) -> WhoisInfo:
        """Fallback mock function if C++ module is missing."""
        print(f"[Python Wrapper] C++ module missing. Running mock whois lookup.")

        # 返回一个模拟结果，让用户知道核心未加载
        return WhoisInfo(
            domain=target,
            registryDomainID="Error",
            registrar="C++ Core Missing",
            registrarWhoisServer=None,
            registrarURL=None,
            creationDate=None,
            updatedDate=None,
            expiryDate=None,
            statuses=[],
            nameServers=[],
            dnssec=None,
        )

    print("[Python Wrapper] Running in safe mock mode.")


# ----------------------------------------------------------------------
# 主执行函数
# ----------------------------------------------------------------------

# 只有当 C++ 模块导入成功时，我们才使用其真正的 execute_scan 函数
if "func_core" in sys.modules:

    # 🚨 这里的 execute_scan 函数指向 C++ 绑定函数
    # 重新定义 execute_scan 函数以确保类型正确性并调用 C++ 核心
    def execute_scan(target: str, ports: str, scan_type: str) -> list[PortScanResult]:
        """
        Calls the C++ core function and ensures the return type.
        """
        # C++ 函数调用
        results_cpp = func_core.execute_scan(target, ports, scan_type)

        # C++ 对象列表直接返回，可以被 Pydantic 很好地处理
        return results_cpp

    def execute_whois(target: str) -> WhoisInfo:
        """
        Calls the C++ core function and ensures the return type.
        """
        # C++ 函数调用
        results_cpp = func_core.execute_whois(target)

        # C++ 对象列表直接返回，可以被 Pydantic 很好地处理
        return results_cpp
