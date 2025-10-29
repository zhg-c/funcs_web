<script setup lang="ts">
import AppLayout from "./components/layout/AppLayout.vue";
import HomeView from "./views/HomeView.vue";
</script>

<template>
  <AppLayout>
    <template #main-content> <HomeView /> </template>
  </AppLayout>
</template>
