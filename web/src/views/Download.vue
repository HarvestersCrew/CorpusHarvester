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

    <v-form v-model="builder_validity">
      <BuilderRequests
        @remove_request="remove_request"
        :requests="requests"
      ></BuilderRequests>
    </v-form>

    <v-btn
      :disabled="!builder_validity || requests.length === 0"
      fab
      large
      fixed
      bottom
      right
      color="blue"
      dark
    >
      <v-icon>mdi-download</v-icon>
    </v-btn>
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
      requests: this.$store.state.api_db_builder,
      builder_validity: true
    };
  },
  beforeDestroy() {
    this.$store.commit("set_api_db_builder", this.requests);
  },
  methods: {
    add_source_to_requests() {
      let api = this.$store.getters.api_by_name(this.api_list_selection);
      let params = {};
      let values = {};
      api.requests.forEach(req => {
        params[req.name] = req;
        values[req.name] = undefined;
      });
      this.requests.unshift({
        api: this.api_list_selection,
        params,
        values
      });
      this.api_list_selection = undefined;
    },
    remove_request(idx) {
      this.requests.splice(idx, 1);
    }
  }
};
</script>
