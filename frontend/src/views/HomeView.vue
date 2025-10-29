<script setup lang="ts">
// 1. 导入功能数据
import { FEATURE_CATEGORIES } from "@/router/features";
// 2. 导入功能卡片组件
import FeatureCard from "@/components/common/FeatureCard.vue";
</script>

<template>
  <div class="home-view">
    <header class="page-header">
      <h1>🚀 Kali实用工具箱</h1>
      <p class="subtitle">
        集成 C++ 高性能核心，提供实用、快速的网络安全与开发辅助工具。
      </p>
    </header>

    <div
      v-for="category in FEATURE_CATEGORIES"
      :key="category.id"
      class="category-section"
    >
      <h2 class="category-title">{{ category.name }}</h2>
      <p class="category-description">{{ category.description }}</p>

      <div class="feature-grid">
        <FeatureCard
          v-for="feature in category.features"
          :key="feature.path"
          :feature="feature"
        />
      </div>
    </div>
  </div>
</template>

<style scoped>
.home-view {
  /* 确保 HomeView 的内容在 AppLayout 的 core-area 内部有良好展现 */
  padding: 0 10px;
}

.page-header {
  margin-bottom: 40px;
  padding-bottom: 20px;
  border-bottom: 1px solid #e0e0e0;
}

h1 {
  font-size: 2.5em;
  color: #2c3e50;
  margin-bottom: 10px;
}

.subtitle {
  font-size: 1.1em;
  color: #6a737d;
  line-height: 1.5;
}

.category-section {
  margin-bottom: 50px;
  padding-top: 30px;
}

.category-title {
  font-size: 1.8em;
  color: #333;
  /* 使用一个视觉元素来突出分类 */
  border-left: 4px solid #42b883;
  padding-left: 15px;
  margin-bottom: 15px;
}
.category-description {
  color: #586069;
  font-size: 1em;
  margin-bottom: 25px;
}
.feature-grid {
  /* 使用 Grid 布局自适应屏幕宽度 */
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
  gap: 20px;
}
</style>
