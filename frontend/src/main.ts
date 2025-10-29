import "./assets/main.css";

import { createApp } from "vue";
import App from "./App.vue";
import router from "./router"; // ⬅️ 确保导入路由

const app = createApp(App);

app.use(router); // ⬅️ 注册 Vue Router

app.mount("#app");
