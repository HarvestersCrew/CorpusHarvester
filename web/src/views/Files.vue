<template>
  <Bar>
    <v-card flat tile>
      <v-toolbar color="blue lighten-2">
        <v-row>
          <v-col cols="4">
            <v-select
              v-model="filters.selectedTypes"
              :items="filters.types"
              label="Types"
              dark
              outlined
              multiple
              hide-details
              dense
            >
            </v-select>
          </v-col>
          <v-col cols="4">
            <v-select
              v-model="filters.selectedSources"
              :items="filters.sources"
              label="Sources"
              dark
              outlined
              multiple
              hide-details
              dense
            >
            </v-select>
          </v-col>
          <v-col cols="4">
            <v-menu :close-on-content-click="false">
              <template v-slot:activator="{ on }">
                <v-text-field
                  v-model="filters.date"
                  label="Date"
                  v-on="on"
                  dark
                  outlined
                  dense
                  hide-details
                >
                </v-text-field>
              </template>
              <v-date-picker v-model="filters.date"></v-date-picker>
            </v-menu>
          </v-col>
        </v-row>
      </v-toolbar>
      <v-card-text class="ma-0 pa-0">
        <v-expansion-panels tile accordion="false dark">
          <v-expansion-panel
            v-for="(item, i) in filters.selectedSources"
            :key="i"
          >
            <v-expansion-panel-header>{{ item }}</v-expansion-panel-header>
            <v-expansion-panel-content
              >{{ item }} parameters</v-expansion-panel-content
            >
          </v-expansion-panel>
        </v-expansion-panels>
      </v-card-text>
    </v-card>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
export default {
  name: "Files",
  components: { Bar },
  data() {
    return {
      files: undefined,
      filters: {
        date: undefined,
        sources: ["Twitter", "TMDB", "WikiCommons"],
        types: ["Text", "Image"],
        selectedSources: [],
        selectedTypes: []
      }
    };
  }
};
</script>
