<script setup lang="ts">
import AppLayout from "./components/layout/AppLayout.vue";
import Hello from "./components/layout/Hello.vue";
</script>

<template>
  <AppLayout>
    <template #main-content>
      <Hello />
    </template>
  </AppLayout>
</template>
