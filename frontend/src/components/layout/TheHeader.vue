<template>
  <header class="header">
    <div class="logo">C++ 加速服务平台</div>
    <nav>
      <a href="/">首页</a>
    </nav>
  </header>
</template>

<script setup lang="ts">
// 无需 TS 逻辑
</script>

<style scoped>
.header {
  background-color: #007bff;
  color: white;
  padding: 15px 50px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}
.logo {
  font-size: 1.5em;
  font-weight: bold;
}
nav a {
  color: white;
  text-decoration: none;
  margin-left: 20px;
  transition: opacity 0.3s;
}
nav a:hover {
  opacity: 0.8;
}
</style>
