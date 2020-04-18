<template>
  <v-expansion-panels hover multiple tile accordion>
    <v-expansion-panel v-for="(request, idx) in requests" :key="idx">
      <v-expansion-panel-header>{{ request.api }}</v-expansion-panel-header>
      <v-expansion-panel-content>
        <v-container> </v-container>
      </v-expansion-panel-content>
    </v-expansion-panel>
  </v-expansion-panels>
</template>

<script>
export default {
  name: "BuilderRequests",
  props: {
    requests: {
      type: Array,
      required: true
    }
  }
};
</script>
