<template>
  <Bar>
    <div>
      <SelectCard>
        <v-col cols="auto">
          <v-form @submit.prevent="search">
            <v-text-field
              dark
              outlined
              hide-details
              dense
              label="Search for..."
              prepend-icon="mdi-magnify"
              v-model="search_text"
              clearable
              :disabled="disabled"
            ></v-text-field>
          </v-form>
        </v-col>
        <v-divider vertical dark></v-divider>
        <v-col cols="auto">
          <v-select
            dark
            outlined
            hide-details
            dense
            label="Sorting..."
            :items="order_items"
            prepend-icon="mdi-sort-alphabetical-variant"
            v-model="order"
            :disabled="disabled"
          ></v-select>
        </v-col>
      </SelectCard>

      <v-container v-if="$store.state.corpuses.results.length > 0">
        <v-row>
          <v-col
            cols="3"
            v-for="(corpus, idx) in $store.state.corpuses.results"
            :key="idx"
          >
            <v-card>
              <v-card-title>{{ corpus.title }}</v-card-title>
              <v-card-subtitle>{{ corpus.creation_date }}</v-card-subtitle>
              <v-card-text> {{ corpus.files.length }} file(s) </v-card-text>
              <v-card-actions>
                <v-spacer></v-spacer>
                <v-btn icon @click="open_corpus(idx)"
                  ><v-icon>mdi-chevron-double-right</v-icon></v-btn
                >
              </v-card-actions>
            </v-card>
          </v-col>
        </v-row>
      </v-container>

      <v-container v-else>
        <v-row justify="center">
          <v-col cols="10" md="6">
            <v-card>
              <v-card-title>Corpus viewer</v-card-title>
              <v-card-text>
                Start looking for corpuses by specifying (or not) a text search
                and an order, and clicking the search button.
              </v-card-text>
            </v-card>
          </v-col>
        </v-row>
      </v-container>

      <v-btn
        :loading="disabled"
        @click="search"
        fab
        fixed
        bottom
        right
        color="blue"
        dark
        x-large
      >
        <v-icon>mdi-magnify</v-icon>
      </v-btn>
    </div>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import SelectCard from "@/components/SelectCard.vue";
export default {
  name: "Corpus",
  components: { Bar, SelectCard },
  beforeDestroy() {
    this.$store.state.corpuses.order = this.order;
    this.$store.state.corpuses.search_text = this.search_text;
    this.$store.state.corpuses.disabled = this.disabled;
  },
  data() {
    return {
      order_items: [
        { text: "Default", value: "none" },
        { text: "Name", value: "name_asc" },
        { text: "Name (desc)", value: "name_desc" },
        { text: "Date", value: "date_asc" },
        { text: "Date (desc)", value: "date_desc" }
      ],
      order: this.$store.state.corpuses.order,
      search_text: this.$store.state.corpuses.search_text,
      disabled: this.$store.state.corpuses.disabled,
      selected_corpus: undefined
    };
  },
  methods: {
    open_corpus(idx) {
      this.selected_corpus = idx;
    },
    search() {
      this.disabled = true;
      let data = { order: this.order };
      if (this.search_text !== "" && this.search_text !== null)
        data.title = this.search_text;

      this.$store.dispatch("send_tokenized_request", {
        type: "get_corpuses",
        data,
        callback: this.searched
      });
    },
    searched() {
      this.disabled = false;
      this.$store.state.corpuses.disabled = this.disabled;
      this.$store.commit(
        "add_success_notification",
        "Corpuses loaded, go check the corpus tab"
      );
    }
  }
};
</script>
