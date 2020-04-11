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
        <v-list-item v-for="item in items" :key="item.title" link>
          <v-list-item-icon>
            <v-icon color="white">{{ item.icon }}</v-icon>
          </v-list-item-icon>

          <v-list-item-content>
            <v-list-item-title>{{ item.title }}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>

        <v-divider class="my-2" light></v-divider>

        <v-list-item v-for="item in items2" :key="item.title" link>
          <v-list-item-icon>
            <v-icon color="white">{{ item.icon }}</v-icon>
          </v-list-item-icon>

          <v-list-item-content>
            <v-list-item-title>{{ item.title }}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>
      </v-list>
    </v-navigation-drawer>

    <v-content>
      <v-container>
        <slot></slot>
      </v-container>
    </v-content>
  </div>
</template>

<script>
export default {
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
