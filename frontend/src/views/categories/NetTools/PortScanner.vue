<script setup lang="ts">
import { ref } from "vue";

// 定义扫描结果的数据结构
interface ScanResult {
  port: number;
  status: string;
  service?: string; // 可选的服务名称
}

const target = ref("127.0.0.1"); // 目标IP/域名
const portRange = ref("1-1024"); // 端口范围
const scanType = ref("tcp"); // 扫描类型
const rawResults = ref<string>('点击 "开始扫描" 查看结果'); // 用于显示原始 JSON 字符串
const formattedResults = ref<ScanResult[]>([]); // 用于表格或列表展示
const isLoading = ref(false);
const error = ref<string | null>(null);

// 🚨 API 基础 URL：根据您当前的运行环境选择
// 选项 1: 如果通过 Nginx 代理 api.example.com 访问
// const API_BASE_URL = 'http://api.example.com/api/v1';
// 选项 2: 如果前端直接调用本地 8000 端口
const API_BASE_URL = "http://192.168.10.106:8000/api/v1";

const scanPorts = async () => {
  // 🚨 检查点 1: 确认点击事件触发
  console.log("--- Scan button clicked! ---");

  // 检查点 2: 确认变量能正常访问
  console.log("Target:", target.value);
  isLoading.value = true;
  error.value = null;
  rawResults.value = "正在发送请求并扫描中...";
  formattedResults.value = [];

  const payload = {
    target: target.value,
    ports: portRange.value,
    scan_type: scanType.value,
  };

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
      throw new Error(errorData.detail || `HTTP 错误: ${response.statusText}`);
    }

    const data = await response.json();

    // 更新结果
    rawResults.value = JSON.stringify(data, null, 2);
    formattedResults.value = data.results;
  } catch (err) {
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
  <div class="port-scanner-view">
    <h1 class="text-3xl font-bold mb-6 text-gray-800">端口扫描器</h1>

    <!-- 输入表单区域 -->
    <div class="input-form bg-gray-50 p-6 rounded-xl shadow-inner mb-6">
      <div class="grid grid-cols-1 md:grid-cols-4 gap-4 items-end">
        <!-- 目标 IP/域名 -->
        <div class="col-span-1 md:col-span-2">
          <label
            for="target"
            class="block text-sm font-semibold text-gray-700 mb-2"
            >目标 IP / 域名</label
          >
          <input
            id="target"
            v-model="target"
            type="text"
            placeholder="e.g., 127.0.0.1 or example.com"
            class="w-full p-3 border border-gray-300 rounded-lg focus:ring-indigo-500 focus:border-indigo-500 transition duration-150"
          />
        </div>

        <!-- 端口范围 -->
        <div class="col-span-1">
          <label
            for="ports"
            class="block text-sm font-semibold text-gray-700 mb-2"
            >端口范围 (Port Range)</label
          >
          <input
            id="ports"
            v-model="portRange"
            type="text"
            placeholder="e.g., 1-1024 or 22,80,443"
            class="w-full p-3 border border-gray-300 rounded-lg focus:ring-indigo-500 focus:border-indigo-500 transition duration-150"
          />
        </div>

        <!-- 协议 (仅保留 TCP 选项) -->
        <div class="col-span-1">
          <label
            for="scan-type"
            class="block text-sm font-semibold text-gray-700 mb-2"
            >协议</label
          >
          <!-- 协议 (Protocol) 的 select 标签 -->
          <select
            id="scan-type"
            v-model="scanType"
            class="w-full p-3 border border-gray-300 rounded-lg focus:ring-blue-500 focus:border-blue-500 transition duration-150 bg-white shadow-sm appearance-none"
          >
             
            <option value="tcp">TCP (默认)</option>
             
            <option value="udp">UDP</option>
          </select>
        </div>
      </div>

      <!-- 扫描按钮 -->
      <button
        @click="scanPorts"
        :disabled="isLoading"
        class="scan-button mt-6 w-full py-3 px-4 font-semibold rounded-lg shadow-md transition duration-200 ease-in-out transform hover:scale-[1.01] focus:outline-none focus:ring-4 focus:ring-indigo-500 focus:ring-opacity-50"
        :class="{
          'bg-indigo-600 text-white hover:bg-indigo-700': !isLoading,
          'bg-gray-400 text-gray-700 cursor-not-allowed': isLoading,
        }"
      >
        <span v-if="isLoading">
          <i class="fas fa-spinner fa-spin mr-2"></i> 扫描中...
        </span>
        <span v-else> 🚀 开始扫描 </span>
      </button>
    </div>

    <!-- 状态和结果区域 -->
    <div class="results-area">
      <h2 class="text-2xl font-semibold mb-4 text-gray-800">扫描结果</h2>

      <!-- 状态消息 -->
      <div
        v-if="formattedResults.length"
        class="p-3 rounded-lg font-medium mb-4"
      ></div>

      <!-- 紧凑数字网格布局 -->
      <div v-if="formattedResults.length" class="results-grid">
        <div
          v-for="result in formattedResults"
          :key="result.port"
          class="result-block"
          :class="{
            open: result.status === 'Open',
            closed: result.status === 'Closed',
            filtered: result.status === 'Filtered',
          }"
          :title="`Port: ${result.port}\nStatus: ${result.status}\nService: ${
            result.service || '未知'
          }`"
        >
          {{ result.port }}
        </div>
      </div>

      <!-- 原始结果（调试用） -->
      <details class="raw-results-details mt-6">
        <summary
          class="cursor-pointer font-medium text-gray-600 hover:text-gray-800 transition duration-150"
        >
          原始 API 响应 (JSON)
        </summary>
        <pre
          class="results-code bg-gray-100 p-4 rounded-lg mt-2 text-sm overflow-x-auto"
          >{{ rawResults }}</pre
        >
      </details>
    </div>
  </div>
</template>
<style scoped>
/* 紧凑数字网格样式 */
.results-grid {
  display: grid;
  /* 核心样式：根据可用宽度自动创建列，每列最小 50px */
  grid-template-columns: repeat(auto-fill, minmax(50px, 1fr));
  gap: 8px; /* 端口块之间的间距 */
  margin-top: 15px;
}

.result-block {
  display: flex;
  align-items: center;
  justify-content: center;
  height: 50px; /* 方块高度 */
  width: 100%; /* 确保在网格中占满宽度 */
  padding: 5px;
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

/* 开放端口的视觉样式 (Open) - 绿色 */
.result-block.open {
  background-color: #10b981; /* Tailwind green-500 */
  color: white;
  border-color: #059669;
  /* 移除原来的静态阴影 */
  box-shadow: 0 4px 10px rgba(16, 185, 129, 0.4);
  transform: scale(1.02); /* 略微缩小放大效果 */
  /* 应用脉冲动画 */
  animation: pulse-glow 2s infinite cubic-bezier(0.66, 0, 0.34, 1);
}
.result-block:hover {
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
}

/* 其他组件的通用样式 */
.raw-results-details summary {
  list-style: none; /* 移除默认箭头 */
}
.raw-results-details summary::before {
  content: "▶"; /* 自定义箭头 */
  display: inline-block;
  margin-right: 8px;
  transition: transform 0.2s;
}
.raw-results-details[open] summary::before {
  content: "▼";
  transform: rotate(0deg);
}
/* 新增：开放端口的脉冲动画 */
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
