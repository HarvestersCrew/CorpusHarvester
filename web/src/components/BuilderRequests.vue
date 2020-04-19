<template>
  <v-expansion-panels hover multiple tile accordion>
    <v-expansion-panel v-for="(request, idx) in requests" :key="idx">
      <v-expansion-panel-header>{{ request.api }}</v-expansion-panel-header>
      <v-expansion-panel-content eager>
        <v-container>
          <v-row align="center">
            <v-col cols="3" v-for="(param, name) in request.params" :key="name">
              <ApiParamInput
                :param="param"
                :disabled="disabled"
                v-model="request.values[name]"
              ></ApiParamInput>
            </v-col>
            <v-col cols="3">
              <v-btn
                :disabled="disabled"
                color="red"
                text
                :dark="!disabled"
                @click="$emit('remove_request', idx)"
                >Remove</v-btn
              >
            </v-col>
          </v-row>
        </v-container>
      </v-expansion-panel-content>
    </v-expansion-panel>
  </v-expansion-panels>
</template>

<script>
import ApiParamInput from "@/components/ApiParamInput.vue";
export default {
  name: "BuilderRequests",
  components: { ApiParamInput },
  props: {
    requests: {
      type: Array,
      required: true
    },
    disabled: {
      type: Boolean,
      required: true
    }
  }
};
</script>
