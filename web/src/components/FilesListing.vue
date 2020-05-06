<template>
  <!-- <v-list>
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
  </v-list> -->
  <v-expansion-panels accordion hover :flat="flat" class="mt-4">
    <v-expansion-panel v-for="(item, index) in files" :key="index">
      <v-expansion-panel-header>
        <v-row dense>
          <v-col cols="auto" align-self="center">
            {{ item.name }}.{{ item.format }}
          </v-col>
          <v-col class="font-weight-light" align-self="center"
            >#{{ item.id }}</v-col
          >
        </v-row>
      </v-expansion-panel-header>
      <v-expansion-panel-content>
        <v-row>
          <v-col
            cols="6"
            sm="4"
            md="3"
            lg="2"
            xl="1"
            v-for="(tag, key) in item.tags"
            :key="tag"
          >
            <div class="font-weight-bold">{{ key }}</div>
            <div>{{ tag }}</div>
          </v-col>
        </v-row>
      </v-expansion-panel-content>
    </v-expansion-panel>
  </v-expansion-panels>
</template>
<script>
import { bytes_conversion } from "@/mixins/bytes_conversion.js";
export default {
  name: "FilesListing",
  props: {
    files: { required: true, type: Array },
    flat: { type: Boolean, default: false }
  },
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
