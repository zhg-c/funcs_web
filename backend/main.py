from fastapi import FastAPI, HTTPException
from fastapi.responses import JSONResponse
from fastapi.middleware.cors import CORSMiddleware
import uvicorn

# 移除 import json

# 假设 libcore 是在顶层导入（即 name="libcore"）
try:
    import libcore
except ImportError:
    print("FATAL ERROR: Could not import libcore. Did you run 'pip install -e .'?")
    raise

app = FastAPI(title="Simple Hello Service")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # 仅用于开发环境
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.get("/api/get_hello/")
async def get_hello_from_cpp():
    print("FastAPI 收到请求，调用 C++ 核心 func_test()...")

    try:
        # 调用 C++ 导出的函数
        result_from_cpp = libcore.func_test()

        # FastAPI 会自动处理 JSONResponse，无需手动 import json
        return JSONResponse(
            content={
                "status": "success",
                "message": "C++ 调用成功",
                "hello_message": result_from_cpp,
            }
        )

    except Exception as e:
        print(f"C++ Core 调用错误：{e}")
        # 返回 500 错误
        raise HTTPException(status_code=500, detail=f"Internal C++ Core Error: {e}")


if __name__ == "__main__":
    # 🚨 关键修正：确保使用 __main__
    uvicorn.run(app, host="0.0.0.0", port=8000)
