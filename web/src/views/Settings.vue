<template>
  <Bar>
    <v-container>
      <v-card class="my-4">
        <v-card-title>Logger settings</v-card-title>
        <v-card-text>
          <v-form @submit.prevent="on_output_path_send">
            <v-row>
              <v-col cols="10" md="5" class="mx-auto">
                <p class="subtitle-2 text-left">
                  Logger level
                </p>
                <p class="caption text-left">
                  Will display all logs at selected level and below (none
                  disables them)
                </p>
                <v-radio-group
                  color="blue"
                  class="ml-4"
                  :value="vuex_logger_level"
                  dense
                  :disabled="vuex_logger_level === undefined || level_disabled"
                  @change="on_level_change"
                >
                  <v-radio
                    v-for="level in logger_levels"
                    :key="level.val"
                    :label="level.name"
                    :value="level.val"
                  ></v-radio>
                </v-radio-group>
              </v-col>

              <v-col cols="10" md="5" class="mx-auto">
                <p class="subtitle-2 text-left">
                  Logger output
                </p>
                <p class="caption text-left">
                  Where to output the logs on the server side (they still are
                  displayed in the
                  <router-link :to="{ name: 'Logs' }">logs tab</router-link>)
                </p>
                <v-radio-group
                  color="blue"
                  class="ml-4"
                  :value="vuex_logger_output"
                  dense
                  :disabled="vuex_logger_output == undefined || output_disabled"
                  @change="on_output_change"
                >
                  <v-radio
                    v-for="output in logger_outputs"
                    :key="output.val"
                    :label="output.name"
                    :value="output.val"
                  ></v-radio>
                </v-radio-group>
                <template v-if="vuex_logger_output == 'file'">
                  <p class="subtitle-2 text-left">
                    Logger output path
                  </p>
                  <v-text-field
                    dense
                    hint="Path to the log file"
                    outlined
                    :placeholder="vuex_logger_output_path"
                    v-model="specified_output_path"
                    :disabled="output_path_disabled"
                    :error-messages="output_path_error"
                  >
                    <template v-slot:append-outer>
                      <v-icon
                        @click="on_output_path_send"
                        color="blue"
                        :disabled="
                          specified_output_path.length === 0 ||
                            output_path_disabled
                        "
                        >mdi-send</v-icon
                      >
                    </template>
                  </v-text-field>
                  <v-row justify="end">
                    <v-col cols="auto">
                      <v-btn
                        color="red"
                        dark
                        small
                        :loading="clear_logfile_disabled"
                        @click="on_clear_logfile"
                        >Clear logfile</v-btn
                      >
                    </v-col>
                  </v-row>
                </template>
              </v-col>
            </v-row>
          </v-form>
        </v-card-text>
      </v-card>

      <v-card class="my-4">
        <v-card-title>API settings</v-card-title>
        <v-card-text>
          <v-row>
            <v-col cols="10" md="5" class="mx-auto mx-md-10">
              <p class="subtitle-2 text-left">
                Refresh the APIs in memory from the disk
              </p>
              <p class="text-left">
                <v-btn
                  @click="on_refresh_apis"
                  :disabled="refresh_apis_disabled"
                  color="warning"
                  >Refresh</v-btn
                >
              </p>
            </v-col>
          </v-row>
        </v-card-text>
      </v-card>

      <v-card class="my-4">
        <v-card-title>Storage settings</v-card-title>
        <v-card-text>
          <v-row>
            <v-col cols="10" md="5" class="mx-auto mx-md-10">
              <p class="subtitle-2 text-left">
                Current storage path
              </p>
              <v-form @submit.prevent="storage_path_confirmation = true">
                <v-text-field
                  dense
                  hint="Where the harvester stores its data"
                  outlined
                  :placeholder="vuex_storage_path"
                  v-model="specified_storage_path"
                  :disabled="storage_path_disabled"
                  :error-messages="storage_path_error"
                >
                  <template v-slot:append-outer>
                    <v-icon
                      @click="storage_path_confirmation = true"
                      color="blue"
                      :disabled="
                        specified_storage_path.length === 0 ||
                          storage_path_disabled
                      "
                      >mdi-send</v-icon
                    >
                  </template>
                </v-text-field>
              </v-form>
            </v-col>
          </v-row>
        </v-card-text>
      </v-card>
    </v-container>
    <v-dialog v-model="storage_path_confirmation" max-width="500" persistent>
      <v-card>
        <v-card-title>Storage migration confirmation</v-card-title>
        <v-card-text class="text-left">
          You are going to migrate the database from
          <span v-text="vuex_storage_path" class="font-weight-bold"></span> to
          <span v-text="specified_storage_path" class="font-weight-bold"></span
          >. Are you sure?
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn
            text
            @click="storage_path_confirmation = false"
            :disabled="storage_path_disabled"
            >Cancel</v-btn
          >
          <v-btn
            text
            color="red"
            :loading="storage_path_disabled"
            @click="on_storage_path_change"
            >Proceed</v-btn
          >
        </v-card-actions>
      </v-card>
    </v-dialog>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import { mapState } from "vuex";
export default {
  name: "Settings",
  components: { Bar },
  computed: mapState({
    vuex_logger_level: state => state.logger_settings.level,
    vuex_logger_output: state => state.logger_settings.output,
    vuex_logger_output_path: state => state.logger_settings.output_path,
    vuex_storage_path: state => state.storage.path
  }),
  data() {
    return {
      storage_path_confirmation: false,
      specified_output_path: "",
      specified_storage_path: "",
      level_disabled: false,
      output_disabled: false,
      output_path_disabled: false,
      storage_path_disabled: false,
      refresh_apis_disabled: false,
      output_path_error: undefined,
      storage_path_error: undefined,
      clear_logfile_disabled: false,
      logger_levels: [
        { name: "Debug", val: "debug" },
        { name: "Info", val: "info" },
        { name: "Warning", val: "warning" },
        { name: "Error", val: "error" },
        { name: "None", val: "none" }
      ],
      logger_outputs: [
        { name: "Standard output", val: "stdout" },
        { name: "File", val: "file" }
      ]
    };
  },
  methods: {
    on_level_change(new_val) {
      this.level_disabled = true;
      this.$store.dispatch("send_tokenized_request", {
        type: "update_logger",
        data: { setting: "level", value: new_val },
        callback: this.on_level_changed
      });
    },
    on_level_changed() {
      this.level_disabled = false;
      this.$store.commit("add_success_notification", "Logger level changed");
    },
    on_output_change(new_val) {
      this.output_disabled = true;
      this.$store.dispatch("send_tokenized_request", {
        type: "update_logger",
        data: { setting: "output", value: new_val },
        callback: this.on_output_changed
      });
    },
    on_output_changed() {
      this.output_disabled = false;
      this.$store.commit("add_success_notification", "Logger output changed");
    },
    on_output_path_send() {
      this.output_path_disabled = true;
      this.output_path_error = undefined;
      this.$store.dispatch("send_tokenized_request", {
        type: "update_logger",
        data: { setting: "output_path", value: this.specified_output_path },
        callback: this.on_output_path_changed
      });
    },
    on_output_path_changed(data) {
      this.output_path_disabled = false;
      if (data.type !== undefined && data.type === "error") {
        this.output_path_error =
          "Invalid path. It must be an already created absolute path with the filename";
      } else {
        this.specified_output_path = "";
        this.$store.commit(
          "add_success_notification",
          "Logger output path changed"
        );
      }
    },
    on_storage_path_change() {
      this.storage_path_disabled = true;
      this.storage_path_error = undefined;
      this.$store.dispatch("send_tokenized_request", {
        type: "storage_migration",
        data: { new_path: this.specified_storage_path },
        callback: this.on_storage_path_changed
      });
    },
    on_storage_path_changed(data) {
      this.storage_path_disabled = false;
      this.storage_path_confirmation = false;
      if (data.type !== undefined && data.type === "error") {
        this.storage_path_error =
          "Invalid path. There must not be a trailing slash, the path must be absolute and parent path must exist but destination directory must not";
      } else {
        this.specified_storage_path = "";
        this.$store.commit(
          "add_success_notification",
          "Storage migration done"
        );
      }
    },
    on_clear_logfile() {
      this.clear_logfile_disabled = true;
      this.$store.dispatch("send_tokenized_request", {
        type: "clear_logfile",
        data: {},
        callback: this.on_cleared_logfile
      });
    },
    on_cleared_logfile(data) {
      this.clear_logfile_disabled = false;
      if (data.type !== undefined && data.type === "error") {
        this.$store.commit(
          "add_error_notification",
          "An error occurred clearing the logs, check the logs tab to see more details"
        );
      } else {
        this.$store.commit("add_success_notification", "Logfile cleared");
      }
    },
    on_refresh_apis() {
      this.refresh_apis_disabled = true;
      this.$store.dispatch("send_tokenized_request", {
        type: "refresh_apis",
        data: {},
        callback: this.on_refreshed_apis
      });
    },
    on_refreshed_apis() {
      this.refresh_apis_disabled = false;
    }
  }
};
</script>
