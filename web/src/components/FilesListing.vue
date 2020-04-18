<template>
  <v-list>
    <v-list-item
      v-for="(item, index) in $store.state.downloaded_files"
      :key="index"
    >
      <v-checkbox></v-checkbox>
      <v-list-item-content>
        <v-row dense>
          <v-col>{{ item.name }}</v-col>
          <v-col>{{ item.format }}</v-col>
          <v-col>{{ item.source }}</v-col>
        </v-row>
      </v-list-item-content>
      <v-list-item-icon>
        <v-btn icon><v-icon>mdi-dots-vertical</v-icon></v-btn>
      </v-list-item-icon>
    </v-list-item>
    <v-divider
      v-if="index + 1 < $store.state.downloaded_files.length"
    ></v-divider>
  </v-list>
</template>
<script>
export default {
  name: "FilesListing"
};
</script>
