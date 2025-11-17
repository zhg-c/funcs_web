// 定义功能模块，方便在 HomeView 中遍历展示，并在 Router 中自动注册路由
import PortScanner from "../views/categories/NetTools/PortScanner.vue";
import WhoIs from "../views/categories/NetTools/WhoIs.vue";
import DNSLookup from "../views/categories/NetTools/DNSLookup.vue";
import PingTool from "../views/categories/NetTools/PingTool.vue";
import TracerouteTool from "../views/categories/NetTools/TracerouteTool.vue";
import SSLInfo from "../views/categories/NetTools/SSLInfo.vue";
// 注意：这个文件我们下一轮再创建

export const FEATURE_CATEGORIES = [
  {
    id: "net-tools",
    name: "🌐 网络安全工具",
    description: "基于 Kali Linux 核心思想的网络分析工具。",
    features: [
      {
        name: "端口扫描器",
        path: "/net/port-scanner",
        icon: "scan",
        component: PortScanner,
        description: "高性能 C++ 驱动的 TCP/UDP 端口扫描功能。",
      },
      {
        name: "域名信息查询",
        path: "/net/whois",
        icon: "whois",
        component: WhoIs,
        description: "查询域名（或 IP 地址）注册信息。",
      },
      {
        name: "DNS 记录查询",
        path: "/net/dns_record",
        icon: "dns_record",
        component: DNSLookup,
        description: "查询域名的 A / AAAA / CNAME / MX / TXT 等 DNS 记录。",
      },
      {
        name: "Ping 连通性测试",
        path: "/net/ping",
        icon: "ping",
        component: PingTool,
        description: "测试目标主机连通性，实时展示 RTT 延迟与统计结果。",
      },
      {
        name: "Traceroute 路由追踪",
        path: "/net/traceroute",
        icon: "route",
        component: TracerouteTool,
        description: "逐跳追踪网络路由路径，分析网络延迟和瓶颈节点。",
      },
      {
        name: "SSL 证书检查",
        path: "/net/ssl",
        icon: "ssl",
        component: SSLInfo,
        description:
          "查看网站的 SSL/TLS 证书详情，包括颁发者、有效期、SAN 等。",
      },
      // 更多网络工具...
    ],
  },
  // 更多分类...
];
