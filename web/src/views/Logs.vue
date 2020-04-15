<template>
  <Bar>
    <v-container fluid id="logs">
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
#logs {
  overflow-y: auto;
  height: 100%;
  background-color: #39404a;
  font-family: "SF Mono Powerline", "Courier New", monospace;
}
</style>
