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
              <v-card-title class="mb-2">
                <v-slide-x-transition hide-on-leave>
                  <v-row
                    dense
                    no-gutters
                    v-show="editing_corpus_name !== corpus.id"
                    class="ml-0"
                  >
                    <v-col>
                      {{ corpus.title }}
                    </v-col>
                    <v-col cols="auto">
                      <v-btn
                        :disabled="disabled"
                        icon
                        @click="open_editing_corpus_name(corpus.id)"
                      >
                        <v-icon>mdi-pencil</v-icon>
                      </v-btn>
                    </v-col>
                  </v-row>
                </v-slide-x-transition>

                <v-slide-x-transition hide-on-leave>
                  <v-row
                    dense
                    no-gutters
                    v-show="editing_corpus_name === corpus.id"
                  >
                    <v-col>
                      <v-text-field
                        dense
                        :placeholder="corpus.title"
                        outlined
                        hide-details
                        v-model="editing_corpus_name_val"
                        :loading="disabled"
                        :disabled="disabled"
                      ></v-text-field>
                    </v-col>
                    <v-col cols="auto" class="ml-2">
                      <v-btn
                        icon
                        @click="close_editing_corpus_name"
                        :disabled="disabled"
                      >
                        <v-icon color="red">mdi-close</v-icon>
                      </v-btn>
                      <v-btn
                        icon
                        @click="send_editing_corpus_name(corpus.id)"
                        :disabled="disabled"
                      >
                        <v-icon color="green">mdi-check</v-icon>
                      </v-btn>
                    </v-col>
                  </v-row>
                </v-slide-x-transition>
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
                    <v-btn
                      icon
                      v-on="on"
                      :disabled="disabled"
                      @click="open_delete_corpus_modal(corpus.id)"
                    >
                      <v-icon>mdi-delete</v-icon>
                    </v-btn>
                  </template>
                  <span>Delete corpus</span>
                </v-tooltip>
                <v-tooltip bottom>
                  <template v-slot:activator="{ on }">
                    <v-btn icon v-on="on" :disabled="disabled"
                      ><v-icon>mdi-folder-move</v-icon></v-btn
                    >
                  </template>
                  <span>Export corpus</span>
                </v-tooltip>

                <v-spacer></v-spacer>

                <v-tooltip bottom>
                  <template v-slot:activator="{ on }">
                    <v-btn
                      icon
                      :disabled="disabled"
                      v-on="on"
                      @click="corpus_dialog_data = corpus"
                    >
                      <v-icon>mdi-dots-vertical</v-icon>
                    </v-btn>
                  </template>
                  <span>See more</span>
                </v-tooltip>
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

    <v-dialog v-model="deleting_corpus_modal" max-width="500">
      <v-card>
        <v-card-title>Deleting corpus #{{ deleting_corpus_val }}</v-card-title>
        <v-card-text class="text-left">
          You are going to delete corpus #{{ deleting_corpus_val }} from the
          database. All files in it will be kept. Are you sure?
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn
            text
            @click="deleting_corpus_modal = false"
            :disabled="disabled"
            >Cancel</v-btn
          >
          <v-btn text color="red" :loading="disabled" @click="delete_corpus"
            >Proceed</v-btn
          >
        </v-card-actions>
      </v-card>
    </v-dialog>

    <v-dialog
      v-model="corpus_dialog"
      fullscreen
      hide-overlay
      persistent
      transition="dialog-bottom-transition"
    >
      <CorpusDetails :corpus="corpus_dialog_data">
        <v-btn icon dark @click="corpus_dialog_data = undefined">
          <v-icon>mdi-close</v-icon>
        </v-btn>
      </CorpusDetails>
    </v-dialog>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
import SelectCard from "@/components/SelectCard.vue";
import CorpusDetails from "@/components/CorpusDetails.vue";
export default {
  name: "Corpus",
  components: { Bar, SelectCard, CorpusDetails },
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
      deleting_corpus_modal: false,
      deleting_corpus_val: undefined,
      corpus_dialog_data: undefined
    };
  },
  computed: {
    corpus_dialog() {
      return this.corpus_dialog_data !== undefined;
    }
  },
  methods: {
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
      this.disabled = true;
      let data = { id: idx, title: this.editing_corpus_name_val };
      this.$store.dispatch("send_tokenized_request", {
        type: "set_corpus_title",
        data,
        callback: this.callback_editing_corpus_name
      });
    },
    callback_editing_corpus_name(data) {
      this.disabled = false;
      if (data.type !== undefined && data.type === "error") {
        this.$store.commit(
          "add_error_notification",
          "An error occurred while editing the corpus name, please check the logs"
        );
        return;
      }
      this.close_editing_corpus_name();
    },
    open_delete_corpus_modal(id) {
      this.deleting_corpus_modal = true;
      this.deleting_corpus_val = id;
    },
    delete_corpus() {
      this.disabled = true;
      let data = { id: this.deleting_corpus_val };
      this.$store.dispatch("send_tokenized_request", {
        type: "delete_corpus",
        data,
        callback: this.callback_delete_corpus
      });
    },
    callback_delete_corpus(data) {
      this.disabled = false;
      this.deleting_corpus_modal = false;
      if (data.type !== undefined && data.type === "error") {
        this.$store.commit(
          "add_error_notification",
          "An error occurred while deleting a corpus, please check the logs"
        );
        return;
      }
    }
  }
};
</script>
