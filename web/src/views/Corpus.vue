<template>
  <Bar>
    <div>
      <SelectCard>
        <v-col cols="5" lg="auto">
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
        <v-col cols="5" lg="auto">
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
            cols="6"
            md="4"
            lg="3"
            v-for="(corpus, idx) in $store.state.corpuses.results"
            :key="idx"
          >
            <v-card>
              <v-card-title>
                <template v-if="editing_corpus_name != corpus.id">
                  {{ corpus.title }}
                  <v-spacer></v-spacer>
                  <v-btn
                    :disabled="editing_corpus_name_loading"
                    icon
                    @click="open_editing_corpus_name(corpus.id)"
                  >
                    <v-icon>mdi-pencil</v-icon>
                  </v-btn>
                </template>

                <template v-else>
                  <v-text-field
                    dense
                    :placeholder="corpus.title"
                    outlined
                    hide-details
                    v-model="editing_corpus_name_val"
                    :loading="editing_corpus_name_loading"
                  ></v-text-field>
                  <v-row justify="end">
                    <v-btn
                      icon
                      @click="close_editing_corpus_name"
                      :disabled="editing_corpus_name_loading"
                    >
                      <v-icon color="red">mdi-close</v-icon>
                    </v-btn>
                    <v-btn
                      icon
                      @click="send_editing_corpus_name(corpus.id)"
                      :disabled="editing_corpus_name_loading"
                    >
                      <v-icon color="green">mdi-check</v-icon>
                    </v-btn>
                  </v-row>
                </template>
              </v-card-title>

              <v-card-subtitle class="pb-0">
                #{{ corpus.id }}
                <br />
                {{ corpus.creation_date }}
                <br />
                {{ corpus.files.length }} file(s)
              </v-card-subtitle>
              <v-card-actions>
                <v-tooltip bottom>
                  <template v-slot:activator="{ on }">
                    <v-btn icon v-on="on"><v-icon>mdi-delete</v-icon></v-btn>
                  </template>
                  <span>Delete corpus</span>
                </v-tooltip>
                <v-tooltip bottom>
                  <template v-slot:activator="{ on }">
                    <v-btn icon v-on="on"
                      ><v-icon>mdi-folder-move</v-icon></v-btn
                    >
                  </template>
                  <span>Export corpus</span>
                </v-tooltip>
                <v-spacer></v-spacer>
                <v-btn icon @click="open_corpus(corpus.id)">
                  <v-icon v-if="selected_corpus === corpus.id" color="blue">
                    mdi-chevron-double-down
                  </v-icon>
                  <v-icon v-else>mdi-chevron-double-right</v-icon>
                </v-btn>
              </v-card-actions>
              <v-expand-transition>
                <v-card-text
                  v-show="selected_corpus === corpus.id"
                  class="pt-0"
                >
                  <v-row class="text-center">
                    <v-col
                      cols="3"
                      v-for="(file, index) in corpus.files.slice(0, 11)"
                      :key="index"
                    >
                      <v-tooltip right>
                        <template v-slot:activator="{ on }">
                          <v-icon v-on="on">{{
                            file_icon(file.format)
                          }}</v-icon>
                        </template>
                        <span> {{ file.name }} </span>
                      </v-tooltip>
                    </v-col>
                    <v-tooltip right v-if="corpus.files.length > 11">
                      <template v-slot:activator="{ on }">
                        <v-col cols="3" class="">
                          <v-btn icon>
                            <v-icon v-on="on"
                              >mdi-dots-horizontal</v-icon
                            ></v-btn
                          ></v-col
                        >
                      </template>
                      <span>+ {{ corpus.files.length - 11 }} files</span>
                    </v-tooltip>
                  </v-row>
                </v-card-text>
              </v-expand-transition>
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
      selected_corpus: undefined,
      editing_corpus_name: undefined,
      editing_corpus_name_val: undefined,
      editing_corpus_name_loading: false
    };
  },
  methods: {
    open_corpus(idx) {
      this.selected_corpus === idx
        ? (this.selected_corpus = undefined)
        : (this.selected_corpus = idx);
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
    },
    file_icon(file_format) {
      switch (file_format) {
        case "txt":
          return "mdi-file-document";
        case "jpg":
        case "png":
          return "mdi-file-image";
        default:
          return "mdi-file";
      }
    },
    open_editing_corpus_name(idx) {
      this.editing_corpus_name = idx;
      this.editing_corpus_name_val = undefined;
    },
    close_editing_corpus_name() {
      this.editing_corpus_name = undefined;
    },
    send_editing_corpus_name(idx) {
      this.editing_corpus_name_loading = true;
      let data = { id: idx, title: this.editing_corpus_name_val };
      this.$store.dispatch("send_tokenized_request", {
        type: "set_corpus_title",
        data,
        callback: this.callback_editing_corpus_name
      });
    },
    callback_editing_corpus_name(data) {
      this.editing_corpus_name_loading = false;
      if (data.type !== undefined && data.type === "error") {
        this.$store.commit(
          "add_error_notification",
          "An error occurred while editing the corpus name, please check the logs"
        );
        return;
      }
      this.close_editing_corpus_name();
    }
  }
};
</script>
