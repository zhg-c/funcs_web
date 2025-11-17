// 定义功能模块，方便在 HomeView 中遍历展示，并在 Router 中自动注册路由
import PortScanner from "../views/categories/NetTools/PortScanner.vue";
import WhoIs from "../views/categories/NetTools/WhoIs.vue";
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
      // 更多网络工具...
    ],
  },
  // 更多分类...
];
