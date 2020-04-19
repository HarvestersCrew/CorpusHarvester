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
                :disabled="global_disable"
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
        :disabled="global_disable"
      ></BuilderRequests>
    </v-form>

    <FilesListing></FilesListing>

    <v-speed-dial open-on-hover fixed bottom right>
      <template v-slot:activator>
        <v-btn color="blue" dark fab>
          <v-icon v-if="!global_disable">mdi-dots-vertical</v-icon>
          <v-progress-circular v-else indeterminate></v-progress-circular>
        </v-btn>
      </template>

      <v-btn
        fab
        color="green"
        :dark="builder_validity && requests.length !== 0 && !global_disable"
        :disabled="!builder_validity || requests.length === 0 || global_disable"
        small
        @click="send_query"
      >
        <v-icon>mdi-download</v-icon>
      </v-btn>

      <v-btn
        fab
        color="red"
        :disabled="
          this.$store.state.downloaded_files.length === 0 || global_disable
        "
        :dark="
          this.$store.state.downloaded_files.length !== 0 && !global_disable
        "
        small
        @click="clear_response"
      >
        <v-icon>mdi-notification-clear-all</v-icon>
      </v-btn>
    </v-speed-dial>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import BuilderRequests from "@/components/BuilderRequests.vue";
import FilesListing from "@/components/FilesListing.vue";
export default {
  name: "Download",
  components: { Bar, BuilderRequests, FilesListing },
  data() {
    return {
      api_list_selection: undefined,
      requests: this.$store.state.api_db_builder,
      builder_validity: true,
      global_disable: this.$store.state.api_db_builder_disabled
    };
  },
  beforeDestroy() {
    this.$store.commit("set_api_db_builder", this.requests);
    this.$store.commit("set_api_db_builder_disabled", this.global_disable);
  },
  computed: {
    server_query() {
      let query = [];
      this.requests.forEach(req => {
        let req_parsed = {
          name: req.api,
          values: {}
        };
        Object.keys(req.values).forEach(key => {
          if (
            req.values[key] !== null &&
            req.values[key] !== undefined &&
            req.values[key] !== ""
          )
            req_parsed.values[key] = req.values[key];
        });
        query.push(req_parsed);
      });
      return query;
    }
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
    },
    send_query() {
      this.global_disable = true;
      this.$store.commit("add_notification", "Download request sent");
      this.$store.dispatch("send_tokenized_request", {
        type: "download_query",
        data: { builder: this.server_query },
        callback: this.query_response
      });
    },
    query_response(data) {
      this.$store.commit("set_api_db_builder_disabled", false);
      this.global_disable = false;
      if (data.type !== undefined && data.type === "error") {
        this.$store.commit(
          "add_error_notification",
          "An error occurred during the download, check the logs for further informations"
        );
      } else {
        this.$store.commit(
          "add_success_notification",
          "Downloaded " +
            data.data.files.length +
            " items, check them in the download tab"
        );
      }
    },
    clear_response() {
      this.$store.commit("clear_downloaded_files");
    }
  }
};
</script>
