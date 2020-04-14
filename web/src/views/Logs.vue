<template>
  <Bar>
    <v-system-bar window dark fixed id="system-bar">
      <v-icon>mdi-alert</v-icon>
      <span>Logs</span>
      <v-spacer></v-spacer>
      <v-icon>mdi-minus</v-icon>
      <v-icon>mdi-checkbox-blank-outline</v-icon>
      <v-icon>mdi-close</v-icon>
    </v-system-bar>
    <div id="logs">
      <v-list-item v-for="(log, index) in $store.state.logs" :key="index">
        <span>[{{ index }}] > {{ log }}</span>
      </v-list-item>
    </div>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
export default {
  name: "Logs",
  components: { Bar }
};
</script>

<style lang="scss">
#system-bar {
  left: 56px;
  width: calc(100% - 56px);
  height: 32px;
}

#logs {
  overflow-y: auto;
  position: fixed;
  background-color: #39404a;
  font-family: "SF Mono Powerline", "Courier New", monospace;
  height: calc(100% - 32px);
  width: 100%;
  margin-top: 32px;
  span {
    color: #dcdcdc;
  }
}
</style>
