<template>
  <v-list>
    <div v-for="(item, index) in $store.state.downloaded_files" :key="index">
      <v-list-item>
        <v-checkbox></v-checkbox>
        <v-list-item-content>
          <v-row dense>
            <v-col>{{ item.name }}.{{ item.format }}</v-col>
            <v-col>{{ item.size }} bytes</v-col>
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
    </div>
  </v-list>
</template>
<script>
export default {
  name: "FilesListing"
};
</script>
