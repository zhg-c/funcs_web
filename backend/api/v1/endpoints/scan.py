from fastapi import APIRouter, HTTPException
from ....schemas.scan import ScanRequest, ScanResponse
from ....services.scan_service import process_port_scan

router = APIRouter(prefix="/scan", tags=["Scanning"])


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
