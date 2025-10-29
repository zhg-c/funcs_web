<template>
  <div class="not-found-container">
    <h1>404</h1>
    <h2>页面未找到</h2>
    <p>抱歉，您访问的页面不存在。</p>
    <RouterLink to="/" class="home-link">返回主页</RouterLink>
  </div>
</template>

<style scoped>
.not-found-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  height: 60vh;
  text-align: center;
}

h1 {
  font-size: 6em;
  color: #f44336; /* 红色，代表错误 */
  margin: 0;
}

h2 {
  font-size: 2em;
  color: #333;
  margin-bottom: 20px;
}

.home-link {
  display: inline-block;
  padding: 10px 20px;
  background-color: #42b883;
  color: white;
  text-decoration: none;
  border-radius: 5px;
  margin-top: 20px;
  transition: background-color 0.3s;
}

.home-link:hover {
  background-color: #36a374;
}
</style>
