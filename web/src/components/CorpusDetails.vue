<template>
  <v-card>
    <v-toolbar dark color="blue">
      <slot></slot>
      <v-toolbar-title>Corpus {{ corpus.title }}</v-toolbar-title>
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
                <v-card-title>Files distribution</v-card-title>
                <v-card-subtitle
                  >Files type proportions in this corpus</v-card-subtitle
                >
                <v-card-text>
                  <DonutChart :chart-data="dataset"></DonutChart>
                </v-card-text>
              </v-card>
            </v-col>
          </v-row>
        </v-col>

        <v-col cols="12" sm="8" md="7" lg="6" xl="4">
          <v-row>
            <v-col cols="12" class="pt-0">
              <v-card>
                <v-card-title>Corpus exportation</v-card-title>
                <v-card-subtitle v-if="corpus.extraction_path !== null"
                  >Estimated file size :</v-card-subtitle
                >
                <v-card-text class="text-center">
                  <v-btn
                    dark
                    color="blue"
                    v-if="corpus.extraction_path === null"
                    >Export</v-btn
                  >
                  <span v-else>
                    {{ corpus.extraction_path }}
                  </span>
                </v-card-text>
                <v-card-actions v-if="corpus.extraction_path !== null">
                  <v-spacer></v-spacer>
                  <v-tooltip bottom>
                    <template v-slot:activator="{ on }">
                      <v-btn icon v-on="on">
                        <v-icon>mdi-download</v-icon>
                      </v-btn>
                    </template>
                    <span>Download corpus</span>
                  </v-tooltip>
                </v-card-actions>
              </v-card>
            </v-col>
            <v-col cols="12">
              <v-card>
                <v-card-title>Files list</v-card-title>
                <v-card-text>
                  10 first files
                  <FilesListing
                    :files="corpus.files.slice(0, 10)"
                    :flat="true"
                  ></FilesListing>
                </v-card-text>
              </v-card>
            </v-col>
          </v-row>
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
  props: { corpus: { required: true, type: Object } },
  data() {
    return {
      dataset: {
        labels: ["Images", "Texts"],
        datasets: [
          {
            data: [this.corpus.stats.images, this.corpus.stats.texts],
            backgroundColor: ["#77aadd", "#ee8866"]
          }
        ]
      }
    };
  }
};
</script>
