from fastapi import APIRouter, HTTPException
from ....schemas.func import ScanRequest, ScanResponse, comRequest, WhoisResponse
from ....schemas.func import DNSRecordResponse
from ....services.func_service import process_port_scan, process_whois, process_dns_record

router = APIRouter(prefix="/func", tags=["kali"])


# --------------ports-------------------


@router.post("/ports", response_model=ScanResponse)
async def scan_ports_endpoint(request: ScanRequest):
    """端口扫描 API 接口"""
    try:
        results = process_port_scan(request)

        return ScanResponse(
            status="success", results=results, message=f"Scan complete for {request.target}"
        )
    except ValueError as e:
        # 处理业务逻辑中的输入错误
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        # 处理底层 C++ 或其他错误
        raise HTTPException(status_code=500, detail="An internal error occurred during the scan.")


# --------------whois-------------------


@router.post("/whois", response_model=WhoisResponse)
async def whois_endpoint(request: comRequest):
    """域名解析 API 接口"""
    try:
        results = process_whois(request)

        return WhoisResponse(
            status="success", results=results, message=f"doman lookup complete for {request.target}"
        )
    except ValueError as e:
        # 处理业务逻辑中的输入错误
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        # 处理底层 C++ 或其他错误
        raise HTTPException(
            status_code=500, detail="An internal error occurred during the doman lookup."
        )


# --------------dns_record-------------------


@router.post("/dns_record", response_model=DNSRecordResponse)
async def dns_record_endpoint(request: comRequest):
    try:
        results = process_dns_record(request)

        return DNSRecordResponse(
            status="success",
            results=results,
            message=f"doman record lookup complete for {request.target}",
        )
    except ValueError as e:
        # 处理业务逻辑中的输入错误
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        # 处理底层 C++ 或其他错误
        raise HTTPException(
            status_code=500, detail="An internal error occurred during the doman record lookup."
        )
