#cmake compile

```
cmake .. -DPYBIND11_DIR=/home/zhg/work/funcs_web/.venv/lib/python3.13/site-packages/pybind11/share/cmake/pybind11
mv port_scanner_core.cpython-313-x86_64-linux-gnu.so ..
```

#run serv

```
cd ~zhg/work/func_web/
python3 -m uvicorn backend.main:app --reload --host 0.0.0.0 --port 8000
```
