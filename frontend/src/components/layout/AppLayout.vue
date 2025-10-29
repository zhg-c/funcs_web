<template>
  <div class="app-container">
    <TheHeader />
    <main class="main-content">
      <div class="core-area">
        <RouterView />
      </div>
      <aside class="sidebar-area">
        <AdSidebar />
      </aside>
    </main>
    <TheFooter />
  </div>
</template>

<script setup lang="ts">
import TheHeader from "./TheHeader.vue";
import TheFooter from "./TheFooter.vue";
import AdSidebar from "./AdSidebar.vue";
import { RouterView } from "vue-router";
// 无需额外的 TS 逻辑
</script>

<style scoped>
.app-container {
  display: flex;
  flex-direction: column;
  min-height: 100vh;
  background-color: #f0f2f5;
}
.main-content {
  flex-grow: 1; /* 占据剩余垂直空间 */
  display: flex;
  max-width: 1200px;
  width: 90%;
  margin: 20px auto;
  gap: 20px;
}
.core-area {
  flex: 3; /* 核心内容占据 75% 宽度 */
  background-color: white;
  padding: 30px;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}
.sidebar-area {
  flex: 1; /* 侧边栏占据 25% 宽度 */
}
</style>
