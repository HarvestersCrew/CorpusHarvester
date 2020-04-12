<template>
  <Bar>
    <v-card flat tile>
      <v-toolbar color="blue lighten-2">
        <v-row>
          <v-col cols="4">
            <v-select
              v-model="filters.selectedTypes"
              :items="types"
              label="Types"
              dark
              outlined
              multiple
              hide-details
              dense
            >
            </v-select>
          </v-col>
          <!-- TODO : allow to select one API multiple times -->
          <v-col cols="4">
            <v-select
              v-model="filters.selectedSources"
              :items="sources"
              item-text="name"
              item-value="api"
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
        <v-expansion-panels tile accordion>
          <v-expansion-panel
            v-for="(item, i) in filters.selectedSources"
            :key="i"
          >
            <v-expansion-panel-header>{{
              item.api_name
            }}</v-expansion-panel-header>
            <v-expansion-panel-content>
              <v-row>
                <v-col
                  cols="4"
                  v-for="(parameter, j) in item.parameters"
                  :key="j"
                >
                  <v-select
                    v-if="parameter.type === 'select_field'"
                    :items="parameter.options"
                    :label="parameter.name"
                    outlined
                    dense
                    hide-details
                  >
                  </v-select>
                  <v-text-field
                    v-if="parameter.type === 'text_field'"
                    :label="parameter.name"
                    outlined
                    dense
                    hide-details
                  >
                  </v-text-field>
                  <v-menu
                    v-if="parameter.type === 'date_field'"
                    :close-on-content-click="false"
                  >
                    <template v-slot:activator="{ on }">
                      <v-text-field
                        :label="parameter.name"
                        v-model="date"
                        v-on="on"
                        outlined
                        dense
                        hide-details
                      >
                      </v-text-field>
                    </template>
                    <v-date-picker v-model="date"></v-date-picker>
                  </v-menu>
                </v-col>
              </v-row>
            </v-expansion-panel-content>
          </v-expansion-panel>
        </v-expansion-panels>
      </v-card-text>
    </v-card>
    <v-list>
      <v-list-item-group multiple active-class="blue--text">
        <template v-for="(item, index) in files">
          <v-list-item :key="index">
            <v-list-item-content>
              <v-row dense>
                <v-col>{{ item.file_name }}</v-col>
                <v-col>{{ item.file_type }}</v-col>
                <v-col>{{ item.file_source }}</v-col>
                <v-col>{{ item.file_date }}</v-col>
                <!--<v-btn icon><v-icon>mdi-dots-vertical</v-icon></v-btn>-->
              </v-row>
            </v-list-item-content>
            <v-list-item-icon>
              <v-btn icon><v-icon>mdi-dots-vertical</v-icon></v-btn>
            </v-list-item-icon>
          </v-list-item>
          <v-divider v-if="index + 1 < files.length" :key="index"></v-divider>
        </template>
      </v-list-item-group>

      <v-fab-transition>
        <v-btn fab large fixed bottom right color="blue" dark>
          <v-icon>mdi-check</v-icon>
        </v-btn>
      </v-fab-transition>
    </v-list>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
export default {
  name: "Files",
  components: { Bar },
  data() {
    return {
      files: [
        {
          file_name: "A.txt",
          file_type: "Text",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "B.txt",
          file_type: "Text",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "C.txt",
          file_type: "Text",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "D.png",
          file_type: "Image",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "E.png",
          file_type: "Image",
          file_source: "Twitter",
          file_date: new Date().toISOString().substring(0, 10)
        },
        {
          file_name: "F.txt",
          file_type: "Text",
          file_source: "TMDB",
          file_date: new Date().toISOString().substring(0, 10)
        }
      ],
      types: ["Text", "Image"],
      sources: [
        {
          name: "Twitter",
          api: {
            api_name: "Twitter",
            parameters: [
              {
                name: "nb_retweets",
                type: "select_field",
                options: ["< 500", "> 500"]
              },
              {
                name: "word",
                type: "text_field"
              },
              {
                name: "tweet_date",
                type: "date_field"
              },
              {
                name: "word2",
                type: "text_field"
              }
            ]
          }
        },
        {
          name: "TMDB",
          api: {
            api_name: "TMDB",
            parameters: [
              {
                name: "word",
                type: "text_field"
              }
            ]
          }
        }
      ],
      filters: {
        date: undefined,
        selectedSources: [],
        selectedTypes: []
      }
    };
  }
};
</script>
