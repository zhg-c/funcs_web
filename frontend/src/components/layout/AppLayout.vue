<script setup lang="ts">
import AdSidebar from "./AdSidebar.vue";
import TheHeader from "./TheHeader.vue";
import TherFooter from "./TherFooter.vue";
</script>

<template>
  <div class="app-container">
    <TheHeader />
    <main class="main-content">
      <div class="core-area">
        <slot name="main-content"></slot>
      </div>
      <aside class="sidebar-area">
        <AdSidebar />
      </aside>
    </main>
    <TherFooter />
  </div>
</template>
