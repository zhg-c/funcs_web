from fastapi import APIRouter
from .endpoints import scan  # 导入我们刚刚定义的路由

api_router = APIRouter()

# 包含所有的接口模块
api_router.include_router(scan.router)
