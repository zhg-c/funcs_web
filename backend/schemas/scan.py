from pydantic import BaseModel
from typing import List, Optional


# 请求模型
class ScanRequest(BaseModel):
    target: str
    ports: str
    scan_type: str = "tcp"


# 响应子模型
class ScanResult(BaseModel):
    port: int
    status: str
    service: Optional[str] = None


# 完整响应模型
class ScanResponse(BaseModel):
    status: str = "success"
    results: List[ScanResult]
    message: Optional[str] = None
