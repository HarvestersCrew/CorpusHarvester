<template>
  <v-list>
    <div v-for="(item, index) in files" :key="index">
      <v-list-item>
        <v-row dense>
          <v-col>{{ item.name }}.{{ item.format }}</v-col>
          <v-col>{{ item_size(item.size) }}</v-col>
          <v-col>{{ item.source }} ({{ api_type(item.source) }})</v-col>
        </v-row>
      </v-list-item>
      <v-divider v-if="index + 1 < files.length"></v-divider>
    </div>
  </v-list>
</template>
<script>
import { bytes_conversion } from "@/mixins/bytes_conversion.js";
export default {
  name: "FilesListing",
  props: { files: { required: true, type: Array } },
  mixins: [bytes_conversion],
  methods: {
    api_type(api_name) {
      return this.$store.getters.api_by_name(api_name).api_type;
    },
    item_size(size) {
      return this.bytes_conversion_string(size);
    }
  }
};
</script>
