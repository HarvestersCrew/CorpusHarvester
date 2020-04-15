<template>
  <Bar>
    <v-container>
      <v-card>
        <v-card-title>Logger settings</v-card-title>
        <v-card-text>
          <v-form>
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
                  :disabled="vuex_logger_level === undefined || level_updating"
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
                  :disabled="vuex_logger_output == undefined || level_updating"
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
                  ></v-text-field>
                </template>
              </v-col>
            </v-row>
          </v-form>
        </v-card-text>
      </v-card>
    </v-container>
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
    vuex_logger_output_path: state => state.logger_settings.output_path
  }),
  data() {
    return {
      level_updating: false,
      output_updating: false,
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
      this.level_updating = true;
      this.$store.dispatch("send_tokenized_request", {
        type: "update_logger",
        data: { setting: "level", value: new_val },
        callback: this.on_level_changed
      });
    },
    on_level_changed() {
      this.level_updating = false;
    },
    on_output_change(new_val) {
      this.output_updating = true;
      this.$store.dispatch("send_tokenized_request", {
        type: "update_logger",
        data: { setting: "output", value: new_val },
        callback: this.on_output_changed
      });
    },
    on_output_changed() {
      this.output_updating = false;
    }
  }
};
</script>
