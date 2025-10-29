# backend/services/scan_service.py

from ..core.port_scanner import execute_scan
from ..schemas.scan import ScanRequest, ScanResult


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
    results: list[ScanResult] = [ScanResult(**r) for r in raw_data]

    return results
