<template>
  <Bar>
    <v-container>
      <v-row justify="center" class="my-4">
        <v-col cols="12" sm="8" md="7" lg="6">
          <v-card>
            <v-card-title>Corpus Harvester</v-card-title>
            <v-card-text>
              <div class="mb-4">
                This tool was made with love by a team of seven students from a
                french CS engineering school as a class project.<br />
                It is available on
                <a href="https://github.com/HarvestersCrew/CorpusHarvester"
                  >Github</a
                >
                and is licensed under the
                <a
                  href="https://github.com/HarvestersCrew/CorpusHarvester/blob/master/LICENSE"
                  >GNU GPL v3.0</a
                >
                license.
              </div>
              <div class="my-4">
                Any person wishing to help develop this project is welcomed. It
                is as simple as referencing
                <a
                  href="https://github.com/HarvestersCrew/CorpusHarvester/issues"
                  >new issues</a
                >
                as you encounter them, or fixing those issues or adding new
                complex feature and making
                <a
                  href="https://github.com/HarvestersCrew/CorpusHarvester/pulls"
                  >pull requests</a
                >.<br />
                Anyone is also free to use this program however it suits him /
                her, provided they respect the given license above.
              </div>
              <div class="my-4">
                Thanks for using the Corpus Harvester!
              </div>
              <div class="mt-4 mr-4 text-right title">
                The HarvestersCrew
              </div>
            </v-card-text>
            <v-card-actions>
              <v-spacer></v-spacer>
              <v-btn
                dark
                target="_blank"
                color="blue"
                href="https://github.com/HarvestersCrew/CorpusHarvester"
              >
                Github
                <v-icon right>mdi-github</v-icon>
              </v-btn>
            </v-card-actions>
          </v-card>
        </v-col>
      </v-row>
    </v-container>
  </Bar>
</template>

<script>
import Bar from "@/components/Bar.vue";
export default {
  name: "Team",
  components: { Bar }
};
</script>
