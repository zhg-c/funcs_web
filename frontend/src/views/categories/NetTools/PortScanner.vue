<script setup lang="ts">
import { ref } from "vue"; // onMounted 已移除

// 定义扫描结果的数据结构 (已修复: 替换 interface 为 type 以避免编译问题)
type ScanResult = {
  port: number;
  status: string;
  service?: string; // 可选的服务名称
};

const target = ref("127.0.0.1"); // 目标IP/域名
const portRange = ref("1-1024"); // 端口范围
const scanType = ref("tcp"); // 扫描类型 ("tcp" 或 "udp")
const rawResults = ref<string>('点击 "开始扫描" 查看结果'); // 用于显示原始 JSON 字符串
const formattedResults = ref<ScanResult[]>([]); // 用于表格或列表展示
const isLoading = ref(false);
const error = ref<string | null>(null);

// 🚨 API 基础 URL：根据您当前的运行环境选择
const API_BASE_URL = "http://192.168.10.106:8000/api/v1";

// ------------------- 新增：CSV 导出功能 -------------------

const exportToCsv = () => {
  if (formattedResults.value.length === 0) {
    // 使用 console 替代 alert()
    console.error("无法导出：没有扫描结果。");
    return;
  }

  // CSV 表头
  let csvContent = "Port,Status,Service\n";

  // 格式化数据行
  formattedResults.value.forEach((result) => {
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
   
  <!-- Main container with max-width and auto margins for centering on desktop -->
   
  <div class="port-scanner-view max-w-4xl mx-auto p-4 font-sans">
       
    <h1 class="text-3xl font-extrabold mb-8 text-gray-900 border-b pb-2">
      🌐 端口扫描器
    </h1>

       
    <!-- 输入表单区域 -->
       
    <div
      class="input-form bg-white p-6 rounded-xl shadow-2xl mb-8 border border-gray-100"
    >
           
      <!-- Use items-start to ensure labels align neatly above inputs -->
           
      <div class="grid grid-cols-1 md:grid-cols-4 gap-6 items-start">
               
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
            class="w-full p-3 border border-gray-300 rounded-lg focus:ring-blue-500 focus:border-blue-500 transition duration-150 shadow-sm"
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
            class="w-full p-3 border border-gray-300 rounded-lg focus:ring-blue-500 focus:border-blue-500 transition duration-150 shadow-sm"
          />
                 
        </div>

               
        <!-- 协议 (Protocol) -->
               
        <div class="col-span-1">
                   
          <label
            for="scan-type"
            class="block text-sm font-semibold text-gray-700 mb-2"
            >协议 (Protocol)</label
          >
                   
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

           
      <!-- 扫描按钮和导出按钮 -->
      <div class="flex flex-col sm:flex-row gap-4 mt-8">
        <!-- 扫描按钮 -->
        <button
          @click="scanPorts"
          :disabled="isLoading"
          class="scan-button w-full sm:w-3/4 py-3 px-4 font-bold text-lg rounded-xl shadow-lg transition duration-300 ease-in-out transform hover:scale-[1.005] hover:shadow-xl focus:outline-none focus:ring-4 focus:ring-blue-500 focus:ring-opacity-70"
          :class="{
            'bg-blue-600 text-white hover:bg-blue-700': !isLoading,
            'bg-gray-400 text-gray-700 cursor-not-allowed': isLoading,
          }"
        >
          <span v-if="isLoading">
            <svg
              class="animate-spin -ml-1 mr-3 h-5 w-5 text-white inline"
              xmlns="http://www.w3.org/2000/svg"
              fill="none"
              viewBox="0 0 24 24"
            >
              <circle
                class="opacity-25"
                cx="12"
                cy="12"
                r="10"
                stroke="currentColor"
                stroke-width="4"
              ></circle>
              <path
                class="opacity-75"
                fill="currentColor"
                d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"
              ></path>
            </svg>
            扫描中...
          </span>
          <span v-else> 🚀 开始扫描 </span>
        </button>

        <!-- 新增：导出按钮 -->
        <button
          @click="exportToCsv"
          :disabled="formattedResults.length === 0"
          class="w-full sm:w-1/4 py-3 px-4 font-bold text-sm rounded-xl shadow-lg transition duration-300 ease-in-out transform hover:scale-[1.005] focus:outline-none focus:ring-4 focus:ring-green-500 focus:ring-opacity-70"
          :class="{
            'bg-green-500 text-white hover:bg-green-600':
              formattedResults.length > 0,
            'bg-gray-300 text-gray-500 cursor-not-allowed':
              formattedResults.length === 0,
          }"
        >
          📥 导出 CSV
        </button>
      </div>

           
      <!-- 错误信息显示 -->
           
      <div
        v-if="error"
        class="mt-4 p-4 bg-red-100 border border-red-400 text-red-700 rounded-lg text-sm font-medium"
      >
                <strong>错误:</strong> {{ error }}      
      </div>
         
    </div>

    <!-- (移除历史记录区域) -->

       
    <!-- 状态和结果区域 -->
       
    <div class="results-area mt-10">
           
      <h2 class="text-2xl font-bold mb-6 text-gray-800">结果可视化</h2>

           
      <!-- 状态消息 -->
           
      <div
        v-if="formattedResults.length"
        class="p-4 bg-green-50 text-green-700 rounded-lg font-medium mb-6 border border-green-200"
      >
                扫描完成！共发现
        {{ formattedResults.filter((r) => r.status === "Open").length }}
        个开放端口。      
      </div>

           
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
          :title="`端口: ${result.port}\n状态: ${result.status}\n服务: ${
            result.service || '未知'
          }`"
        >
          <!-- 开放端口显示端口号和发现的服务 -->
          <div
            v-if="result.status === 'Open'"
            class="flex flex-col items-center"
          >
            <span class="text-lg font-bold">{{ result.port }}</span>
            <span
              v-if="result.service"
              class="text-xs font-light mt-[-4px] opacity-90"
              >{{ result.service }}</span
            >
          </div>
          <!-- 其他状态只显示端口号 -->
          <div v-else class="text-lg font-bold">
            {{ result.port }}
          </div>
                 
        </div>
               
        <div
          v-if="
            scanType === 'udp' &&
            formattedResults.every(
              (r) =>
                r.status ===
                'Error: Requires CAP_NET_RAW or root to use RAW sockets'
            )
          "
          class="col-span-full p-4 bg-yellow-100 text-yellow-800 rounded-lg mt-4 text-center"
        >
                    <strong>UDP 扫描提示：</strong> 由于安全限制，UDP
          扫描需要原始套接字权限
          (root/CAP_NET_RAW)。如果您在受限环境中运行，结果可能显示为错误。      
           
        </div>
             
      </div>

           
      <!-- 原始结果（调试用） -->
           
      <details class="raw-results-details mt-8">
               
        <summary
          class="cursor-pointer font-semibold text-gray-700 hover:text-gray-900 transition duration-150"
        >
                    原始 API 响应 (JSON)        
        </summary>
               
        <pre
          class="results-code bg-gray-900 text-gray-200 p-6 rounded-lg mt-3 text-sm overflow-x-auto shadow-inner"
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
