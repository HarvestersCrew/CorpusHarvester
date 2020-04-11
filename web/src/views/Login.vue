<template>
  <v-content>
    <v-container class="fill-height" fluid>
      <v-row align="center" justify="center">
        <v-col cols="12" sm="8" md="4">
          <v-card class="elevation-12">
            <v-toolbar color="primary" dark flat>
              <v-toolbar-title>Corpus Harvester</v-toolbar-title>
            </v-toolbar>

            <v-form @submit.prevent="connect">
              <v-card-text>
                <v-text-field
                  autofocus
                  clearable
                  persistent-hint
                  hint="URL of the Harvester server"
                  label="URL"
                  v-model="url"
                />
              </v-card-text>
              <v-card-actions>
                <v-spacer />
                <v-btn color="primary" @click="connect">Connect</v-btn>
              </v-card-actions>
            </v-form>
          </v-card>
        </v-col>
      </v-row>
    </v-container>
  </v-content>
</template>
<script>
export default {
  data() {
    return {
      url: ""
    };
  },
  name: "Login",
  methods: {
    connect() {
      console.log(this.url);
      this.$router.push("/home");
    }
  }
};
</script>
