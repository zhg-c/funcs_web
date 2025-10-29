from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from .api.v1.api import api_router  # 导入聚合路由

app = FastAPI(
    title="Kali Utility Web API", description="High-performance backend powered by C++ core."
)

# --- CORS 配置 ---
origins = [
    "http://127.0.0.1:5173",
    "http://localhost:5173",
    "http://192.168.10.106:5173",  # 确保 IP 地址也可以访问
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# 注册所有 API 路由，使用 /api 前缀
app.include_router(api_router, prefix="/api/v1")


# 根路径健康检查 (可选)
@app.get("/")
def read_root():
    return {"status": "ok", "message": "API is running"}


# 之前的测试路由可以移除或修改
# @app.get("/api/get_hello/")
# async def get_hello_from_cpp():
#     return {"message": "hello,world"}
