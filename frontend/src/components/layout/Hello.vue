<template>
  <div>
    <h2>C++ 核心功能测试区</h2>
    <p>点击下方按钮，将通过 FastAPI 调用 C++ 核心库的 `func_test()`。</p>

    <button @click="fetchHello" :disabled="isLoading">
      {{ isLoading ? "正在请求..." : "点击获取 C++ Hello 消息" }}
    </button>

    <div v-if="result" :class="['result-box', result.status]">
      <p>
        <strong>状态:</strong>
        {{ result.status === "success" ? "成功" : "失败" }}
      </p>
      <p><strong>后端响应:</strong> {{ result.message }}</p>
      <p>
        <strong>C++ 返回:</strong>
        <span class="cpp-message">{{ result.hello_message }}</span>
      </p>
    </div>

    <div v-else-if="error" class="result-box error">
      <p><strong>请求错误:</strong> {{ error }}</p>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from "vue";

// 🚨 定义响应的数据结构类型
interface HelloResponse {
  status: "success" | "error";
  message: string;
  hello_message: string;
}

const isLoading = ref(false);
const result = ref<HelloResponse | null>(null);
const error = ref<string | null>(null);

// 🚨 确保与后端的 API 地址匹配
const apiUrl = "http://127.0.0.1:8000/api/get_hello/";

const fetchHello = async () => {
  isLoading.value = true;
  result.value = null;
  error.value = null;

  try {
    const response = await fetch(apiUrl);

    if (!response.ok) {
      // 如果 HTTP 状态码不是 2xx，FastAPI 返回的错误通常不是标准JSON
      const errorText = await response.text();
      throw new Error(
        `HTTP 状态码 ${response.status}: ${errorText.substring(0, 100)}...`
      );
    }

    const data: HelloResponse = await response.json();
    result.value = data;
  } catch (err) {
    if (err instanceof Error) {
      error.value = err.message;
    } else {
      error.value = "发生未知错误";
    }
  } finally {
    isLoading.value = false;
  }
};
</script>

<style scoped>
button {
  padding: 10px 20px;
  font-size: 1em;
  cursor: pointer;
  background-color: #007bff;
  color: white;
  border: none;
  border-radius: 5px;
  transition: background-color 0.3s;
  margin-top: 15px;
}
button:hover:not(:disabled) {
  background-color: #0056b3;
}
button:disabled {
  background-color: #ccc;
  cursor: not-allowed;
}
.result-box {
  margin-top: 25px;
  padding: 20px;
  border-radius: 6px;
  text-align: left;
}
.result-box.success {
  border: 1px solid #28a745;
  background-color: #d4edda;
  color: #155724;
}
.result-box.error {
  border: 1px solid #dc3545;
  background-color: #f8d7da;
  color: #721c24;
}
.cpp-message {
  font-weight: bold;
  font-size: 1.1em;
}
</style>
