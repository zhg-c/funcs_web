<script setup lang="ts">
import { defineProps } from "vue";
import { RouterLink } from "vue-router";

// 定义接收的 props 的类型
interface Feature {
  name: string;
  path: string;
  icon: string;
  description: string;
  // component: any; // 不需要在卡片中用到 component
}

// 接收 feature 对象作为 prop
const props = defineProps<{
  feature: Feature;
}>();
</script>

<template>
  <RouterLink :to="props.feature.path" class="feature-card">
    <div class="card-icon">
      <span class="icon-placeholder">{{ props.feature.icon }}</span>
    </div>

    <div class="card-content">
      <h3 class="card-title">{{ props.feature.name }}</h3>
      <p class="card-description">{{ props.feature.description }}</p>
    </div>

    <span class="arrow-indicator">→</span>
  </RouterLink>
</template>

<style scoped>
.feature-card {
  display: flex;
  align-items: center;
  padding: 20px;
  background-color: #fff;
  border-radius: 12px;
  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.05);
  transition: all 0.3s ease;
  text-decoration: none; /* 移除 RouterLink 的下划线 */
  color: #333; /* 默认文字颜色 */
  border: 1px solid #eee;
}

.feature-card:hover {
  transform: translateY(-5px); /* 悬浮时上移 */
  box-shadow: 0 8px 20px rgba(0, 0, 0, 0.1);
  border-color: #42b883; /* Vue 绿 */
}

.card-icon {
  flex-shrink: 0;
  width: 50px;
  height: 50px;
  background-color: #f0fdf4; /* 浅绿背景 */
  border-radius: 8px;
  display: flex;
  justify-content: center;
  align-items: center;
  margin-right: 15px;
}

.icon-placeholder {
  font-size: 1.5em;
  color: #42b883; /* Vue 绿 */
}

.card-title {
  margin: 0;
  font-size: 1.2em;
  font-weight: 600;
  color: #2c3e50;
}

.card-description {
  margin: 5px 0 0;
  font-size: 0.9em;
  color: #607d8b;
  line-height: 1.4;
}

.arrow-indicator {
  margin-left: auto;
  font-size: 1.5em;
  color: #42b883;
  transition: transform 0.3s;
}

.feature-card:hover .arrow-indicator {
  transform: translateX(5px);
}
</style>
