<template>
  <Bar>
    <v-card flat tile>
      <v-toolbar color="blue lighten-2">
        <v-container fluid>
          <v-row>
            <v-col cols="auto">
              <v-autocomplete
                :items="$store.state.apis"
                v-model="api_list_selection"
                item-text="name"
                item-value="name"
                outlined
                dark
                hide-details
                dense
                label="Add a source..."
                prepend-icon="mdi-api"
                append-outer-icon="mdi-plus"
                @click:append-outer="add_source_to_requests"
              >
              </v-autocomplete>
            </v-col>
          </v-row>
        </v-container>
      </v-toolbar>
    </v-card>
    <BuilderRequests :requests="requests"></BuilderRequests>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import BuilderRequests from "@/components/BuilderRequests.vue";
export default {
  name: "Download",
  components: { Bar, BuilderRequests },
  data() {
    return {
      api_list_selection: undefined,
      requests: []
    };
  },
  methods: {
    add_source_to_requests() {
      let params = {};
      let api = this.$store.getters.api_by_name(this.api_list_selection);
      api.requests.forEach(req => {
        params[req.name] = undefined;
      });
      this.requests.unshift({
        api: this.api_list_selection,
        params,
        type: "web"
      });
      this.api_list_selection = undefined;
    }
  }
};
</script>
