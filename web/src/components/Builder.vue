<template>
  <div>
    <SelectCard>
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
      <v-divider vertical dark></v-divider>
      <v-col cols="auto">
        <v-text-field
          v-model="specified_number"
          outlined
          dark
          hide-details
          dense
          label="Approximate number to retrieve"
          :disabled="global_disable"
          type="number"
          prepend-icon="mdi-numeric"
          :rules="[validate_number]"
        ></v-text-field>
      </v-col>
    </SelectCard>

    <v-form v-model="builder_validity">
      <BuilderRequests
        @remove_request="remove_request"
        :requests="requests"
        :disabled="global_disable"
      ></BuilderRequests>
    </v-form>

    <FilesListing :builder_type="builder_type"></FilesListing>

    <v-dialog v-model="corpus_export" max-width="600px" persistent>
      <v-card>
        <v-card-title>Export to corpus</v-card-title>
        <v-card-text>
          <v-form>
            <p class="subtitle-2 text-left">Add to existing corpus</p>
            <v-text-field
              type="number"
              outlined
              hide-details
              dense
              label="Corpus ID"
              clearable
              v-model="corpus_id"
              :disabled="
                (corpus_name !== '' && corpus_name !== null) || corpus_disable
              "
            ></v-text-field>
          </v-form>
          <v-divider class="my-6"></v-divider>
          <p class="subtitle-2 text-left">Add to new corpus</p>
          <v-text-field
            outlined
            hide-details
            dense
            label="New corpus name"
            clearable
            v-model="corpus_name"
            :disabled="
              (corpus_id !== '' && corpus_id !== null) || corpus_disable
            "
          ></v-text-field>
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn text @click="corpus_export = false" :disabled="corpus_disable"
            >Cancel</v-btn
          >
          <v-btn
            text
            color="blue"
            :disabled="!corpus_name && !corpus_id"
            :loading="corpus_disable"
            @click="export_to_corpus"
            >Export</v-btn
          >
        </v-card-actions>
      </v-card>
    </v-dialog>

    <v-speed-dial open-on-hover fixed bottom right>
      <template v-slot:activator>
        <v-btn color="blue" dark fab x-large>
          <v-icon v-if="!global_disable">mdi-dots-vertical</v-icon>
          <v-progress-circular v-else indeterminate></v-progress-circular>
        </v-btn>
      </template>

      <v-tooltip left color="green" dark>
        <template v-slot:activator="{ on }">
          <v-btn
            fab
            color="green"
            :dark="builder_validity && requests.length !== 0 && !global_disable"
            :disabled="
              !builder_validity || requests.length === 0 || global_disable
            "
            small
            @click="send_query"
            v-on="on"
          >
            <v-icon>mdi-download</v-icon>
          </v-btn>
        </template>
        <span>Start download</span>
      </v-tooltip>

      <v-tooltip left color="indigo" dark>
        <template v-slot:activator="{ on }">
          <v-btn
            fab
            color="indigo"
            :disabled="
              $store.state.builders.files[builder_type].length === 0 ||
                global_disable
            "
            :dark="
              $store.state.builders.files[builder_type].length !== 0 &&
                !global_disable
            "
            small
            @click="corpus_export = true"
            v-on="on"
          >
            <v-icon>mdi-folder-move</v-icon>
          </v-btn>
        </template>
        <span>Export to corpus</span>
      </v-tooltip>

      <v-tooltip left color="red" dark>
        <template v-slot:activator="{ on }">
          <v-btn
            fab
            color="red"
            :disabled="
              $store.state.builders.files[builder_type].length === 0 ||
                global_disable
            "
            :dark="
              $store.state.builders.files[builder_type].length !== 0 &&
                !global_disable
            "
            small
            @click="clear_response"
            v-on="on"
          >
            <v-icon>mdi-notification-clear-all</v-icon>
          </v-btn>
        </template>
        <span>Clear display</span>
      </v-tooltip>
    </v-speed-dial>
  </div>
</template>

<script>
import BuilderRequests from "@/components/BuilderRequests.vue";
import FilesListing from "@/components/FilesListing.vue";
import SelectCard from "@/components/SelectCard.vue";
export default {
  name: "Builder",
  props: { builder_type: { required: true, type: String } },
  components: { BuilderRequests, FilesListing, SelectCard },
  data() {
    return {
      api_list_selection: undefined,
      requests: this.$store.state.builders.requests[this.builder_type],
      builder_validity: true,
      global_disable: this.$store.state.builders.disabled[this.builder_type],
      specified_number: this.$store.state.builders.number[this.builder_type],

      corpus_export: false,
      corpus_id: null,
      corpus_name: null,
      corpus_disable: false
    };
  },
  beforeDestroy() {
    this.$store.state.builders.requests[this.builder_type] = this.requests;
    this.$store.state.builders.disabled[
      this.builder_type
    ] = this.global_disable;
    this.$store.state.builders.number[
      this.builder_type
    ] = this.specified_number;
  },
  computed: {
    notif_sent_message() {
      return this.builder_type === "web"
        ? "Download request sent"
        : "Database request sent";
    },
    server_method() {
      return this.builder_type === "web" ? "download_query" : "";
    },
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
      let param_type = this.builder_type === "web" ? "requests" : "responses";
      api[param_type].forEach(req => {
        if (req.is_public !== undefined && req.is_public === true) {
          params[req.name] = req;
          values[req.name] = undefined;
        }
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
      this.$store.commit("add_notification", this.notif_sent_message);
      let data = { builder: this.server_query };

      if (this.specified_number !== "" && this.specified_number !== undefined) {
        data.number = parseInt(this.specified_number);
      }

      this.$store.dispatch("send_tokenized_request", {
        type: this.server_method,
        data,
        callback: this.query_response
      });
    },

    query_response(data) {
      this.$store.state.builders.disabled[this.builder_type] = false;
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
      this.$store.state.builders.files[this.builder_type] = [];
    },

    validate_number(input) {
      if (!input) return true;
      if (parseInt(input) < 0) return false;
      return true;
    },

    export_to_corpus() {
      this.corpus_disable = true;
      let obj = { type: this.builder_type };

      if (this.corpus_id !== null && this.corpus_id !== "") {
        obj.create = false;
        obj.id = parseInt(this.corpus_id);
      } else if (this.corpus_name !== null && this.corpus_name !== "") {
        obj.create = true;
        obj.title = this.corpus_name;
      } else {
        return;
      }

      this.$store.dispatch("send_tokenized_request", {
        type: "add_build_to_corpus",
        data: obj,
        callback: this.exported_to_corpus
      });
    },

    exported_to_corpus(data) {
      this.corpus_disable = false;
      if (data.type !== undefined && data.type === "error") {
        let msg;
        if (data.data === "db_id_not_found") msg = "Corpus ID not found";
        else msg = "An unexpected error occurred";
        this.$store.commit("add_error_notification", msg);
      } else {
        this.$store.commit(
          "add_success_notification",
          "Successfully added files to corpus"
        );
        this.corpus_export = false;
      }
    }
  }
};
</script>
