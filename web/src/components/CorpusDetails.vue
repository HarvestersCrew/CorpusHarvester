<template>
  <v-card>
    <v-toolbar dark color="blue">
      <slot></slot>
      <v-toolbar-title>{{ corpus.title }}</v-toolbar-title>
    </v-toolbar>
    <v-container>
      <v-row justify="center" class="my-4">
        <v-col cols="12" sm="8" md="7" lg="6" xl="4">
          <v-row>
            <v-col cols="12" class="pt-0">
              <v-card>
                <v-card-title>Description</v-card-title>
                <v-card-text class="px-4">
                  <b>ID</b> #{{ corpus.id }}
                  <br />
                  <b>Creation date :</b>
                  {{ corpus.creation_date }}
                  <br />
                  <b>Source(s) :</b>
                  <br />
                  <b>Type(s) :</b>
                  <br />
                </v-card-text>
              </v-card>
            </v-col>

            <v-col cols="12">
              <v-card>
                <v-card-title>Corpus size</v-card-title>
                <v-card-subtitle>
                  Sum of the size of all corpus files (the corpus itself does
                  not take up storage space)
                </v-card-subtitle>
                <v-card-text class="px-4">
                  <div class="display-2">
                    ~ TBD
                  </div>
                  ({{ corpus.files.length }} files in total)
                </v-card-text>
              </v-card>
            </v-col>

            <v-col cols="12">
              <v-card>
                <v-card-title>Distribution</v-card-title>
                <v-card-text>
                  <DonutChart></DonutChart>
                  File types proportions in this corpus
                </v-card-text>
              </v-card>
            </v-col>
          </v-row>
        </v-col>

        <v-col cols="12" sm="8" md="7" lg="6" xl="4">
          <v-card>
            <v-card-title>Files list</v-card-title>
            <v-card-text>
              <FilesListing :files="corpus.files"></FilesListing>
            </v-card-text>
          </v-card>
        </v-col>
      </v-row>
    </v-container>
  </v-card>
</template>

<script>
import DonutChart from "@/components/donut_chart.js";
import FilesListing from "@/components/FilesListing.vue";
export default {
  name: "CorpusDetails",
  components: { DonutChart, FilesListing },
  props: { corpus: { required: true, type: Object } }
};
</script>
