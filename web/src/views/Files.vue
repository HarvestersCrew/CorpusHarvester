<template>
  <Bar>
    <Builder builder_type="db"></Builder>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import Builder from "@/components/Builder.vue";
export default {
  name: "Files",
  components: { Bar, Builder }
};
</script>
