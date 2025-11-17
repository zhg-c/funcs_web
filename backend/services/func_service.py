# backend/services/func_service.py

from ..core.func import execute_scan, execute_whois
from ..schemas.func import ScanRequest, ScanResult, WhoisInfo, comRequest


def process_port_scan(request: ScanRequest) -> list[ScanResult]:
    """
    业务逻辑：接收请求，调用核心模块，并返回 Pydantic 模型列表。
    """

    # 1. 业务逻辑验证 (例如：检查端口范围格式是否正确)
    if not all(c.isdigit() or c in ",-" for c in request.ports.replace(" ", "")):
        raise ValueError("Invalid port format.")

    # 2. 调用 C++ 核心
    raw_data = execute_scan(request.target, request.ports, request.scan_type)

    # 3. 数据转换 (将 C++ 返回的字典转换为 Pydantic 模型)
    # results: list[ScanResult] = [ScanResult(**r) for r in raw_data]
    results = []
    for r in raw_data:
        # 强制将 Pybind11 对象属性转换为标准 Python 字典
        d = {"port": r.port, "status": r.status, "service": r.service}

        # 使用字典解包构造 Pydantic 模型
        results.append(ScanResult(**d))

    return results


def process_whois(request: comRequest) -> WhoisInfo:
    """
    业务逻辑：接收请求，调用核心模块，并返回 Pydantic 模型列表。
    """
    raw_data = execute_whois(request.target)
    whois_info_dict = {
        "domain": raw_data.domain,
        "registryDomainID": raw_data.registryDomainID,
        "registrar": raw_data.registrar,
        "registrarWhoisServer": raw_data.registrarWhoisServer,
        "registrarURL": raw_data.registrarURL,
        "creationDate": raw_data.creationDate,
        "updatedDate": raw_data.updatedDate,
        "expiryDate": raw_data.expiryDate,
        "statuses": raw_data.statuses,
        "nameServers": raw_data.nameServers,
        "dnssec": raw_data.dnssec,
    }

    return whois_info_dict
