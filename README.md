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

🧩 一、WHOIS 是什么

WHOIS 是一种网络服务和协议，用来查询域名（或 IP 地址）注册信息。
简单来说，当你输入一个网站，比如：

www.example.com

WHOIS 查询会去全球的注册机构（如 Verisign、ICANN、APNIC 等）请求该域名的注册信息，并返回一份“身份证”式报告。

📜 二、WHOIS 查询的典型返回信息包括：
字段 含义
Domain Name 域名，例如 example.com
Registrar 域名注册商，例如 GoDaddy、Namecheap
Registrant 域名所有者（个人或公司）
Creation Date 域名注册时间
Expiration Date 域名过期时间
Name Servers 负责解析该域名的 DNS 服务器
Emails 联系邮箱（技术、管理、滥用举报邮箱等）
Updated Date 最后更新时间
Raw Text 原始 WHOIS 服务器返回内容（完整文本）
🕵️ 三、WHOIS 的实际用途
1️⃣ 网络安全分析

调查目标网站背后的注册组织或个人。

识别钓鱼网站、假冒域名（如域名注册时间过短、匿名注册）。

判断网站是否与其他可疑站点有关联（通过邮箱、注册商、NS 服务器）。

2️⃣ 渗透测试与威胁情报

通过 WHOIS 信息确定潜在的攻击面（例如子域、同注册邮箱关联的域）。

建立域名与 IP 的情报图谱。

WHOIS 数据常用于 OSINT（开放源情报）分析。

3️⃣ 网络维护与管理

查询自己域名的到期时间，防止忘记续费。

检查域名是否被别人抢注或伪造。

了解某 IP 所属的组织（用于防火墙白名单、日志分析等）。

4️⃣ 法律取证

确认网站归属，提供给执法机关或法务部门追踪网络犯罪。

⚙️ 四、你现在实现的 WHOIS 功能的定位

在你的项目中，它会成为：

🔍 网络安全工具集中用于“域名信息查询”的一个模块。

它可以：

在前端输入域名（如 baidu.com、github.com）；

通过后端（C++ 实现高性能查询）执行 WHOIS；

返回结构化结果到前端展示（比如注册时间、注册商、DNS、邮箱等）。

未来还可以扩展：

自动识别域名所属国家；

调用 IP 地理位置接口；

与端口扫描结果联动，分析域名/IP 的综合信息。
