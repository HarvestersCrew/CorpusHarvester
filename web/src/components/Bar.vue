<template>
  <div>
    <v-navigation-drawer
      app
      v-model="drawer"
      color="blue"
      mini-variant
      permanent
    >
      <v-list dense nav>
        <v-list-item
          v-for="item in items"
          :key="item.title"
          :to="{ name: item.title }"
          link
        >
          <v-list-item-icon>
            <v-tooltip right>
              <template v-slot:activator="{ on }">
                <v-icon color="white" v-on="on">{{ item.icon }}</v-icon>
              </template>
              <span>{{ item.title }}</span>
            </v-tooltip>
          </v-list-item-icon>

          <v-list-item-content>
            <v-list-item-title>{{ item.title }}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>
      </v-list>

      <template v-slot:append>
        <v-list dense nav>
          <v-list-item
            v-for="item in items2"
            :key="item.title"
            :to="{ name: item.title }"
            link
          >
            <v-list-item-icon>
              <v-tooltip right>
                <template v-slot:activator="{ on }">
                  <v-icon color="white" v-on="on">{{ item.icon }}</v-icon>
                </template>
                <span>{{ item.title }}</span>
              </v-tooltip>
            </v-list-item-icon>

            <v-list-item-content>
              <v-list-item-title>{{ item.title }}</v-list-item-title>
            </v-list-item-content>
          </v-list-item>
        </v-list>
      </template>
    </v-navigation-drawer>

    <v-content>
      <slot></slot>
    </v-content>
  </div>
</template>

<script>
export default {
  beforeCreate() {
    // Will check presence in local storage of a server
    if (this.$store.state.socket.url === undefined) {
      this.$router.push("login");
    }
  },
  data() {
    return {
      drawer: true,
      items: [
        { title: "Files", icon: "mdi-pencil" },
        { title: "Corpus", icon: "mdi-file" }
      ],
      items2: [
        { title: "Settings", icon: "mdi-settings" },
        { title: "Info", icon: "mdi-information-outline" }
      ]
    };
  }
};
</script>
