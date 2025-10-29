import { createRouter, createWebHistory } from "vue-router";
import HomeView from "../views/HomeView.vue";
import AppLayout from "../components/layout/AppLayout.vue";
import NotFoundView from "../views/NotFoundView.vue";
import { FEATURE_CATEGORIES } from "./features";

// 动态生成所有功能页的路由
const featureRoutes = FEATURE_CATEGORIES.flatMap((category) =>
  category.features.map((feature) => ({
    path: feature.path,
    name: feature.name.replace(/\s/g, ""),
    // 使用动态导入，以优化打包体积
    component: feature.component,
  }))
);

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: "/",
      component: AppLayout, // 使用您的主布局
      children: [
        {
          path: "",
          name: "Home",
          component: HomeView, // 主页
        },
        // 动态添加所有功能路由
        ...featureRoutes,
      ],
    },
    // 404 页面
    {
      path: "/:pathMatch(.*)*",
      name: "NotFound",
      component: NotFoundView,
    },
  ],
});

export default router;
