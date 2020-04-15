<template>
  <v-system-bar
    app
    window
    height="32px"
    :dark="current_data.dark"
    :color="current_data.color"
  >
    <v-icon>{{ current_data.icon }}</v-icon>
    <span>{{ current_data.title }}</span>
  </v-system-bar>
</template>
<script>
export default {
  name: "TopBar",
  data() {
    return {
      default_values: {
        dark: true,
        color: "blue",
        title: "Corpus Harvester",
        icon: ""
      },
      Logs: {
        color: "black",
        icon: "mdi-clipboard-alert",
        title: "Logs"
      },
      Files: {
        icon: "mdi-pencil",
        title: "Files"
      },
      Corpus: { icon: "mdi-file", title: "Corpus" },
      Settings: { icon: "mdi-settings", title: "Settings" },
      Info: { icon: "mdi-information-outline", title: "Informations" }
    };
  },
  computed: {
    current_data: function() {
      let data = this.default_values;
      let current_route = this.$router.currentRoute.name;
      if (this[current_route] !== undefined) {
        for (let [key, val] of Object.entries(this[current_route])) {
          data[key] = val;
        }
      }
      return data;
    }
  }
};
</script>
