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
  <div class="tool-page">
    <h1 class="page-title">⚙️ 端口扫描器 (C++核心)</h1>
    <p class="description">基于高性能 C++ 核心的网络端口扫描工具。</p>

    <div class="input-form">
      <div class="form-group">
        <label for="target">目标地址 (IP/域名)</label>
        <input
          id="target"
          v-model="target"
          type="text"
          placeholder="例如: 192.168.1.1 或 example.com"
        />
      </div>

      <div class="form-group">
        <label for="ports">端口范围 (Port Range)</label>
        <input
          id="ports"
          v-model="portRange"
          type="text"
          placeholder="例如: 1-1024, 80, 443"
        />
      </div>

      <div class="form-group">
        <label for="scan-type">扫描类型</label>
        <select id="scan-type" v-model="scanType">
          <option value="tcp">TCP SYN/Connect</option>
          <option value="udp">UDP Scan</option>
        </select>
      </div>

      <button @click="scanPorts" :disabled="isLoading" class="scan-button">
        {{ isLoading ? "扫描中..." : "🚀 开始扫描" }}
      </button>

      <p v-if="error" class="error-message">❌ {{ error }}</p>
    </div>

    <div class="results-area">
      <h2>扫描结果</h2>

      <table v-if="formattedResults.length" class="results-table">
        <thead>
          <tr>
            <th>端口号</th>
            <th>状态</th>
            <th>服务</th>
          </tr>
        </thead>
        <tbody>
          <tr
            v-for="result in formattedResults"
            :key="result.port"
            :class="{ open: result.status === 'Open' }"
          >
            <td>{{ result.port }}</td>
            <td>{{ result.status }}</td>
            <td>{{ result.service || "-" }}</td>
          </tr>
        </tbody>
      </table>

      <details>
        <summary>原始 API 响应 (JSON)</summary>
        <pre class="results-code">{{ rawResults }}</pre>
      </details>
    </div>
  </div>
</template>

<style scoped>
/* 保持原有的样式不变，并新增表格样式 */
.error-message {
  color: #f44336;
  margin-top: 15px;
  font-weight: 600;
}
.results-table {
  width: 100%;
  border-collapse: collapse;
  margin-top: 15px;
}
.results-table th,
.results-table td {
  padding: 10px;
  text-align: left;
  border-bottom: 1px solid #ddd;
}
.results-table th {
  background-color: #f0fdf4;
  color: #36a374;
  font-weight: 700;
}
.results-table tr.open {
  background-color: #e6ffed; /* 开放端口高亮 */
  font-weight: 600;
}
.results-code {
  /* ... 保持不变 ... */
}
</style>
