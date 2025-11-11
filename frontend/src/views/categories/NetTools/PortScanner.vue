<script setup lang="ts">
import { ref, computed } from "vue"; // onMounted 已移除

// 定义扫描结果的数据结构 (已修复: 替换 interface 为 type 以避免编译问题)
type ScanResult = {
  port: number;
  status: string;
  service?: string; // 可选的服务名称
};

const target = ref("127.0.0.1"); // 目标IP/域名
const portRange = ref("1-1024"); // 端口范围
const scanType = ref("tcp"); // 扫描类型 ("tcp" 或 "udp")
const showOnlyOpenPorts = ref(false); // 状态：控制是否只显示开放端口
const rawResults = ref<string>('点击 "开始扫描" 查看结果'); // 用于显示原始 JSON 字符串
const formattedResults = ref<ScanResult[]>([]); // 用于表格或列表展示
const isLoading = ref(false);
const error = ref<string | null>(null);

// 🚨 API 基础 URL：根据您当前的运行环境选择
const API_BASE_URL = "http://192.168.10.106:8000/api/v1";

// ------------------- 计算属性：根据复选框过滤结果 -------------------

const filteredResults = computed(() => {
  if (showOnlyOpenPorts.value) {
    // 只有勾选了复选框时，才过滤出状态为 'Open' 的端口
    return formattedResults.value.filter((result) => result.status === "Open");
  }
  // 否则，显示所有结果
  return formattedResults.value;
});

// ------------------- 新增：CSV 导出功能 -------------------

const exportToCsv = () => {
  // 导出时，导出当前视图中的结果 (即 filteredResults)
  const resultsToExport = filteredResults.value;

  if (resultsToExport.length === 0) {
    console.error("无法导出：没有扫描结果。");
    return;
  }

  // CSV 表头
  let csvContent = "Port,Status,Service\n";

  // 格式化数据行
  resultsToExport.forEach((result) => {
    // 确保 Service 字段不会包含逗号导致 CSV 格式混乱
    const serviceSafe = (result.service || "Unknown").replace(/,/g, "");
    csvContent += `${result.port},${result.status},${serviceSafe}\n`;
  });

  // 1. 创建 Blob 对象
  // 使用 'text/csv;charset=utf-8;' 确保 Excel 能够识别中文
  const blob = new Blob([csvContent], { type: "text/csv;charset=utf-8;" });

  // 2. 创建临时 URL
  const link = document.createElement("a");

  // 生成文件名
  const timestamp = new Date()
    .toISOString()
    .slice(0, 19)
    .replace("T", "_")
    .replace(/:/g, "-");
  link.download = `scan_results_${target.value}_${timestamp}.csv`;

  // 兼容性处理
  if (link.download !== undefined) {
    // 适用于大部分浏览器
    const url = URL.createObjectURL(blob);
    link.setAttribute("href", url);
    link.style.visibility = "hidden";
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    URL.revokeObjectURL(url);
  } else {
    // IE 10+ Fallback
    console.error("您的浏览器不支持自动下载。请复制原始 JSON 数据。");
    // 实际应用中会提供一个复制按钮或更友好的提示
  }
};

// ------------------- 新增结束 -------------------

const scanPorts = async () => {
  console.log("--- Scan button clicked! ---");

  console.log("Target:", target.value);
  isLoading.value = true;
  error.value = null;
  rawResults.value = "正在发送请求并扫描中...";
  formattedResults.value = [];

  const payload = {
    target: target.value,
    ports: portRange.value,
    scan_type: scanType.value,
  }; // API 调用与指数退避重试逻辑

  const makeApiCall = async (retryCount = 0): Promise<any> => {
    try {
      const response = await fetch(`${API_BASE_URL}/scan/ports`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(payload),
      });

      if (!response.ok) {
        // 处理 HTTP 错误 (4xx, 5xx)
        const errorData = await response.json();
        throw new Error(
          errorData.detail || `HTTP 错误: ${response.statusText}`
        );
      }

      return await response.json();
    } catch (err) {
      if (retryCount < 3) {
        const delay = Math.pow(2, retryCount) * 1000;
        await new Promise((resolve) => setTimeout(resolve, delay));
        return makeApiCall(retryCount + 1);
      }
      throw err;
    }
  };

  try {
    const data = await makeApiCall(); // 更新结果

    rawResults.value = JSON.stringify(data, null, 2);
    formattedResults.value = data.results;

    // (移除历史记录保存逻辑)
  } catch (err: any) {
    console.error("API Error:", err);
    error.value = `扫描失败: ${err.message}. 请检查后端服务和 CORS 配置。`;
    rawResults.value = "";
  } finally {
    isLoading.value = false;
  }
  console.log("--- Execution finished without fetch ---");
};
</script>

<template>
  <div class="params">
    <h2>🌐 端口扫描器</h2>
    <div class="scan-form">
      <form action="#">
        <label for="target">目标IP/域名：</label>
        <input
          type="text"
          name="target"
          id="target"
          v-model="target"
          placeholder="比如：127.0.0.1 或 example.com"
        />
        <label for="ports">端口范围：</label>
        <input
          type="text"
          name="ports"
          id="ports"
          v-model="portRange"
          placeholder="比如：1-1024 或 22,80,443"
        />
        <label for="scan-type">网络协议：</label>
        <select name="scan-type" id="scan-type" v-model="scanType">
          <option value="tcp">TCP</option>
          <option value="udp">UDP</option>
        </select>
        <input
          type="submit"
          name="protocol"
          id="protocol"
          :value="isLoading ? '正在扫描...' : '开始扫描'"
          :disabled="isLoading"
          @click="scanPorts"
        />
      </form>
    </div>
  </div>
  <div class="results">
    <br />
    <div class="err" v-if="error"><strong>错误:</strong> {{ error }}</div>
    <div v-if="formattedResults.length">
      <div>
        <label for="show-open">只显示开放端口</label>
        <input type="checkbox" name="show-open" v-model="showOnlyOpenPorts" />
        <div>
          <button @click="exportToCsv">📥 导出 CSV 格式</button>
        </div>
      </div>
      <h3>扫描结果</h3>
      <p>
        扫描完成！共发现
        {{ formattedResults.filter((r) => r.status === "Open").length }}
        个开放端口。
      </p>
      <div class="all-ports">
        <div
          v-for="result in filteredResults"
          :key="result.port"
          :class="{
            open: result.status === 'Open',
            closed: result.status === 'Closed',
            filtered: result.status === 'Filtered',
          }"
          :title="`端口: ${result.port}\n状态: ${result.status}\n服务: ${
            result.service || '未知'
          }`"
        >
          {{ result.port }}
        </div>
      </div>
    </div>
  </div>
</template>
<style scoped>
.params {
  display: grid;
  place-items: center;
}
div.scan-form {
  display: grid;
  place-items: center;
  max-width: 600px;
  width: 100%;
  border: 2px solid #04aa6d;
  border-radius: 4px;
  background-color: #f2f2f2;
  padding: 20px;
}
form {
  display: flex;
  flex-wrap: wrap;
  justify-content: space-between;
}
label {
  min-width: 48%;
  margin-bottom: 10px;
}
input[type="text"],
select {
  min-width: 45%;
  margin-bottom: 10px;
  border: 1px solid #ccc;
  border-radius: 4px;
  padding: 10px;
}
input[type="submit"] {
  margin: 20px auto 0;
  width: 50%;
  background-color: #04aa46;
  color: white;
  padding: 9px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}
@media screen and (max-width: 600px) {
  label,
  input[type="text"],
  input[type="submit"],
  select,
  textarea {
    flex: 100%;
    min-width: 100%;
  }
}

.err {
  display: flex;
  align-items: center;
  gap: 8px; /* strong 与文字之间的间距 */
  padding: 12px 16px;
  margin-top: 12px;

  background-color: #fee2e2; /* 浅红背景 */
  color: #991b1b; /* 深红文字 */
  border: 1px solid #fca5a5; /* 红色边框 */
  border-radius: 6px; /* 圆角 */
  font-size: 0.9rem;
  font-weight: 500;

  box-shadow: 0 1px 2px rgba(0, 0, 0, 0.05);
  transition: background-color 0.3s, box-shadow 0.3s;
}

.err strong {
  color: #b91c1c; /* 稍深一点的红色强调“错误:” */
}

.all-ports {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(50px, 1fr));
  gap: 8px;
  margin-top: 15px;
}
.all-ports div {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 50px; /* 方块高度 */
  width: 100%; /* 确保在网格中占满宽度 */
  border-radius: 4px;
  font-weight: 600;
  font-size: 0.9rem;
  cursor: default;
  transition: background-color 0.2s, box-shadow 0.2s;
  /* 默认/关闭/过滤状态 */
  background-color: #f7f7f7;
  color: #4b5563; /* 灰色文字 */
  border: 1px solid #e5e7eb;
}
.all-ports div.open {
  background-color: #10b981; /* Tailwind green-500 */
  color: white;
  border-color: #059669;
  /* 移除原来的静态阴影 */
  box-shadow: 0 4px 10px rgba(16, 185, 129, 0.4);
  transform: scale(1.02); /* 略微缩小放大效果 */
  /* 应用脉冲动画 */
  animation: pulse-glow 2s infinite cubic-bezier(0.66, 0, 0.34, 1);
}
.all-ports div:hover {
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
}
@keyframes pulse-glow {
  0% {
    box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.6); /* 初始状态，柔和绿色 */
  }
  70% {
    box-shadow: 0 0 0 10px rgba(16, 185, 129, 0); /* 扩散到 10px 后完全透明 */
  }
  100% {
    box-shadow: 0 0 0 0 rgba(16, 185, 129, 0);
  }
}
</style>
