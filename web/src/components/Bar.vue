<template>
  <div>
    <v-navigation-drawer app color="blue" mini-variant permanent>
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
                <v-badge
                  :value="
                    item.badge !== undefined && own_getter(item.badge) !== 0
                  "
                  color="red"
                  overlap
                  dot
                >
                  <v-icon color="white" v-on="on">{{ item.icon }}</v-icon>
                </v-badge>
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

          <v-list-item>
            <v-list-item-icon>
              <v-tooltip right>
                <template v-slot:activator="{ on }">
                  <v-icon color="red" v-on="on" @click="logout"
                    >mdi-power</v-icon
                  >
                </template>
                <span>Quit</span>
              </v-tooltip>
            </v-list-item-icon>
          </v-list-item>
        </v-list>
      </template>
    </v-navigation-drawer>

    <TopBar></TopBar>

    <v-content style="height:100vh">
      <slot></slot>
    </v-content>

    <Notifications></Notifications>
  </div>
</template>

<script>
import TopBar from "@/components/TopBar.vue";
import Notifications from "@/components/Notifications.vue";
export default {
  components: { TopBar, Notifications },
  beforeCreate() {
    // Will check presence in local storage of a server
    if (this.$store.state.socket.url === undefined) {
      this.$store.commit("set_redirect_page", this.$router.currentRoute.name);
      this.$router.push("login");
    } else if (!this.$store.state.first_init_done) {
      this.$store.dispatch("send_request", {
        type: "get_apis_infos",
        data: {}
      });
      this.$store.dispatch("send_request", {
        type: "get_logger_infos",
        data: {}
      });
      this.$store.dispatch("send_request", {
        type: "get_storage_path",
        data: {}
      });
      this.$store.commit("set_first_init_done", true);
    }
  },
  data() {
    return {
      items: [
        { title: "Files", icon: "mdi-pencil" },
        { title: "Corpus", icon: "mdi-file" },
        {
          title: "Logs",
          icon: "mdi-clipboard-alert",
          badge: "unread"
        }
      ],
      items2: [
        { title: "Settings", icon: "mdi-settings" },
        { title: "Info", icon: "mdi-information-outline" }
      ]
    };
  },
  methods: {
    own_getter(type) {
      if (type === "unread") return this.$store.state.logs.unread;
      else return undefined;
    },
    logout() {
      this.$store.dispatch("disconnect_server");
    }
  }
};
</script>
