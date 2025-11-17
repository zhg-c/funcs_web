<script setup lang="ts">
import { ref } from "vue";

const fieldMap: Record<string, string> = {
  domain: "域名",
  registryDomainID: "注册域名ID",
  registrar: "注册商",
  registrarWhoisServer: "注册商 WHOIS 服务器",
  registrarURL: "注册商网站",
  creationDate: "创建日期",
  updatedDate: "更新日期",
  expiryDate: "到期日期",
  dnssec: "DNSSEC 状态",
  statuses: "域名状态",
  nameServers: "名称服务器",
};
const whois = ref<any | null>(null);
const target = ref("127.0.0.1"); // 目标IP/域名
const rawResults = ref<string>();
const isLoading = ref(false);
const error = ref<string | null>(null);

// 🚨 API 基础 URL：根据您当前的运行环境选择
const API_BASE_URL = "http://192.168.10.106:8000/api/v1";

const lookupDoman = async () => {
  console.log("--- Scan button clicked! ---");

  console.log("Target:", target.value);
  isLoading.value = true;
  error.value = null;
  rawResults.value = "正在发送请求并查找中...";

  const payload = {
    target: target.value,
  };
  const makeApiCall = async (retryCount = 0): Promise<any> => {
    try {
      const response = await fetch(`${API_BASE_URL}/func/whois`, {
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

    rawResults.value = data;
    whois.value = data.results;

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
    <h2>域名信息查询</h2>
    <div class="scan-form">
      <form>
        <label for="target">目标IP/域名：</label>
        <input
          type="text"
          name="target"
          id="target"
          v-model="target"
          placeholder="比如：example.com"
        />
        <input
          type="submit"
          name="protocol"
          id="protocol"
          :value="isLoading ? '正在查询...' : '开始查询'"
          :disabled="isLoading"
          @click="lookupDoman"
        />
      </form>
    </div>
  </div>
  <div class="results">
    <br />
    <div class="err" v-if="error"><strong>错误:</strong> {{ error }}</div>
    <div class="params">
      <div v-if="whois">
        <h3>查询结果</h3>

        <div
          v-for="(value, key) in whois"
          :key="key"
          style="margin-bottom: 10px"
        >
          <template v-if="Array.isArray(value)">
            <!-- 处理列表字段 -->
            <strong>{{ fieldMap[key] || key }}：</strong>
            <ul>
              <li v-for="(item, index) in value" :key="index">
                {{ item }}
              </li>
            </ul>
          </template>

          <template v-else>
            <!-- 处理普通字段 -->
            <p>
              <strong>{{ fieldMap[key] || key }}：</strong>
              {{ value || "（无数据）" }}
            </p>
          </template>
        </div>
      </div>
    </div>
  </div>
</template>
<style scoped>
@import "@/assets/netTools.css";
</style>
