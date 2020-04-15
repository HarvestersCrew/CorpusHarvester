<template>
  <Bar>
    <v-system-bar window dark fixed id="system-bar">
      <v-icon>mdi-alert</v-icon>
      <span>Logs</span>
    </v-system-bar>
    <v-container fluid id="logs">
      <v-col cols="12">
        <v-list color="#39404a" dense>
          <v-list-item
            v-for="(log, index) in $store.state.logs.messages.slice().reverse()"
            :key="index"
          >
            <v-list-item-content class="grey--text text--lighten-4 text-left">
              [{{ $store.state.logs.messages.length - index }}] > {{ log }}
            </v-list-item-content>
          </v-list-item>
        </v-list>
      </v-col>
    </v-container>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
export default {
  name: "Logs",
  components: { Bar },
  mounted() {
    this.$store.commit("clear_unread_logs");
  }
};
</script>

<style lang="scss">
#system-bar {
  left: 56px;
  width: calc(100% - 56px);
  height: 32px;
}

#logs {
  margin-top: 32px;
  height: calc(100vh - 32px);
  overflow-y: auto;
  background-color: #39404a;
  font-family: "SF Mono Powerline", "Courier New", monospace;
}
</style>
