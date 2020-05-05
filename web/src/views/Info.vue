<template>
  <Bar>
    <v-container>
      <v-row justify="center" class="my-4">
        <v-col cols="12" sm="8" md="7" lg="6" xl="4">
          <v-card>
            <v-card-title>Files #</v-card-title>
            <v-card-subtitle>
              Currently, {{ files }} files are stored in the database.
            </v-card-subtitle>
            <v-card-text>
              <DonutChart
                :chart-data="files_dataset"
                :options="files_options"
              ></DonutChart>
            </v-card-text>
          </v-card>
        </v-col>

        <v-col cols="12" sm="8" md="7" lg="6" xl="4">
          <v-row>
            <v-col cols="12" class="pt-0">
              <v-card>
                <v-card-title>Corpus #</v-card-title>
                <v-card-text class="px-4">
                  <div class="display-3">{{ corpus }}</div>
                  corpus in the database
                </v-card-text>
              </v-card>
            </v-col>

            <v-col cols="12">
              <v-card>
                <v-card-title>Storage size</v-card-title>
                <v-card-text class="px-4">
                  <div class="display-2">{{ storage_size }}</div>
                  taken on disk by the storage
                </v-card-text>
              </v-card>
            </v-col>
          </v-row>
        </v-col>
      </v-row>
    </v-container>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import DonutChart from "@/components/donut_chart.js";
export default {
  name: "Info",
  components: { Bar, DonutChart },
  data() {
    return {
      corpus: 0,
      files: 0,
      images: 0,
      texts: 0,
      bytes: 0,
      files_dataset: {},
      files_options: { legend: { labels: { padding: 50 } } }
    };
  },
  mounted() {
    this.$store.dispatch("send_tokenized_request", {
      type: "get_db_stats",
      data: {},
      callback: this.stats_loaded
    });
  },
  computed: {
    storage_size() {
      let units = ["B", "KB", "MB", "GB", "TB"];
      let divided_size = this.bytes;
      let str = "~ ";
      let i;
      for (i = 0; i < units.length - 1; ++i) {
        if (divided_size < 1000) {
          break;
        }
        divided_size /= 1000;
      }
      str += Math.round(divided_size * 100) / 100 + " " + units[i];
      return str;
    }
  },
  methods: {
    fill_datasets() {
      this.files_dataset = {
        labels: ["Images", "Texts"],
        datasets: [
          {
            data: [this.images, this.texts],
            backgroundColor: ["#77aadd", "#ee8866"]
          }
        ]
      };
    },
    stats_loaded(data) {
      let stats = data.data.stats;
      this.corpus = stats.corpus_count;
      this.files = stats.file_count;
      this.images = stats.image_count;
      this.texts = stats.text_count;
      this.bytes = stats.total_size;
      this.fill_datasets();
    }
  }
};
</script>
