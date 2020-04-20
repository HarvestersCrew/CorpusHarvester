<template>
  <div>
    <v-list v-if="$store.state.builders.files[builder_type].length > 0">
      <div
        v-for="(item, index) in $store.state.builders.files[builder_type]"
        :key="index"
      >
        <v-list-item>
          <v-row dense>
            <v-col>{{ item.name }}.{{ item.format }}</v-col>
            <v-col>{{ item.size }} bytes</v-col>
            <v-col>{{ item.source }} ({{ api_type(item.source) }})</v-col>
          </v-row>
        </v-list-item>
        <v-divider
          v-if="index + 1 < $store.state.builders.files[builder_type].length"
        ></v-divider>
      </div>
    </v-list>

    <v-container v-else>
      <v-row justify="center">
        <v-col cols="10" md="6">
          <v-card>
            <v-card-title>{{ empty_title }}</v-card-title>
            <v-card-text class="text-left">{{ empty_text }}</v-card-text>
          </v-card>
        </v-col>
      </v-row>
    </v-container>
  </div>
</template>
<script>
export default {
  name: "FilesListing",
  props: { builder_type: { required: true, type: String } },
  computed: {
    empty_title() {
      return this.builder_type === "web"
        ? "How to download"
        : "How to search in the database";
    },
    empty_text() {
      return this.builder_type === "web"
        ? "Start by adding requests and filling needed parameters. Once you validate it, the download will start and you'll just have to wait."
        : "Start by adding requests and filling parameters. Once you validate it, they will be executed against the database.";
    }
  },
  methods: {
    api_type(api_name) {
      return this.$store.getters.api_by_name(api_name).api_type;
    }
  }
};
</script>
