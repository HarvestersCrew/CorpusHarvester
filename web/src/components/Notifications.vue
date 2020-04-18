<template>
  <v-snackbar :color="color" v-model="displayed">
    <span>{{ msg }}</span>
    <v-spacer></v-spacer>
    <v-btn icon :dark="dark" @click="clear"><v-icon>mdi-close</v-icon></v-btn>
  </v-snackbar>
</template>

<script>
import { mapState } from "vuex";
export default {
  name: "Notifications",
  data() {
    return {
      displayed: false,
      dark: true,
      color: undefined,
      msg: undefined
    };
  },
  computed: mapState({
    notifications: "notifications"
  }),
  watch: {
    displayed: function(new_val) {
      if (new_val === false) {
        window.setTimeout(() => {
          this.$store.commit("pop_notification");
        }, 200);
      }
    },
    notifications: function() {
      if (
        this.$store.state.notifications.length > 0 &&
        this.displayed === false
      ) {
        let entry = this.$store.state.notifications[0];
        if (typeof entry === "object") {
          this.msg = entry.msg;
          this.dark = entry.dark;
          this.color = entry.color;
        } else {
          this.msg = entry;
          this.dark = true;
          this.color = undefined;
        }
        this.displayed = true;
      }
    }
  },
  methods: {
    clear() {
      this.displayed = false;
    }
  }
};
</script>
