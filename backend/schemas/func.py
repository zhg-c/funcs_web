from pydantic import BaseModel
from typing import List, Optional


class comRequest(BaseModel):
    target: str


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


class WhoisInfo(BaseModel):
    domain: str
    registryDomainID: Optional[str] = None
    registrar: Optional[str] = None
    registrarWhoisServer: Optional[str] = None
    registrarURL: Optional[str] = None
    creationDate: Optional[str] = None
    updatedDate: Optional[str] = None
    expiryDate: Optional[str] = None
    statuses: List[str] = []
    nameServers: List[str] = []
    dnssec: Optional[str] = None


class WhoisResponse(BaseModel):
    status: str = "success"
    results: WhoisInfo
    message: Optional[str] = None
