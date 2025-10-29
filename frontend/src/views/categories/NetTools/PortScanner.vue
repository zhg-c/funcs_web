<script setup lang="ts">
import { ref } from "vue";

// 假设我们有一个简单的API调用函数，稍后会实现
const scanPorts = async () => {
  // 1. 数据校验 (略)
  // 2. 准备 payload
  const payload = {
    target: target.value,
    ports: portRange.value, // e.g., "1-1024"
    scan_type: scanType.value,
  };

  results.value = "正在扫描中...请稍候。";
  isLoading.value = true;

  try {
    // 3. 调用后端 API (需要替换成实际的 fetch/axios 调用)
    // const response = await fetch('http://api.example.com/scan_ports', { ... });

    // 模拟 API 响应
    await new Promise((resolve) => setTimeout(resolve, 2000));
    const mockData = [
      { port: 21, status: "Closed" },
      { port: 22, status: "Open", service: "SSH" },
      { port: 80, status: "Open", service: "HTTP" },
    ];

    results.value = JSON.stringify(mockData, null, 2);
  } catch (error) {
    results.value = `扫描失败: ${error.message}`;
  } finally {
    isLoading.value = false;
  }
};

// 表单数据绑定
const target = ref("127.0.0.1"); // 目标IP/域名
const portRange = ref("1-1024"); // 端口范围
const scanType = ref("tcp"); // 扫描类型
const results = ref('点击 "开始扫描" 查看结果');
const isLoading = ref(false);
</script>

<template>
  <div class="tool-page">
    <h1 class="page-title">⚙️ 端口扫描器 (C++核心)</h1>
    <p class="description">
      基于高性能 C++ 核心的网络端口扫描工具，支持多种协议和范围扫描。
    </p>

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
    </div>

    <div class="results-area">
      <h2>扫描结果</h2>
      <pre class="results-code">{{ results }}</pre>
    </div>
  </div>
</template>

<style scoped>
.tool-page {
  max-width: 900px;
  margin: 0 auto;
  padding: 20px;
}
.page-title {
  font-size: 2em;
  color: #2c3e50;
  margin-bottom: 5px;
}
.description {
  color: #6a737d;
  margin-bottom: 30px;
  padding-bottom: 15px;
  border-bottom: 1px solid #eee;
}
.input-form {
  background-color: #f8f8f8;
  padding: 25px;
  border-radius: 8px;
  margin-bottom: 30px;
}
.form-group {
  margin-bottom: 15px;
}
label {
  display: block;
  font-weight: 600;
  margin-bottom: 5px;
  color: #333;
}
input[type="text"],
select {
  width: 100%;
  padding: 10px;
  border: 1px solid #ccc;
  border-radius: 4px;
  box-sizing: border-box;
  font-size: 1em;
}
.scan-button {
  padding: 12px 20px;
  background-color: #42b883;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 1.1em;
  transition: background-color 0.3s;
  width: 100%;
}
.scan-button:hover:not(:disabled) {
  background-color: #36a374;
}
.scan-button:disabled {
  background-color: #a8d5c4;
  cursor: not-allowed;
}
.results-area {
  margin-top: 20px;
}
.results-code {
  background-color: #272822; /* 深色背景，类似代码编辑器 */
  color: #f8f8f2;
  padding: 15px;
  border-radius: 4px;
  overflow-x: auto;
  white-space: pre-wrap;
  font-family: "Consolas", "Monaco", monospace;
  min-height: 150px;
}
</style>
