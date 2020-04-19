<template>
  <Bar>
    <Builder builder_type="web"></Builder>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import Builder from "@/components/Builder.vue";
export default {
  name: "Download",
  components: { Bar, Builder }
};
</script>
