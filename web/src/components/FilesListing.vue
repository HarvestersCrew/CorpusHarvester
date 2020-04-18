<template>
  <div>
    <v-list v-if="$store.state.downloaded_files.length > 0">
      <div v-for="(item, index) in $store.state.downloaded_files" :key="index">
        <v-list-item>
          <v-row dense>
            <v-col>{{ item.name }}.{{ item.format }}</v-col>
            <v-col>{{ item.size }} bytes</v-col>
            <v-col>{{ item.source }} ({{ api_type(item.source) }})</v-col>
          </v-row>
        </v-list-item>
        <v-divider
          v-if="index + 1 < $store.state.downloaded_files.length"
        ></v-divider>
      </div>
    </v-list>

    <v-container v-else>
      <v-row justify="center">
        <v-col cols="10" md="6">
          <v-card>
            <v-card-title>How to download</v-card-title>
            <v-card-text class="text-left">
              Start by adding requests and filling needed parameters. Once you
              validate it, the download will start and you'll just have to wait.
            </v-card-text>
          </v-card>
        </v-col>
      </v-row>
    </v-container>
  </div>
</template>
<script>
export default {
  name: "FilesListing",
  methods: {
    api_type(api_name) {
      return this.$store.getters.api_by_name(api_name).api_type;
    }
  }
};
</script>
